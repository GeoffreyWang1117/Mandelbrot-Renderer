const express = require('express');
const compression = require('compression');
const cors = require('cors');
const { execFile } = require('child_process');
const path = require('path');
const sharp = require('sharp');

const app = express();
const PORT = process.env.PORT || 3000;
const BINARY_PATH = process.env.FRACTAL_BIN || path.join(__dirname, '..', 'build', 'fractal_api');

// Concurrency limiter for render requests (prevents OOM on small instances)
const MAX_CONCURRENT_RENDERS = parseInt(process.env.MAX_RENDERS) || 2;
let activeRenders = 0;

app.use(cors());
app.use(compression());
app.use(express.json());

// Health check
app.get('/api/health', (req, res) => {
    res.json({ status: 'ok', version: '1.0.0', renderer: 'fractal-api' });
});

// Server-side fractal rendering API
// Returns PNG image rendered by C++ backend
app.get('/api/render', (req, res) => {
    const {
        fractal = 'mandelbrot',
        width = '800',
        height = '600',
        cx = '-0.5',
        cy = '0.0',
        zoom = '1.0',
        iter = '1000',
        juliaReal = '-0.7269',
        juliaImag = '0.1889',
        format = 'png'
    } = req.query;

    // Validate dimensions (cap at 4K for safety)
    const w = Math.min(Math.max(parseInt(width) || 800, 1), 3840);
    const h = Math.min(Math.max(parseInt(height) || 600, 1), 2160);
    const maxIter = Math.min(Math.max(parseInt(iter) || 1000, 50), 10000);
    const zoomVal = Math.max(parseFloat(zoom) || 1.0, 0.001);

    const validFractals = ['mandelbrot', 'julia', 'burning_ship', 'newton', 'tricorn', 'phoenix'];
    if (!validFractals.includes(fractal)) {
        return res.status(400).json({ error: 'Invalid fractal type' });
    }

    // Concurrency guard — reject if too many renders in-flight
    if (activeRenders >= MAX_CONCURRENT_RENDERS) {
        return res.status(503).json({
            error: 'Server busy',
            detail: `${activeRenders} renders in progress, max ${MAX_CONCURRENT_RENDERS}`
        });
    }
    activeRenders++;

    const args = [
        '--fractal', fractal,
        '--width', String(w),
        '--height', String(h),
        '--cx', String(parseFloat(cx) || 0),
        '--cy', String(parseFloat(cy) || 0),
        '--zoom', String(zoomVal),
        '--iter', String(maxIter)
    ];

    if (fractal === 'julia') {
        args.push('--julia-real', String(parseFloat(juliaReal) || -0.7269));
        args.push('--julia-imag', String(parseFloat(juliaImag) || 0.1889));
    }

    const { phoenixPx = '0.5667', phoenixPy = '0.0' } = req.query;
    if (fractal === 'phoenix') {
        args.push('--phoenix-px', String(parseFloat(phoenixPx) || 0.5667));
        args.push('--phoenix-py', String(parseFloat(phoenixPy) || 0.0));
    }

    execFile(BINARY_PATH, args, {
        encoding: 'buffer',
        maxBuffer: 30 * 1024 * 1024, // 30MB — enough for 4K PPM (24MB)
        timeout: 30000 // 30s timeout
    }, async (err, stdout, stderr) => {
        if (err) {
            activeRenders--;
            console.error('Render failed:', err.message);
            if (stderr && stderr.length > 0) {
                console.error('stderr:', stderr.toString());
            }
            return res.status(500).json({
                error: 'Render failed',
                details: err.message
            });
        }

        try {
            // Convert PPM to requested format
            if (format === 'ppm') {
                res.set('Content-Type', 'image/x-portable-pixmap');
                res.set('Content-Disposition',
                    `inline; filename="${fractal}_${w}x${h}.ppm"`);
                return res.send(stdout);
            }

            // Parse PPM header to get dimensions for sharp
            // PPM format: "P6\nWIDTH HEIGHT\n255\n" followed by raw RGB data
            const headerEnd = findPpmDataStart(stdout);
            const rawPixels = stdout.slice(headerEnd);

            const image = sharp(rawPixels, {
                raw: { width: w, height: h, channels: 3 }
            });

            let buffer;
            if (format === 'webp') {
                buffer = await image.webp({ quality: 90 }).toBuffer();
                res.set('Content-Type', 'image/webp');
            } else if (format === 'jpeg' || format === 'jpg') {
                buffer = await image.jpeg({ quality: 92 }).toBuffer();
                res.set('Content-Type', 'image/jpeg');
            } else {
                // Default to PNG
                buffer = await image.png({ compressionLevel: 2 }).toBuffer();
                res.set('Content-Type', 'image/png');
            }

            res.set('Content-Disposition',
                `inline; filename="${fractal}_${w}x${h}.${format === 'jpg' ? 'jpg' : format}"`);
            res.set('Cache-Control', 'public, max-age=3600');
            res.send(buffer);
        } catch (convErr) {
            console.error('Image conversion failed:', convErr.message);
            res.status(500).json({ error: 'Image conversion failed', details: convErr.message });
        } finally {
            activeRenders--;
        }
    });
});

