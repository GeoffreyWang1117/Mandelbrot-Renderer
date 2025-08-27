// Multi-Fractal Web Renderer - Interactive JavaScript Implementation
// High-performance web demo with progressive rendering
// Supports: Mandelbrot Set, Julia Set, and more
//
// Author: Geoffrey Wang (with Claude AI assistance)
// Date: August 13, 2025
// Repository: https://github.com/GeoffreyWang1117/Mandelbrot-Renderer

class FractalRenderer {
    constructor() {
        this.canvas = document.getElementById('mandelbrot-canvas');
        if (!this.canvas) {
            console.error('❌ Canvas element not found!');
            throw new Error('Canvas element not found');
        }
        
        this.ctx = this.canvas.getContext('2d');
        if (!this.ctx) {
            console.error('❌ 2D context not available!');
            throw new Error('2D context not available');
        }
        
        this.width = 800;
        this.height = 600;
        
        // Fractal parameters
        this.fractalType = 'mandelbrot'; // 'mandelbrot' or 'julia'
        this.centerX = -0.5;
        this.centerY = 0.0;
        this.zoom = 1.0;
        this.maxIterations = 100;
        
        // Julia set parameters
        this.juliaC = { x: -0.7269, y: 0.1889 };
        
        // Rendering state
        this.isRendering = false;
        this.renderWorker = null;
        this.imageData = null;
        
        // Performance tracking
        this.renderStartTime = 0;
        this.pixelsRendered = 0;
        
        // Initialize UI
        this.initializeUI();
        this.setupEventListeners();
        
        // Initial render
        this.render();
    }
    
    initializeUI() {
        this.canvas.width = this.width;
        this.canvas.height = this.height;
        this.imageData = this.ctx.createImageData(this.width, this.height);
        
        // Clear canvas
        this.ctx.fillStyle = '#000';
        this.ctx.fillRect(0, 0, this.width, this.height);
    }
    
    setupEventListeners() {
        // Canvas interactions
        this.canvas.addEventListener('click', (e) => this.handleCanvasClick(e));
        this.canvas.addEventListener('mousedown', (e) => this.handleMouseDown(e));
        
        // Fractal type selector
        document.getElementById('fractal-type').addEventListener('change', (e) => {
            this.fractalType = e.target.value;
            this.updateFractalControls();
            this.render();
        });
        
        // Julia set parameter controls
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
        
        // Control inputs
        document.getElementById('iterations').addEventListener('input', (e) => {
            this.maxIterations = parseInt(e.target.value);
            document.getElementById('iterations-value').textContent = e.target.value;
        });
        
        document.getElementById('zoom').addEventListener('input', (e) => {
            this.zoom = parseFloat(e.target.value);
            document.getElementById('zoom-value').textContent = e.target.value + 'x';
        });
        
        // Control buttons
        document.getElementById('render-btn').addEventListener('click', () => this.render());
        document.getElementById('reset-btn').addEventListener('click', () => this.resetView());
        document.getElementById('save-btn').addEventListener('click', () => this.saveImage());
        
        // Preset locations (update for current fractal)
        document.getElementById('classic-btn').addEventListener('click', () => this.loadPreset('classic'));
        document.getElementById('spiral-btn').addEventListener('click', () => this.loadPreset('spiral'));
        document.getElementById('seahorse-btn').addEventListener('click', () => this.loadPreset('seahorse'));
    }
    
    handleCanvasClick(e) {
        if (this.isRendering) return;
        
        const rect = this.canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;
        
        // Convert screen coordinates to complex plane
        const scale = 4.0 / this.zoom;
        const newCenterX = this.centerX + (x - this.width/2) * scale / this.width;
        const newCenterY = this.centerY + (y - this.height/2) * scale / this.height;
        
        // Zoom in
        this.centerX = newCenterX;
        this.centerY = newCenterY;
        this.zoom *= 2;
        
        // Update zoom slider
        if (this.zoom <= 1000) {
            document.getElementById('zoom').value = this.zoom;
            document.getElementById('zoom-value').textContent = this.zoom.toFixed(1) + 'x';
        }
        
        this.render();
    }
    
    handleMouseDown(e) {
        // Simple drag-to-pan implementation
        let isDragging = false;
        let lastX = e.clientX;
        let lastY = e.clientY;
        
        const handleMouseMove = (e) => {
            if (!isDragging) return;
            
            const deltaX = e.clientX - lastX;
            const deltaY = e.clientY - lastY;
            
            const scale = 4.0 / this.zoom;
            this.centerX -= deltaX * scale / this.width;
            this.centerY -= deltaY * scale / this.height;
            
            lastX = e.clientX;
            lastY = e.clientY;
        };
        
        const handleMouseUp = () => {
            if (isDragging) {
                isDragging = false;
                this.render();
            }
            document.removeEventListener('mousemove', handleMouseMove);
            document.removeEventListener('mouseup', handleMouseUp);
        };
        
        isDragging = true;
        document.addEventListener('mousemove', handleMouseMove);
        document.addEventListener('mouseup', handleMouseUp);
    }
    
