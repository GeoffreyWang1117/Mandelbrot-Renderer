# GitHub Pages Deployment Guide

This guide explains how to deploy the Mandelbrot web demo to GitHub Pages.

## Quick Setup

1. **Push to GitHub**
   ```bash
   git add .
   git commit -m "Add interactive web demo"
   git push origin main
   ```

2. **Enable GitHub Pages**
   - Go to your repository on GitHub
   - Click **Settings** tab
   - Scroll to **Pages** section
   - Under **Source**, select "Deploy from a branch"
   - Choose **main** branch and **/ (root)** folder
   - Click **Save**

3. **Update Workflow (Optional)**
   - The `.github/workflows/deploy.yml` file is already configured
   - It will automatically deploy the `/web` folder to GitHub Pages
   - Your site will be available at: `https://geoffreywang1117.github.io/Mandelbrot-Renderer/web/`

## Manual GitHub Pages Setup

If you prefer manual setup without GitHub Actions:

1. **Repository Settings**
   - Go to Settings > Pages
   - Source: Deploy from a branch
   - Branch: main
   - Folder: `/web`

2. **Custom Domain (Optional)**
   - Add a `CNAME` file in `/web` directory with your domain
   - Configure DNS to point to GitHub Pages

## Local Testing

Test the web demo locally before deployment:

```bash
# Method 1: Python HTTP server
cd web
python -m http.server 8000
# Visit: http://localhost:8000

# Method 2: Node.js serve
npx serve web
# Visit: http://localhost:3000

# Method 3: PHP built-in server
cd web
php -S localhost:8000
```

## Customization

### Update GitHub Repository URL
Replace placeholder URLs in:
- `web/index.html` (GitHub link)
- `README_EN.md` (demo link)
- `web/README.md` (demo link)

### Branding
- Update title in `web/index.html`
- Modify color scheme in CSS
- Add your logo or branding

### Performance Metrics
- Update performance comparison with your actual benchmark results
- Modify system specifications in the info panel

## Features of the Web Demo

### Interactive Controls
- **Click to zoom**: 2x zoom at click point
- **Drag to pan**: Mouse drag to move around
- **Parameter adjustment**: Sliders for iterations and zoom
- **Preset locations**: Quick access to interesting regions

### Progressive Rendering
- Renders in chunks for UI responsiveness
- Real-time progress indicator
- Performance metrics display

### Mobile Support
- Responsive design
- Touch-friendly controls
- Optimized for mobile screens

## Browser Compatibility

| Browser | Support | Notes |
|---------|---------|-------|
| Chrome | ✅ Full | Recommended |
| Firefox | ✅ Full | Excellent performance |
| Safari | ✅ Full | Works well |
| Edge | ✅ Full | Good performance |
| Mobile | ✅ Responsive | Touch controls |

## Performance Expectations

The web demo is designed for demonstration purposes:

- **Performance**: ~50,000 pixels/second
- **Responsiveness**: Progressive rendering maintains UI
- **Memory Usage**: Efficient canvas-based rendering
- **Compatibility**: Works on all modern browsers

For production-quality high-performance rendering, users should download and compile the native C++/CUDA implementations which achieve 1,000-40,000x better performance.

## Troubleshooting

### Deployment Issues
```bash
# Check GitHub Pages status
# Go to repository > Settings > Pages > View deployment

# Force rebuild
git commit --allow-empty -m "Trigger rebuild"
git push
```

### Local Testing Issues
```bash
# CORS issues with file:// protocol
# Use HTTP server instead of opening HTML directly
python -m http.server 8000
```

### Performance Issues
- Reduce max iterations for faster rendering
- Lower resolution for older devices
- Progressive rendering ensures UI stays responsive

## Success Metrics

A successful deployment should have:
- ✅ Live demo accessible at GitHub Pages URL
- ✅ Interactive zoom and pan functionality
- ✅ Progressive rendering with performance display
- ✅ Mobile responsive design
- ✅ Links to main project repository
- ✅ Performance comparison showing native advantages

---

The web demo serves as an perfect introduction to your high-performance Mandelbrot project, allowing users to immediately experience the fractal while highlighting the massive performance benefits of the native implementations!