// Serve high-resolution wallpaper presets
app.get('/api/wallpaper/:preset', (req, res) => {
    const presets = {
        'mandelbrot-classic': {
            fractal: 'mandelbrot', cx: '-0.5', cy: '0', zoom: '1', iter: '1000'
        },
        'mandelbrot-spiral': {
            fractal: 'mandelbrot', cx: '-0.7269', cy: '0.1889', zoom: '100', iter: '2000'
        },
        'mandelbrot-seahorse': {
            fractal: 'mandelbrot', cx: '-0.7453', cy: '0.1127', zoom: '200', iter: '2000'
        },
        'julia-dragon': {
            fractal: 'julia', cx: '0', cy: '0', zoom: '1', iter: '1000',
            juliaReal: '-0.7269', juliaImag: '0.1889'
        },
        'julia-galaxy': {
            fractal: 'julia', cx: '0', cy: '0', zoom: '1', iter: '1000',
            juliaReal: '-0.8', juliaImag: '0.156'
        },
        'burning-ship': {
            fractal: 'burning_ship', cx: '-0.5', cy: '-0.5', zoom: '1', iter: '1000'
        },
        'newton': {
            fractal: 'newton', cx: '0', cy: '0', zoom: '1', iter: '200'
        },
        'tricorn': {
            fractal: 'tricorn', cx: '-0.3', cy: '0', zoom: '1', iter: '1000'
        },
        'phoenix': {
            fractal: 'phoenix', cx: '0', cy: '0', zoom: '1', iter: '1000',
            phoenixPx: '0.5667', phoenixPy: '0'
        }
    };

    const preset = presets[req.params.preset];
    if (!preset) {
        return res.status(404).json({
            error: 'Unknown preset',
            available: Object.keys(presets)
        });
    }

    const resolution = req.query.resolution || '1920x1080';
    const [width, height] = resolution.split('x').map(Number);
    const format = req.query.format || 'png';

    const params = new URLSearchParams({
        ...preset,
        width: String(Math.min(width || 1920, 3840)),
        height: String(Math.min(height || 1080, 2160)),
        format
    });

    // Internal redirect via query rewrite (avoids client redirect)
    req.url = '/api/render?' + params.toString();
    req.query = Object.fromEntries(params.entries());
    app.handle(req, res);
});

// List available API endpoints
app.get('/api', (req, res) => {
    res.json({
        name: 'Fractal Renderer API',
        version: '1.0.0',
        endpoints: {
            'GET /api/health': 'Health check',
            'GET /api/render': 'Render fractal image (params: fractal, width, height, cx, cy, zoom, iter, format)',
            'GET /api/wallpaper/:preset': 'High-res wallpaper presets (params: resolution, format)',
        },
        fractals: ['mandelbrot', 'julia', 'burning_ship', 'newton'],
        formats: ['png', 'webp', 'jpeg', 'ppm'],
        maxResolution: '3840x2160'
    });
});

// Parse PPM header to find start of pixel data
function findPpmDataStart(buffer) {
    let pos = 0;
    let newlines = 0;
    // PPM P6 has 3 lines of header: "P6\n", "WIDTH HEIGHT\n", "255\n"
    while (pos < buffer.length && newlines < 3) {
        if (buffer[pos] === 0x0A) newlines++;
        pos++;
    }
    return pos;
}

app.listen(PORT, '0.0.0.0', () => {
    console.log(`Fractal Renderer API server running on port ${PORT}`);
    console.log(`Binary path: ${BINARY_PATH}`);
    console.log(`Endpoints:`);
    console.log(`  GET /api/health`);
    console.log(`  GET /api/render?fractal=mandelbrot&width=1920&height=1080`);
    console.log(`  GET /api/wallpaper/mandelbrot-classic?resolution=3840x2160`);
});
