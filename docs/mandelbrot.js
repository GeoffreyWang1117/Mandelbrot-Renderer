// Multi-Fractal Web Renderer - Interactive JavaScript Implementation
// High-performance web demo with progressive rendering
// Supports: Mandelbrot, Julia, Burning Ship, Newton, Tricorn, Phoenix
//
// Author: Geoffrey Wang (with Claude AI assistance)
// Repository: https://github.com/GeoffreyWang1117/Mandelbrot-Renderer

class FractalRenderer {
    constructor() {
        this.canvas = document.getElementById('mandelbrot-canvas');
        if (!this.canvas) throw new Error('Canvas element not found');

        this.ctx = this.canvas.getContext('2d');
        if (!this.ctx) throw new Error('2D context not available');

        this.width = 800;
        this.height = 600;

        // Fractal parameters
        this.fractalType = 'mandelbrot';
        this.centerX = -0.5;
        this.centerY = 0.0;
        this.zoom = 1.0;
        this.maxIterations = 100;

        // Julia set parameters
        this.juliaC = { x: -0.7269, y: 0.1889 };

        // Phoenix parameters
        this.phoenixP = { x: 0.5667, y: 0.0 };

        // Rendering state
        this.isRendering = false;
        this.renderCancelled = false;
        this.imageData = null;

        // Performance tracking
        this.renderStartTime = 0;
        this.pixelsRendered = 0;

        this.initializeUI();
        this.setupEventListeners();
        this.loadStateFromURL();
        this.render();
    }

    initializeUI() {
        this.canvas.width = this.width;
        this.canvas.height = this.height;
        this.imageData = this.ctx.createImageData(this.width, this.height);
        this.ctx.fillStyle = '#000';
        this.ctx.fillRect(0, 0, this.width, this.height);
    }

    setupEventListeners() {
        this.canvas.addEventListener('click', (e) => this.handleCanvasClick(e));
        this.canvas.addEventListener('mousedown', (e) => this.handleMouseDown(e));
        this.canvas.addEventListener('wheel', (e) => this.handleWheel(e), { passive: false });
        this.canvas.addEventListener('contextmenu', (e) => e.preventDefault());

        document.getElementById('fractal-type').addEventListener('change', (e) => {
            this.fractalType = e.target.value;
            this.updateFractalControls();
            this.render();
        });

        document.getElementById('julia-cx').addEventListener('input', (e) => {
            this.juliaC.x = parseFloat(e.target.value);
            document.getElementById('julia-cx-value').textContent = e.target.value;
            if (this.fractalType === 'julia') this.render();
        });

        document.getElementById('julia-cy').addEventListener('input', (e) => {
            this.juliaC.y = parseFloat(e.target.value);
            document.getElementById('julia-cy-value').textContent = e.target.value;
            if (this.fractalType === 'julia') this.render();
        });

        const phoenixPx = document.getElementById('phoenix-px');
        if (phoenixPx) {
            phoenixPx.addEventListener('input', (e) => {
                this.phoenixP.x = parseFloat(e.target.value);
                document.getElementById('phoenix-px-value').textContent = e.target.value;
                if (this.fractalType === 'phoenix') this.render();
            });
        }
        const phoenixPy = document.getElementById('phoenix-py');
        if (phoenixPy) {
            phoenixPy.addEventListener('input', (e) => {
                this.phoenixP.y = parseFloat(e.target.value);
                document.getElementById('phoenix-py-value').textContent = e.target.value;
                if (this.fractalType === 'phoenix') this.render();
            });
        }

        document.getElementById('iterations').addEventListener('input', (e) => {
            this.maxIterations = parseInt(e.target.value);
            document.getElementById('iterations-value').textContent = e.target.value;
        });

        document.getElementById('zoom').addEventListener('input', (e) => {
            this.zoom = parseFloat(e.target.value);
            document.getElementById('zoom-value').textContent = e.target.value + 'x';
        });

        document.getElementById('render-btn').addEventListener('click', () => this.render());
        document.getElementById('reset-btn').addEventListener('click', () => this.resetView());
        document.getElementById('save-btn').addEventListener('click', () => this.saveImage());
        document.getElementById('share-btn').addEventListener('click', () => this.shareView());

        document.getElementById('classic-btn').addEventListener('click', () => this.loadPreset('classic'));
        document.getElementById('spiral-btn').addEventListener('click', () => this.loadPreset('spiral'));
        document.getElementById('seahorse-btn').addEventListener('click', () => this.loadPreset('seahorse'));
        document.getElementById('elephant-btn').addEventListener('click', () => this.loadPreset('elephant'));
        document.getElementById('galaxy-btn').addEventListener('click', () => this.loadPreset('galaxy'));
        document.getElementById('lightning-btn').addEventListener('click', () => this.loadPreset('lightning'));

        document.getElementById('save-favorite-btn').addEventListener('click', () => this.saveFavorite());
        document.getElementById('manage-favorites-btn').addEventListener('click', () => this.manageFavorites());

        const serverRenderBtn = document.getElementById('server-render-btn');
        if (serverRenderBtn) {
            serverRenderBtn.addEventListener('click', () => this.serverRender());
        }
        this.checkApiServer();
    }

