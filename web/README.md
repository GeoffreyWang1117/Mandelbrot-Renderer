# Mandelbrot Web Demo

This directory contains a lightweight web implementation of the Mandelbrot renderer, designed for GitHub Pages deployment and interactive demonstration.

## Features

- **Interactive Web Interface**: Click to zoom, drag to pan
- **Progressive Rendering**: Real-time updates during computation
- **Responsive Design**: Works on desktop and mobile
- **Performance Comparison**: Shows speed differences between implementations
- **Preset Locations**: Quick access to interesting fractal regions

## Live Demo

🌐 **[View Live Demo](https://geoffreywang1117.github.io/Mandelbrot-Renderer/web/)**

## Files

- `index.html`: Main web interface with modern responsive design
- `mandelbrot.js`: JavaScript implementation with progressive rendering
- `README.md`: This documentation

## Deployment to GitHub Pages

1. Push the project to GitHub
2. Go to repository Settings > Pages
3. Select "Deploy from a branch"
4. Choose `main` branch and `/web` folder
5. Your demo will be live at: `https://geoffreywang1117.github.io/Mandelbrot-Renderer/web/`

## Performance Comparison

| Implementation | Performance | Use Case |
|---|---|---|
| Web JavaScript | ~50K pixels/sec | Interactive demo |
| C++ CPU | 1.6M pixels/sec | High-quality rendering |
| OpenMP | 120M pixels/sec | Multi-core systems |
| CUDA GPU | 1,700M pixels/sec | Ultimate performance |

## Technical Implementation

### Progressive Rendering
The web version uses chunked rendering to maintain UI responsiveness:
- Renders in batches of 1000 pixels
- Updates canvas progressively
- Shows real-time performance metrics

### Interactive Controls
- **Zoom**: Click anywhere to zoom 2x at that point
- **Pan**: Drag to move around the fractal
- **Parameters**: Adjust iterations and zoom level
- **Presets**: Jump to interesting locations

### Color Algorithm
Uses HSV color space for smooth gradients:
- Hue varies with iteration count
- Saturation and value create depth
- Black for points in the Mandelbrot set

## Browser Compatibility

- **Chrome/Edge**: Full support
- **Firefox**: Full support  
- **Safari**: Full support
- **Mobile**: Responsive design works on touch devices

## Local Development

```bash
# Serve locally (any HTTP server)
cd web
python -m http.server 8000
# or
npx serve .
```

Then visit: http://localhost:8000

## Integration with Main Project

This web demo serves as an interactive introduction to the full high-performance implementation. Users can:

1. **Explore**: Interact with the fractal in the browser
2. **Learn**: See performance comparisons and technical details
3. **Download**: Access the full native implementations for maximum performance

The web version achieves ~50,000 pixels/second, while the CUDA GPU version reaches 1.7 billion pixels/second - a 34,000x performance difference that demonstrates the power of parallel computing!
