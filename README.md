# Fractal Renderer

A high-performance fractal rendering engine featuring 6 fractal types, multiple compute backends (CPU, OpenMP, CUDA, WebAssembly), and a server-side rendering API deployable on EC2.

![C++](https://img.shields.io/badge/C++-17-blue)
![CUDA](https://img.shields.io/badge/CUDA-12.6-green)
![Docker](https://img.shields.io/badge/Docker-ready-blue)
![License](https://img.shields.io/badge/license-MIT-blue)

**[Live Demo](https://geoffreywang1117.github.io/Mandelbrot-Renderer/)** | **[WebAssembly Version](https://geoffreywang1117.github.io/Mandelbrot-Renderer/wasm-fractal.html)**

## Supported Fractals

| Fractal | Formula | Description |
|---------|---------|-------------|
| Mandelbrot | z = z² + c | Classic escape-time fractal with cardioid optimization |
| Julia | z = z² + c (fixed c) | Parameter-space exploration, interactive c control |
| Burning Ship | z = (\|Re(z)\| + i\|Im(z)\|)² + c | Absolute-value variant with flame-like patterns |
| Newton | z = z - f(z)/f'(z), f = z³ - 1 | Root-finding convergence basins (RGB) |
| Tricorn | z = conj(z)² + c | Conjugate variant, tri-symmetric patterns |
| Phoenix | z = z² + p_re + p_im * z_{n-1} | Uses iteration history, feathered patterns |

## Performance

| Backend | 800x600 | Pixels/sec | Speedup |
|---------|---------|------------|---------|
| Web JavaScript | ~3,000 ms | ~160K | 1x |
| WebAssembly | ~600 ms | ~800K | 5x |
| CPU (single-thread) | 292 ms | 1.6M | 10x |
| OpenMP (32 cores) | 4 ms | 120M | 750x |
| CUDA GPU | 0.024 ms | 20B | 125,000x |

Optimizations: Mandelbrot cardioid/period-2 bulb skip (~2.4x), cached z² values, smooth coloring via `i + 1 - log2(log|z|)`, sine-wave color palette.

## Quick Start

### Web Demo (no install)

Visit the **[Live Demo](https://geoffreywang1117.github.io/Mandelbrot-Renderer/)** — scroll to zoom, click to re-center, drag to pan.

### Local Development

```bash
git clone https://github.com/GeoffreyWang1117/Mandelbrot-Renderer.git
cd Mandelbrot-Renderer

# Static file server (JS version only)
make dev

# Full stack: C++ API server + static files
make server
# Then open http://localhost:3000 for API, http://localhost:8080 for UI
```

### EC2 Deployment (t3a.micro safe)

**Pre-built images (recommended, peak 290 MB RAM):**

```bash
# One-line deploy
curl -fsSL https://raw.githubusercontent.com/GeoffreyWang1117/Mandelbrot-Renderer/main/deploy.sh | bash

# Or manually
docker compose -f docker-compose.prod.yml pull
docker compose -f docker-compose.prod.yml up -d
```

**Build on server (needs 2+ GB RAM):**

```bash
docker compose up -d --build
```

### Native C++ Build

```bash
make build          # CPU + API binary
make api            # API binary only (no cmake needed)

# With cmake (all targets)
mkdir build && cd build
cmake .. -DENABLE_ALL=ON && make -j$(nproc)

# CUDA standalone
nvcc -O3 -o build/mandelbrot_cuda src/mandelbrot_cuda_standalone.cu
```

## Server API

When deployed with Docker or `make server`, a REST API is available:

```
GET /api/render?fractal=mandelbrot&width=1920&height=1080&zoom=1&iter=1000&format=png
GET /api/wallpaper/mandelbrot-spiral?resolution=3840x2160
GET /api/health
```

Parameters: `fractal`, `width`, `height`, `cx`, `cy`, `zoom`, `iter`, `format` (png/webp/jpeg), `juliaReal`, `juliaImag`, `phoenixPx`, `phoenixPy`.

Max resolution: 3840x2160. Concurrent render limit: 2 (configurable via `MAX_RENDERS` env var). Returns 503 when busy.

## Project Structure

```
.
├── docs/                   # Web frontend (deployed to GitHub Pages)
│   ├── index.html          #   JS fractal explorer (6 fractal types)
│   ├── mandelbrot.js       #   Rendering engine + UI logic
│   ├── wasm-fractal.html   #   WebAssembly version
│   ├── fractals.wasm       #   Compiled WASM binary
│   └── fractals.js         #   Emscripten glue code
├── src/                    # C++ source
│   ├── render_api.cpp      #   Server-side render binary (all 6 fractals)
│   ├── render.cpp          #   CPU single-thread renderer
│   ├── render_omp.cpp      #   OpenMP parallel renderer
│   ├── render_cuda.cu      #   CUDA GPU renderer
│   ├── main.cpp            #   CLI entry point
│   └── mandelbrot_cuda_standalone.cu
├── include/                # C++ headers
├── wasm/                   # WASM build config (Emscripten)
│   ├── src/fractals_wasm.cpp
│   ├── CMakeLists.txt
│   └── build.sh
├── server/                 # Node.js API server
│   ├── index.js            #   Express + sharp (PPM→PNG conversion)
│   └── package.json
├── nginx/                  # Nginx reverse proxy config
│   └── nginx.conf
├── Dockerfile              # Single-container build
├── Dockerfile.api          # API-only container (for docker-compose)
├── docker-compose.yml      # Local build deployment
├── docker-compose.prod.yml # Pre-built image deployment
├── deploy.sh               # One-line EC2 deploy script
├── Makefile                # Build & deploy commands
├── CMakeLists.txt          # C++ build config
└── .github/workflows/
    ├── deploy.yml          # GitHub Pages auto-deploy
    └── docker-publish.yml  # Docker image CI/CD → GHCR
```

## Docker

Two deployment modes:

| Mode | Command | RAM needed | Best for |
|------|---------|-----------|----------|
| Pre-built pull | `make docker-prod` | 290 MB peak | t3a.micro ($6.80/mo) |
| Build on server | `make docker-up` | 570 MB peak | t3a.small+ |

Resource limits (docker-compose): Nginx 128 MB, API 512 MB. Concurrency limiter prevents OOM.

## CLI Usage

```bash
# CPU single-thread
./build/mandelbrot_cpu --width 1920 --height 1080 --iter 2000 --output output/hd.ppm

# Server-side API binary (outputs PPM to stdout)
./build/fractal_api --fractal tricorn --width 3840 --height 2160 --iter 1000 > out.ppm

# All fractal_api options:
#   --fractal    mandelbrot|julia|burning_ship|newton|tricorn|phoenix
#   --width/height/iter/cx/cy/zoom
#   --julia-real/--julia-imag    (Julia c parameter)
#   --phoenix-px/--phoenix-py    (Phoenix p parameter)
```

## License

MIT

## Author

Geoffrey Wang (with Claude AI assistance)