    // --- Scroll wheel zoom ---
    handleWheel(e) {
        e.preventDefault();
        if (this.isRendering) return;

        const rect = this.canvas.getBoundingClientRect();
        const mx = e.clientX - rect.left;
        const my = e.clientY - rect.top;

        const scale = 4.0 / this.zoom;
        const worldX = this.centerX + (mx - this.width / 2) * scale / this.width;
        const worldY = this.centerY + (my - this.height / 2) * scale / this.height;

        const factor = e.deltaY < 0 ? 1.3 : 1 / 1.3;
        this.zoom *= factor;

        // Re-center so the point under cursor stays fixed
        const newScale = 4.0 / this.zoom;
        this.centerX = worldX - (mx - this.width / 2) * newScale / this.width;
        this.centerY = worldY - (my - this.height / 2) * newScale / this.height;

        if (this.zoom <= 1000) {
            document.getElementById('zoom').value = this.zoom;
            document.getElementById('zoom-value').textContent = this.zoom.toFixed(1) + 'x';
        }
        this.render();
    }

    async checkApiServer() {
        const group = document.getElementById('server-render-group');
        if (!group) return;
        try {
            const resp = await fetch('/api/health', { signal: AbortSignal.timeout(2000) });
            if (resp.ok) {
                group.style.display = 'block';
                this.apiAvailable = true;
            } else {
                group.style.display = 'none';
            }
        } catch {
            group.style.display = 'none';
            this.apiAvailable = false;
        }
    }

    async serverRender() {
        if (!this.apiAvailable) {
            alert('Server API is not available. Make sure the API server is running.');
            return;
        }
        const resolution = document.getElementById('export-resolution').value;
        const [width, height] = resolution.split('x');
        const params = new URLSearchParams({
            fractal: this.fractalType, width, height,
            cx: this.centerX.toString(), cy: this.centerY.toString(),
            zoom: this.zoom.toString(), iter: this.maxIterations.toString(),
            format: 'png'
        });
        if (this.fractalType === 'julia') {
            params.set('juliaReal', this.juliaC.x.toString());
            params.set('juliaImag', this.juliaC.y.toString());
        }
        if (this.fractalType === 'phoenix') {
            params.set('phoenixPx', this.phoenixP.x.toString());
            params.set('phoenixPy', this.phoenixP.y.toString());
        }
        const btn = document.getElementById('server-render-btn');
        btn.textContent = 'Rendering on server...';
        btn.disabled = true;
        try {
            const resp = await fetch('/api/render?' + params.toString());
            if (!resp.ok) {
                const err = await resp.json().catch(() => ({ error: 'Unknown error' }));
                throw new Error(err.error || resp.statusText);
            }
            const blob = await resp.blob();
            const url = URL.createObjectURL(blob);
            const link = document.createElement('a');
            link.download = `${this.fractalType}_${width}x${height}.png`;
            link.href = url;
            link.click();
            URL.revokeObjectURL(url);
        } catch (err) {
            alert('Server render failed: ' + err.message);
        } finally {
            btn.textContent = 'Server HD Export';
            btn.disabled = false;
        }
    }