    resetView() {
        this.centerX = -0.5;
        this.centerY = 0.0;
        this.zoom = 1.0;
        this.maxIterations = 100;
        
        // Reset UI
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
    
    // Update UI controls based on fractal type
    updateFractalControls() {
        const juliaControls = document.getElementById('julia-controls');
        if (this.fractalType === 'julia') {
            juliaControls.style.display = 'block';
            // Reset view for Julia set
            this.centerX = 0.0;
            this.centerY = 0.0;
            this.zoom = 1.0;
        } else {
            juliaControls.style.display = 'none';
            // Reset view for Mandelbrot
            this.centerX = -0.5;
            this.centerY = 0.0;
            this.zoom = 1.0;
        }
        
        // Update UI sliders
        document.getElementById('zoom').value = this.zoom;
        document.getElementById('zoom-value').textContent = this.zoom.toFixed(1) + 'x';
    }
    
    // Load fractal-specific presets
    loadPreset(preset) {
        if (this.fractalType === 'mandelbrot') {
            switch(preset) {
                case 'classic':
                    this.centerX = -0.5; this.centerY = 0.0; this.zoom = 1.0;
                    break;
                case 'spiral':
                    this.centerX = -0.7269; this.centerY = 0.1889; this.zoom = 100;
                    break;
                case 'seahorse':
                    this.centerX = -0.7453; this.centerY = 0.1127; this.zoom = 200;
                    break;
            }
        } else if (this.fractalType === 'julia') {
            switch(preset) {
                case 'classic':
                    this.juliaC.x = -0.7269; this.juliaC.y = 0.1889;
                    this.centerX = 0.0; this.centerY = 0.0; this.zoom = 1.0;
                    break;
                case 'spiral':
                    this.juliaC.x = -0.4; this.juliaC.y = 0.6;
                    this.centerX = 0.0; this.centerY = 0.0; this.zoom = 1.0;
                    break;
                case 'seahorse':
                    this.juliaC.x = 0.285; this.juliaC.y = 0.01;
                    this.centerX = 0.0; this.centerY = 0.0; this.zoom = 1.0;
                    break;
            }
            // Update Julia parameter sliders
            document.getElementById('julia-cx').value = this.juliaC.x;
            document.getElementById('julia-cx-value').textContent = this.juliaC.x.toFixed(4);
            document.getElementById('julia-cy').value = this.juliaC.y;
            document.getElementById('julia-cy-value').textContent = this.juliaC.y.toFixed(4);
        }
        
        // Update zoom slider
        document.getElementById('zoom').value = this.zoom;
        document.getElementById('zoom-value').textContent = this.zoom.toFixed(1) + 'x';
        
        this.render();
    }
    
    // Core fractal computation dispatcher
    computeIterations(cx, cy) {
        if (this.fractalType === 'mandelbrot') {
            return this.mandelbrotIterations(cx, cy);
        } else if (this.fractalType === 'julia') {
            return this.juliaIterations(cx, cy);
        }
        return 0;
    }
    
    // Core Mandelbrot computation
    mandelbrotIterations(cx, cy) {
        let x = 0.0, y = 0.0;
        let iterations = 0;
        
        while (x*x + y*y < 4.0 && iterations < this.maxIterations) {
            const xtemp = x*x - y*y + cx;
            y = 2*x*y + cy;
            x = xtemp;
            iterations++;
        }
        
        return iterations;
    }
    
    // Core Julia set computation
    juliaIterations(cx, cy) {
        let x = cx, y = cy;
        let iterations = 0;
        
        while (x*x + y*y < 4.0 && iterations < this.maxIterations) {
            const xtemp = x*x - y*y + this.juliaC.x;
            y = 2*x*y + this.juliaC.y;
            x = xtemp;
            iterations++;
        }
        
        return iterations;
    }
    
    // Convert iterations to color
    iterationsToColor(iterations) {
        if (iterations === this.maxIterations) {
            return [0, 0, 0]; // Black for points in the set
        }
        
        // Smooth HSV coloring
        const t = iterations / this.maxIterations;
        const hue = t * 360;
        const saturation = 1.0;
        const value = t < 1.0 ? 1.0 : 0.0;
        
        // Simple HSV to RGB conversion
        const c = value * saturation;
        const x = c * (1 - Math.abs(((hue / 60) % 2) - 1));
        const m = value - c;
        
        let r, g, b;
        if (hue >= 0 && hue < 60) { r = c; g = x; b = 0; }
        else if (hue >= 60 && hue < 120) { r = x; g = c; b = 0; }
        else if (hue >= 120 && hue < 180) { r = 0; g = c; b = x; }
        else if (hue >= 180 && hue < 240) { r = 0; g = x; b = c; }
        else if (hue >= 240 && hue < 300) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }
        
        return [
            Math.floor((r + m) * 255),
            Math.floor((g + m) * 255),
            Math.floor((b + m) * 255)
        ];
    }
    