    handleCanvasClick(e) {
        if (this.isRendering) return;
        const rect = this.canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;
        const scale = 4.0 / this.zoom;
        this.centerX += (x - this.width / 2) * scale / this.width;
        this.centerY += (y - this.height / 2) * scale / this.height;
        this.zoom *= 2;
        if (this.zoom <= 1000) {
            document.getElementById('zoom').value = this.zoom;
            document.getElementById('zoom-value').textContent = this.zoom.toFixed(1) + 'x';
        }
        this.render();
    }

    handleMouseDown(e) {
        if (e.button === 2) return; // skip right-click
        let isDragging = false;
        let lastX = e.clientX, lastY = e.clientY;
        const onMove = (e) => {
            if (!isDragging) return;
            const scale = 4.0 / this.zoom;
            this.centerX -= (e.clientX - lastX) * scale / this.width;
            this.centerY -= (e.clientY - lastY) * scale / this.height;
            lastX = e.clientX;
            lastY = e.clientY;
        };
        const onUp = () => {
            if (isDragging) {
                isDragging = false;
                this.render();
            }
            document.removeEventListener('mousemove', onMove);
            document.removeEventListener('mouseup', onUp);
        };
        isDragging = true;
        document.addEventListener('mousemove', onMove);
        document.addEventListener('mouseup', onUp);
    }

    resetView() {
        this.centerX = -0.5;
        this.centerY = 0.0;
        this.zoom = 1.0;
        this.maxIterations = 100;
        document.getElementById('zoom').value = 1;
        document.getElementById('zoom-value').textContent = '1x';
        document.getElementById('iterations').value = 100;
        document.getElementById('iterations-value').textContent = '100';
        this.render();
    }

    saveImage() {
        const link = document.createElement('a');
        link.download = `${this.fractalType}_${Date.now()}.png`;
        link.href = this.canvas.toDataURL();
        link.click();
    }

    shareView() {
        const params = new URLSearchParams();
        params.set('fractal', this.fractalType);
        params.set('cx', this.centerX.toFixed(10));
        params.set('cy', this.centerY.toFixed(10));
        params.set('zoom', this.zoom.toString());
        params.set('iter', this.maxIterations.toString());
        if (this.fractalType === 'julia') {
            params.set('jx', this.juliaC.x.toFixed(10));
            params.set('jy', this.juliaC.y.toFixed(10));
        }
        if (this.fractalType === 'phoenix') {
            params.set('px', this.phoenixP.x.toFixed(10));
            params.set('py', this.phoenixP.y.toFixed(10));
        }
        const shareURL = window.location.origin + window.location.pathname + '?' + params.toString();
        if (navigator.clipboard && navigator.clipboard.writeText) {
            navigator.clipboard.writeText(shareURL).then(() => {
                alert('Share link copied to clipboard!');
            }).catch(() => alert('Share this link:\n\n' + shareURL));
        } else {
            alert('Share this link:\n\n' + shareURL);
        }
        window.history.replaceState({}, '', shareURL);
    }

    loadStateFromURL() {
        const params = new URLSearchParams(window.location.search);
        if (params.has('fractal')) {
            this.fractalType = params.get('fractal');
            document.getElementById('fractal-type').value = this.fractalType;
            this.updateFractalControls();
        }
        if (params.has('cx')) this.centerX = parseFloat(params.get('cx'));
        if (params.has('cy')) this.centerY = parseFloat(params.get('cy'));
        if (params.has('zoom')) {
            this.zoom = parseFloat(params.get('zoom'));
            document.getElementById('zoom').value = this.zoom;
            document.getElementById('zoom-value').textContent = this.zoom.toFixed(1) + 'x';
        }
        if (params.has('iter')) {
            this.maxIterations = parseInt(params.get('iter'));
            document.getElementById('iterations').value = this.maxIterations;
            document.getElementById('iterations-value').textContent = this.maxIterations.toString();
        }
        if (this.fractalType === 'julia') {
            if (params.has('jx')) {
                this.juliaC.x = parseFloat(params.get('jx'));
                document.getElementById('julia-cx').value = this.juliaC.x;
                document.getElementById('julia-cx-value').textContent = this.juliaC.x.toFixed(4);
            }
            if (params.has('jy')) {
                this.juliaC.y = parseFloat(params.get('jy'));
                document.getElementById('julia-cy').value = this.juliaC.y;
                document.getElementById('julia-cy-value').textContent = this.juliaC.y.toFixed(4);
            }
        }
        if (this.fractalType === 'phoenix') {
            if (params.has('px')) this.phoenixP.x = parseFloat(params.get('px'));
            if (params.has('py')) this.phoenixP.y = parseFloat(params.get('py'));
        }
    }

    updateFractalControls() {
        const juliaControls = document.getElementById('julia-controls');
        const phoenixControls = document.getElementById('phoenix-controls');
        juliaControls.style.display = this.fractalType === 'julia' ? 'block' : 'none';
        if (phoenixControls) {
            phoenixControls.style.display = this.fractalType === 'phoenix' ? 'block' : 'none';
        }

        switch (this.fractalType) {
            case 'julia':
                this.centerX = 0.0; this.centerY = 0.0; this.zoom = 1.0; break;
            case 'mandelbrot':
                this.centerX = -0.5; this.centerY = 0.0; this.zoom = 1.0; break;
            case 'burning_ship':
                this.centerX = -0.5; this.centerY = -0.5; this.zoom = 1.0; break;
            case 'tricorn':
                this.centerX = -0.3; this.centerY = 0.0; this.zoom = 1.0; break;
            case 'phoenix':
                this.centerX = 0.0; this.centerY = 0.0; this.zoom = 1.0; break;
            default:
                this.centerX = 0.0; this.centerY = 0.0; this.zoom = 1.0;
        }
        document.getElementById('zoom').value = this.zoom;
        document.getElementById('zoom-value').textContent = this.zoom.toFixed(1) + 'x';
    }