    // Progressive rendering with Web Workers simulation
    async render() {
        if (this.isRendering) return;
        
        this.isRendering = true;
        this.renderStartTime = performance.now();
        this.pixelsRendered = 0;
        
        // Update UI
        document.getElementById('render-btn').textContent = '⏳ Rendering...';
        document.getElementById('render-btn').disabled = true;
        
        // Calculate complex plane bounds
        const scale = 4.0 / this.zoom;
        const minX = this.centerX - scale/2;
        const maxX = this.centerX + scale/2;
        const minY = this.centerY - scale/2;
        const maxY = this.centerY + scale/2;
        
        // Progressive rendering - render in chunks for responsiveness
        const chunkSize = 1000; // pixels per chunk
        let pixelIndex = 0;
        
        const renderChunk = () => {
            const chunkStart = performance.now();
            let pixelsInChunk = 0;
            
            while (pixelsInChunk < chunkSize && pixelIndex < this.width * this.height) {
                const x = pixelIndex % this.width;
                const y = Math.floor(pixelIndex / this.width);
                
                // Map pixel to complex plane
                const cx = minX + (maxX - minX) * x / (this.width - 1);
                const cy = minY + (maxY - minY) * y / (this.height - 1);
                
                // Compute fractal iterations
                const iterations = this.computeIterations(cx, cy);
                const [r, g, b] = this.iterationsToColor(iterations);
                
                // Set pixel in image data
                const dataIndex = pixelIndex * 4;
                this.imageData.data[dataIndex] = r;
                this.imageData.data[dataIndex + 1] = g;
                this.imageData.data[dataIndex + 2] = b;
                this.imageData.data[dataIndex + 3] = 255; // Alpha
                
                pixelIndex++;
                pixelsInChunk++;
                this.pixelsRendered++;
            }
            
            // Update canvas
            this.ctx.putImageData(this.imageData, 0, 0);
            
            // Update progress
            const progress = (pixelIndex / (this.width * this.height)) * 100;
            document.getElementById('progress').style.width = progress + '%';
            
            // Update performance stats
            const elapsed = performance.now() - this.renderStartTime;
            const pixelsPerSecond = Math.floor(this.pixelsRendered / (elapsed / 1000));
            document.getElementById('web-perf').textContent = '~' + (pixelsPerSecond / 1000).toFixed(0) + 'K';
            document.getElementById('stats').textContent = 
                `Rendered: ${this.pixelsRendered.toLocaleString()} pixels | ` +
                `Speed: ${pixelsPerSecond.toLocaleString()} pixels/sec | ` +
                `Progress: ${progress.toFixed(1)}%`;
            
            if (pixelIndex < this.width * this.height) {
                // Continue rendering next chunk
                setTimeout(renderChunk, 1);
            } else {
                // Rendering complete
                this.finishRendering(elapsed);
            }
        };
        
        // Start progressive rendering
        setTimeout(renderChunk, 1);
    }
    
    finishRendering(elapsed) {
        this.isRendering = false;
        
        // Final performance stats
        const pixelsPerSecond = Math.floor(this.pixelsRendered / (elapsed / 1000));
        document.getElementById('stats').textContent = 
            `✅ Complete! ${this.pixelsRendered.toLocaleString()} pixels in ${elapsed.toFixed(0)}ms ` +
            `(${pixelsPerSecond.toLocaleString()} pixels/sec)`;
        
        // Reset UI
        document.getElementById('render-btn').textContent = '🚀 Render';
        document.getElementById('render-btn').disabled = false;
        document.getElementById('progress').style.width = '100%';
        
        // Reset progress bar after delay
        setTimeout(() => {
            document.getElementById('progress').style.width = '0%';
        }, 2000);
    }
}

// Initialize when page loads
document.addEventListener('DOMContentLoaded', () => {
    new FractalRenderer();
    
    console.log('🌀 Multi-Fractal Web Renderer initialized!');
    console.log('💡 Click on the fractal to zoom in, drag to pan');
    console.log('🔄 Switch between Mandelbrot Set and Julia Set');
    console.log('⚙️ Adjust parameters using the control panel');
    console.log('🚀 For ultimate performance, check out the CUDA GPU version!');
    console.log('👨‍💻 Created by Geoffrey Wang with Claude AI assistance');
    console.log('📁 https://github.com/GeoffreyWang1117/Mandelbrot-Renderer');
});

// Add some keyboard shortcuts
document.addEventListener('keydown', (e) => {
    if (e.code === 'Space') {
        e.preventDefault();
        document.getElementById('render-btn').click();
    } else if (e.code === 'KeyR') {
        document.getElementById('reset-btn').click();
    } else if (e.code === 'KeyS') {
        document.getElementById('save-btn').click();
    }
});