    loadPreset(preset) {
        const presets = {
            mandelbrot: {
                classic:   { cx: -0.5, cy: 0, z: 1, iter: 100 },
                spiral:    { cx: -0.7269, cy: 0.1889, z: 100, iter: 256 },
                seahorse:  { cx: -0.7453, cy: 0.1127, z: 200, iter: 256 },
                elephant:  { cx: 0.2869, cy: 0.0143, z: 300, iter: 512 },
                galaxy:    { cx: -0.7, cy: 0.0, z: 50, iter: 256 },
                lightning: { cx: -0.16, cy: 1.0405, z: 150, iter: 256 }
            },
            julia: {
                classic:   { jx: -0.7269, jy: 0.1889, cx: 0, cy: 0, z: 1 },
                spiral:    { jx: -0.4, jy: 0.6, cx: 0, cy: 0, z: 1 },
                seahorse:  { jx: 0.285, jy: 0.01, cx: 0, cy: 0, z: 1 },
                elephant:  { jx: 0.3, jy: 0.5, cx: 0, cy: 0, z: 1 },
                galaxy:    { jx: -0.8, jy: 0.156, cx: 0, cy: 0, z: 1 },
                lightning: { jx: -0.70176, jy: -0.3842, cx: 0, cy: 0, z: 1 }
            },
            burning_ship: {
                classic:   { cx: -0.5, cy: -0.5, z: 1 },
                spiral:    { cx: -1.7269, cy: -0.0311, z: 100 },
                seahorse:  { cx: -1.775, cy: -0.01, z: 500 },
                elephant:  { cx: -1.74, cy: -0.028, z: 200, iter: 256 },
                galaxy:    { cx: -1.65, cy: -0.035, z: 80, iter: 256 },
                lightning: { cx: -1.72, cy: -0.025, z: 150, iter: 256 }
            },
            newton: {
                classic:   { cx: 0, cy: 0, z: 1 },
                spiral:    { cx: 0, cy: 0, z: 3 },
                seahorse:  { cx: 0.5, cy: 0.866, z: 20 },
                elephant:  { cx: 1, cy: 0, z: 15, iter: 100 },
                galaxy:    { cx: 0, cy: 1, z: 8, iter: 100 },
                lightning: { cx: 0.866, cy: 0.5, z: 25, iter: 150 }
            },
            tricorn: {
                classic:   { cx: -0.3, cy: 0, z: 1 },
                spiral:    { cx: -0.11, cy: 0.93, z: 30, iter: 256 },
                seahorse:  { cx: -1.108, cy: 0.229, z: 80, iter: 256 },
                elephant:  { cx: 0.3, cy: 0.52, z: 50, iter: 256 },
                galaxy:    { cx: -0.4, cy: 0.6, z: 15, iter: 200 },
                lightning: { cx: -1.0, cy: 0.3, z: 100, iter: 300 }
            },
            phoenix: {
                classic:   { cx: 0, cy: 0, z: 1, px: 0.5667, py: 0 },
                spiral:    { cx: 0, cy: 0, z: 3, px: 0.2, py: 0.01 },
                seahorse:  { cx: 0, cy: 0, z: 1, px: -0.5, py: 0.0 },
                elephant:  { cx: 0, cy: 0, z: 1, px: 0.56667, py: -0.5 },
                galaxy:    { cx: 0, cy: 0, z: 2, px: 0.4, py: 0.1 },
                lightning: { cx: 0, cy: 0, z: 1, px: 0.28, py: 0.008 }
            }
        };

        const p = (presets[this.fractalType] || presets.mandelbrot)[preset];
        if (!p) return;

        this.centerX = p.cx ?? this.centerX;
        this.centerY = p.cy ?? this.centerY;
        this.zoom = p.z ?? this.zoom;
        if (p.iter) this.maxIterations = p.iter;
        if (p.jx !== undefined) { this.juliaC.x = p.jx; this.juliaC.y = p.jy; }
        if (p.px !== undefined) { this.phoenixP.x = p.px; this.phoenixP.y = p.py; }

        // Sync UI
        document.getElementById('zoom').value = this.zoom;
        document.getElementById('zoom-value').textContent = this.zoom.toFixed(1) + 'x';
        document.getElementById('iterations').value = this.maxIterations;
        document.getElementById('iterations-value').textContent = this.maxIterations.toString();
        if (this.fractalType === 'julia') {
            document.getElementById('julia-cx').value = this.juliaC.x;
            document.getElementById('julia-cx-value').textContent = this.juliaC.x.toFixed(4);
            document.getElementById('julia-cy').value = this.juliaC.y;
            document.getElementById('julia-cy-value').textContent = this.juliaC.y.toFixed(4);
        }
        this.render();
    }

    saveFavorite() {
        const name = prompt('Enter a name for this favorite view:');
        if (!name) return;
        const fav = {
            name, fractalType: this.fractalType,
            centerX: this.centerX, centerY: this.centerY,
            zoom: this.zoom, maxIterations: this.maxIterations,
            juliaC: { ...this.juliaC }, phoenixP: { ...this.phoenixP },
            timestamp: Date.now()
        };
        let favs = JSON.parse(localStorage.getItem('fractal-favorites') || '[]');
        favs.push(fav);
        if (favs.length > 20) favs = favs.slice(-20);
        localStorage.setItem('fractal-favorites', JSON.stringify(favs));
        alert('Favorite saved: ' + name);
    }

    manageFavorites() {
        const favs = JSON.parse(localStorage.getItem('fractal-favorites') || '[]');
        if (favs.length === 0) { alert('No favorites saved yet.'); return; }
        let msg = 'Your Saved Favorites:\n\n';
        favs.forEach((f, i) => {
            msg += `${i + 1}. ${f.name} (${f.fractalType}) - ${new Date(f.timestamp).toLocaleDateString()}\n`;
        });
        msg += '\nEnter number to load, "delete N" to remove, or "clear" to remove all:';
        const input = prompt(msg);
        if (!input) return;
        if (input.toLowerCase() === 'clear') {
            if (confirm('Delete ALL favorites?')) {
                localStorage.removeItem('fractal-favorites');
                alert('All favorites cleared.');
            }
            return;
        }
        if (input.toLowerCase().startsWith('delete ')) {
            const idx = parseInt(input.substring(7)) - 1;
            if (idx >= 0 && idx < favs.length) {
                const del = favs.splice(idx, 1)[0];
                localStorage.setItem('fractal-favorites', JSON.stringify(favs));
                alert('Deleted: ' + del.name);
                this.manageFavorites();
            }
            return;
        }
        const idx = parseInt(input) - 1;
        if (idx >= 0 && idx < favs.length) this.loadFavorite(favs[idx]);
    }

    loadFavorite(fav) {
        this.fractalType = fav.fractalType;
        this.centerX = fav.centerX;
        this.centerY = fav.centerY;
        this.zoom = fav.zoom;
        this.maxIterations = fav.maxIterations;
        this.juliaC = { ...fav.juliaC };
        if (fav.phoenixP) this.phoenixP = { ...fav.phoenixP };
        document.getElementById('fractal-type').value = this.fractalType;
        this.updateFractalControls();
        document.getElementById('zoom').value = this.zoom;
        document.getElementById('zoom-value').textContent = this.zoom.toFixed(1) + 'x';
        document.getElementById('iterations').value = this.maxIterations;
        document.getElementById('iterations-value').textContent = this.maxIterations.toString();
        this.render();
    }

    // ========================================================================
    // Fractal computation — optimized hot loops
    // ========================================================================

    computeIterations(cx, cy) {
        switch (this.fractalType) {
            case 'mandelbrot': return this.mandelbrotIterations(cx, cy);
            case 'julia':      return this.juliaIterations(cx, cy);
            case 'burning_ship': return this.burningShipIterations(cx, cy);
            case 'newton':     return this.newtonIterations(cx, cy);
            case 'tricorn':    return this.tricornIterations(cx, cy);
            case 'phoenix':    return this.phoenixIterations(cx, cy);
            default: return 0;
        }
    }

    // Mandelbrot with cardioid/bulb skip + smooth coloring
    mandelbrotIterations(cx, cy) {
        // Cardioid check: skip points inside the main cardioid
        const cy2 = cy * cy;
        const q = (cx - 0.25) * (cx - 0.25) + cy2;
        if (q * (q + (cx - 0.25)) <= 0.25 * cy2) return this.maxIterations;
        // Period-2 bulb check
        if ((cx + 1) * (cx + 1) + cy2 <= 0.0625) return this.maxIterations;

        let zx = 0.0, zy = 0.0;
        let zx2 = 0.0, zy2 = 0.0;
        let i = 0;
        const maxIter = this.maxIterations;

        while (i < maxIter && zx2 + zy2 <= 4.0) {
            zy = 2.0 * zx * zy + cy;
            zx = zx2 - zy2 + cx;
            zx2 = zx * zx;
            zy2 = zy * zy;
            i++;
        }

        if (i === maxIter) return maxIter;
        // Smooth coloring: fractional escape count
        return i + 1 - Math.log2(Math.log(zx2 + zy2) * 0.5);
    }

    juliaIterations(cx, cy) {
        let zx = cx, zy = cy;
        let zx2 = zx * zx, zy2 = zy * zy;
        let i = 0;
        const maxIter = this.maxIterations;
        const jcx = this.juliaC.x, jcy = this.juliaC.y;

        while (i < maxIter && zx2 + zy2 <= 4.0) {
            zy = 2.0 * zx * zy + jcy;
            zx = zx2 - zy2 + jcx;
            zx2 = zx * zx;
            zy2 = zy * zy;
            i++;
        }

        if (i === maxIter) return maxIter;
        return i + 1 - Math.log2(Math.log(zx2 + zy2) * 0.5);
    }

    burningShipIterations(cx, cy) {
        let zx = 0.0, zy = 0.0;
        let zx2 = 0.0, zy2 = 0.0;
        let i = 0;
        const maxIter = this.maxIterations;

        while (i < maxIter && zx2 + zy2 <= 4.0) {
            const ax = Math.abs(zx), ay = Math.abs(zy);
            zy = 2.0 * ax * ay + cy;
            zx = ax * ax - ay * ay + cx; // note: ax*ax = zx2, ay*ay = zy2
            zx2 = zx * zx;
            zy2 = zy * zy;
            i++;
        }

        if (i === maxIter) return maxIter;
        return i + 1 - Math.log2(Math.log(zx2 + zy2) * 0.5);
    }

    // Tricorn (Mandelbar): z_{n+1} = conj(z)^2 + c
    tricornIterations(cx, cy) {
        let zx = 0.0, zy = 0.0;
        let zx2 = 0.0, zy2 = 0.0;
        let i = 0;
        const maxIter = this.maxIterations;

        while (i < maxIter && zx2 + zy2 <= 4.0) {
            // conj(z) = (zx, -zy), conj(z)^2 = (zx^2 - zy^2, -2*zx*zy)
            zy = -2.0 * zx * zy + cy;
            zx = zx2 - zy2 + cx;
            zx2 = zx * zx;
            zy2 = zy * zy;
            i++;
        }

        if (i === maxIter) return maxIter;
        return i + 1 - Math.log2(Math.log(zx2 + zy2) * 0.5);
    }

    // Phoenix: z_{n+1} = z_n^2 + Re(p) + Im(p) * z_{n-1}
    phoenixIterations(cx, cy) {
        let zx = cx, zy = cy;
        let prevX = 0.0, prevY = 0.0;
        let zx2 = zx * zx, zy2 = zy * zy;
        let i = 0;
        const maxIter = this.maxIterations;
        const px = this.phoenixP.x, py = this.phoenixP.y;

        while (i < maxIter && zx2 + zy2 <= 4.0) {
            const newX = zx2 - zy2 + px + py * prevX;
            const newY = 2.0 * zx * zy + py * prevY;
            prevX = zx;
            prevY = zy;
            zx = newX;
            zy = newY;
            zx2 = zx * zx;
            zy2 = zy * zy;
            i++;
        }

        if (i === maxIter) return maxIter;
        return i + 1 - Math.log2(Math.log(zx2 + zy2) * 0.5);
    }

    newtonIterations(cx, cy) {
        let x = cx, y = cy;
        const maxIter = this.maxIterations;
        const tol = 1e-6;

        for (let i = 0; i < maxIter; i++) {
            const x2 = x * x - y * y;
            const y2 = 2 * x * y;
            const x3 = x2 * x - y2 * y;
            const y3 = x2 * y + y2 * x;
            const denom = 3 * (x2 * x2 + y2 * y2);
            if (denom < 1e-10) break;

            const newX = (2 * x3 + 1) / denom * x2 + (2 * y3) / denom * y2;
            const newY = (2 * y3) / denom * x2 - (2 * x3 + 1) / denom * y2;
            const dx = newX - x, dy = newY - y;

            x = newX;
            y = newY;

            if (dx * dx + dy * dy < tol) {
                if (Math.abs(x - 1) < 0.1 && Math.abs(y) < 0.1) return maxIter + 1;
                if (Math.abs(x + 0.5) < 0.1 && Math.abs(y - 0.866) < 0.1) return maxIter + 2;
                if (Math.abs(x + 0.5) < 0.1 && Math.abs(y + 0.866) < 0.1) return maxIter + 3;
                break;
            }
        }
        return 0;
    }

    // ========================================================================
    // Color mapping — smooth coloring with better palettes
    // ========================================================================

    iterationsToColor(iterations) {
        if (this.fractalType === 'newton') {
            if (iterations > this.maxIterations) {
                const root = iterations - this.maxIterations;
                switch (root) {
                    case 1: return [220, 60, 60];
                    case 2: return [60, 220, 60];
                    case 3: return [60, 60, 220];
                }
            }
            return [30, 30, 30];
        }

        const maxIter = this.maxIterations;
        if (iterations >= maxIter) return [0, 0, 0];

        // Smooth t from fractional iteration count
        const t = iterations / maxIter;

        // Ultra Fractal-style smooth palette using sine waves
        const r = Math.floor(127.5 * (1 + Math.cos(2 * Math.PI * (t * 5 + 0.0))));
        const g = Math.floor(127.5 * (1 + Math.cos(2 * Math.PI * (t * 5 + 0.33))));
        const b = Math.floor(127.5 * (1 + Math.cos(2 * Math.PI * (t * 5 + 0.67))));
        return [r, g, b];
    }

    // ========================================================================
    // Rendering — row-based progressive rendering (much faster than 1000-pixel chunks)
    // ========================================================================

    async render() {
        if (this.isRendering) {
            this.renderCancelled = true;
            return;
        }

        this.isRendering = true;
        this.renderCancelled = false;
        this.renderStartTime = performance.now();
        this.pixelsRendered = 0;

        document.getElementById('render-btn').textContent = 'Rendering...';
        document.getElementById('render-btn').disabled = true;

        const scale = 4.0 / this.zoom;
        const minX = this.centerX - scale / 2;
        const maxX = this.centerX + scale / 2;
        const minY = this.centerY - scale / 2;
        const maxY = this.centerY + scale / 2;

        const w = this.width, h = this.height;
        const data = this.imageData.data;
        const rowsPerChunk = 8; // Render 8 rows at a time — good balance

        let y = 0;
        const renderRows = () => {
            if (this.renderCancelled) {
                this.finishRendering(performance.now() - this.renderStartTime);
                return;
            }

            const endY = Math.min(y + rowsPerChunk, h);
            for (; y < endY; y++) {
                const cy = minY + (maxY - minY) * y / (h - 1);
                for (let x = 0; x < w; x++) {
                    const cx = minX + (maxX - minX) * x / (w - 1);
                    const iter = this.computeIterations(cx, cy);
                    const [r, g, b] = this.iterationsToColor(iter);
                    const idx = (y * w + x) * 4;
                    data[idx] = r;
                    data[idx + 1] = g;
                    data[idx + 2] = b;
                    data[idx + 3] = 255;
                }
            }
            this.pixelsRendered = y * w;

            this.ctx.putImageData(this.imageData, 0, 0);

            const progress = (y / h) * 100;
            document.getElementById('progress').style.width = progress + '%';
            const elapsed = performance.now() - this.renderStartTime;
            const pps = Math.floor(this.pixelsRendered / (elapsed / 1000));
            document.getElementById('web-perf').textContent = '~' + (pps / 1000).toFixed(0) + 'K';
            document.getElementById('stats').textContent =
                `Rendered: ${this.pixelsRendered.toLocaleString()} pixels | ` +
                `Speed: ${pps.toLocaleString()} pixels/sec | Progress: ${progress.toFixed(1)}%`;

            if (y < h) {
                requestAnimationFrame(renderRows);
            } else {
                this.finishRendering(elapsed);
            }
        };

        requestAnimationFrame(renderRows);
    }

    finishRendering(elapsed) {
        this.isRendering = false;
        const pps = Math.floor(this.pixelsRendered / (elapsed / 1000));
        document.getElementById('stats').textContent =
            `Complete! ${this.pixelsRendered.toLocaleString()} pixels in ${elapsed.toFixed(0)}ms ` +
            `(${pps.toLocaleString()} pixels/sec)`;
        document.getElementById('render-btn').textContent = 'Render';
        document.getElementById('render-btn').disabled = false;
        document.getElementById('progress').style.width = '100%';
        setTimeout(() => { document.getElementById('progress').style.width = '0%'; }, 2000);
    }
}

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    new FractalRenderer();
});

// Keyboard shortcuts
document.addEventListener('keydown', (e) => {
    if (e.code === 'Space') { e.preventDefault(); document.getElementById('render-btn').click(); }
    else if (e.code === 'KeyR') document.getElementById('reset-btn').click();
    else if (e.code === 'KeyS') document.getElementById('save-btn').click();
});
