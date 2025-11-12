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
        this.fractalType = 'mandelbrot'; // 'mandelbrot', 'julia', 'burning_ship', 'newton'
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

        // Load state from URL if present
        this.loadStateFromURL();

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
        document.getElementById('share-btn').addEventListener('click', () => this.shareView());
        
        // Preset locations (update for current fractal)
        document.getElementById('classic-btn').addEventListener('click', () => this.loadPreset('classic'));
        document.getElementById('spiral-btn').addEventListener('click', () => this.loadPreset('spiral'));
        document.getElementById('seahorse-btn').addEventListener('click', () => this.loadPreset('seahorse'));
        document.getElementById('elephant-btn').addEventListener('click', () => this.loadPreset('elephant'));
        document.getElementById('galaxy-btn').addEventListener('click', () => this.loadPreset('galaxy'));
        document.getElementById('lightning-btn').addEventListener('click', () => this.loadPreset('lightning'));

        // Favorites
        document.getElementById('save-favorite-btn').addEventListener('click', () => this.saveFavorite());
        document.getElementById('manage-favorites-btn').addEventListener('click', () => this.manageFavorites());
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

    // Share current view via URL
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

        const shareURL = window.location.origin + window.location.pathname + '?' + params.toString();

        // Copy to clipboard
        if (navigator.clipboard && navigator.clipboard.writeText) {
            navigator.clipboard.writeText(shareURL).then(() => {
                alert('🔗 Share link copied to clipboard!\n\nAnyone with this link can view your exact fractal view.');
            }).catch(() => {
                // Fallback for clipboard failure
                this.showShareDialog(shareURL);
            });
        } else {
            this.showShareDialog(shareURL);
        }

        // Also update browser URL without reload
        window.history.replaceState({}, '', shareURL);
    }

    // Show share dialog fallback
    showShareDialog(url) {
        const message = `Share this link:\n\n${url}`;
        alert(message);
    }

    // Load fractal state from URL parameters
    loadStateFromURL() {
        const params = new URLSearchParams(window.location.search);

        if (params.has('fractal')) {
            this.fractalType = params.get('fractal');
            document.getElementById('fractal-type').value = this.fractalType;
            this.updateFractalControls();
        }

        if (params.has('cx')) {
            this.centerX = parseFloat(params.get('cx'));
        }

        if (params.has('cy')) {
            this.centerY = parseFloat(params.get('cy'));
        }

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
    }
    
    // Update UI controls based on fractal type
    updateFractalControls() {
        const juliaControls = document.getElementById('julia-controls');
        if (this.fractalType === 'julia') {
            juliaControls.style.display = 'block';
            this.centerX = 0.0;
            this.centerY = 0.0;
            this.zoom = 1.0;
        } else {
            juliaControls.style.display = 'none';
            // Set default center and zoom for each fractal type
            switch(this.fractalType) {
                case 'mandelbrot':
                    this.centerX = -0.5; this.centerY = 0.0; this.zoom = 1.0;
                    break;
                case 'burning_ship':
                    this.centerX = -0.5; this.centerY = -0.5; this.zoom = 1.0;
                    break;
                case 'newton':
                    this.centerX = 0.0; this.centerY = 0.0; this.zoom = 1.0;
                    break;
                default:
                    this.centerX = -0.5; this.centerY = 0.0; this.zoom = 1.0;
            }
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
                    this.maxIterations = 256;
                    break;
                case 'seahorse':
                    this.centerX = -0.7453; this.centerY = 0.1127; this.zoom = 200;
                    this.maxIterations = 256;
                    break;
                case 'elephant':
                    this.centerX = 0.28693186889504513; this.centerY = 0.014286693904085048;
                    this.zoom = 300; this.maxIterations = 512;
                    break;
                case 'galaxy':
                    this.centerX = -0.7; this.centerY = 0.0; this.zoom = 50;
                    this.maxIterations = 256;
                    break;
                case 'lightning':
                    this.centerX = -0.16; this.centerY = 1.0405; this.zoom = 150;
                    this.maxIterations = 256;
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
                case 'elephant':
                    this.juliaC.x = 0.3; this.juliaC.y = 0.5;
                    this.centerX = 0.0; this.centerY = 0.0; this.zoom = 1.0;
                    break;
                case 'galaxy':
                    this.juliaC.x = -0.8; this.juliaC.y = 0.156;
                    this.centerX = 0.0; this.centerY = 0.0; this.zoom = 1.0;
                    break;
                case 'lightning':
                    this.juliaC.x = -0.70176; this.juliaC.y = -0.3842;
                    this.centerX = 0.0; this.centerY = 0.0; this.zoom = 1.0;
                    break;
            }
            // Update Julia parameter sliders
            document.getElementById('julia-cx').value = this.juliaC.x;
            document.getElementById('julia-cx-value').textContent = this.juliaC.x.toFixed(4);
            document.getElementById('julia-cy').value = this.juliaC.y;
            document.getElementById('julia-cy-value').textContent = this.juliaC.y.toFixed(4);
        } else if (this.fractalType === 'burning_ship') {
            switch(preset) {
                case 'classic':
                    this.centerX = -0.5; this.centerY = -0.5; this.zoom = 1.0;
                    break;
                case 'spiral':
                    this.centerX = -1.7269; this.centerY = -0.0311; this.zoom = 100;
                    break;
                case 'seahorse':
                    this.centerX = -1.775; this.centerY = -0.01; this.zoom = 500;
                    break;
                case 'elephant':
                    this.centerX = -1.74; this.centerY = -0.028; this.zoom = 200;
                    this.maxIterations = 256;
                    break;
                case 'galaxy':
                    this.centerX = -1.65; this.centerY = -0.035; this.zoom = 80;
                    this.maxIterations = 256;
                    break;
                case 'lightning':
                    this.centerX = -1.72; this.centerY = -0.025; this.zoom = 150;
                    this.maxIterations = 256;
                    break;
            }
        } else if (this.fractalType === 'newton') {
            switch(preset) {
                case 'classic':
                    this.centerX = 0.0; this.centerY = 0.0; this.zoom = 1.0;
                    break;
                case 'spiral':
                    this.centerX = 0.0; this.centerY = 0.0; this.zoom = 3.0;
                    break;
                case 'seahorse':
                    this.centerX = 0.5; this.centerY = 0.866; this.zoom = 20.0;
                    break;
                case 'elephant':
                    this.centerX = 1.0; this.centerY = 0.0; this.zoom = 15.0;
                    this.maxIterations = 100;
                    break;
                case 'galaxy':
                    this.centerX = 0.0; this.centerY = 1.0; this.zoom = 8.0;
                    this.maxIterations = 100;
                    break;
                case 'lightning':
                    this.centerX = 0.866; this.centerY = 0.5; this.zoom = 25.0;
                    this.maxIterations = 150;
                    break;
            }
        }
        
        // Update UI sliders
        document.getElementById('zoom').value = this.zoom;
        document.getElementById('zoom-value').textContent = this.zoom.toFixed(1) + 'x';
        document.getElementById('iterations').value = this.maxIterations;
        document.getElementById('iterations-value').textContent = this.maxIterations.toString();

        this.render();
    }

    // Save current view as favorite
    saveFavorite() {
        const name = prompt('Enter a name for this favorite view:');
        if (!name) return;

        const favorite = {
            name: name,
            fractalType: this.fractalType,
            centerX: this.centerX,
            centerY: this.centerY,
            zoom: this.zoom,
            maxIterations: this.maxIterations,
            juliaC: { ...this.juliaC },
            timestamp: Date.now()
        };

        // Get existing favorites from localStorage
        let favorites = JSON.parse(localStorage.getItem('fractal-favorites') || '[]');
        favorites.push(favorite);

        // Limit to 20 favorites
        if (favorites.length > 20) {
            favorites = favorites.slice(-20);
        }

        localStorage.setItem('fractal-favorites', JSON.stringify(favorites));
        alert('⭐ Favorite saved successfully!\n\nName: ' + name);
    }

    // Manage favorites dialog
    manageFavorites() {
        const favorites = JSON.parse(localStorage.getItem('fractal-favorites') || '[]');

        if (favorites.length === 0) {
            alert('📋 No favorites saved yet!\n\nClick "⭐ Save Favorite" to save interesting views.');
            return;
        }

        let message = '📋 Your Saved Favorites:\n\n';
        favorites.forEach((fav, index) => {
            const date = new Date(fav.timestamp).toLocaleDateString();
            message += `${index + 1}. ${fav.name} (${fav.fractalType}) - ${date}\n`;
        });
        message += '\nEnter a number to load, or "delete X" to remove (e.g., "delete 2"), or "clear" to remove all:';

        const input = prompt(message);
        if (!input) return;

        if (input.toLowerCase() === 'clear') {
            if (confirm('Are you sure you want to delete ALL favorites?')) {
                localStorage.removeItem('fractal-favorites');
                alert('🗑️ All favorites cleared!');
            }
            return;
        }

        if (input.toLowerCase().startsWith('delete ')) {
            const indexToDelete = parseInt(input.substring(7)) - 1;
            if (indexToDelete >= 0 && indexToDelete < favorites.length) {
                const deleted = favorites.splice(indexToDelete, 1)[0];
                localStorage.setItem('fractal-favorites', JSON.stringify(favorites));
                alert(`🗑️ Deleted: ${deleted.name}`);
                this.manageFavorites(); // Reopen dialog
            } else {
                alert('❌ Invalid favorite number!');
            }
            return;
        }

        const index = parseInt(input) - 1;
        if (index >= 0 && index < favorites.length) {
            this.loadFavorite(favorites[index]);
        } else {
            alert('❌ Invalid favorite number!');
        }
    }

    // Load a favorite view
    loadFavorite(favorite) {
        this.fractalType = favorite.fractalType;
        this.centerX = favorite.centerX;
        this.centerY = favorite.centerY;
        this.zoom = favorite.zoom;
        this.maxIterations = favorite.maxIterations;
        this.juliaC = { ...favorite.juliaC };

        // Update UI
        document.getElementById('fractal-type').value = this.fractalType;
        this.updateFractalControls();
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

        alert(`✅ Loaded favorite: ${favorite.name}`);
        this.render();
    }
    
    // Core fractal computation dispatcher
    computeIterations(cx, cy) {
        if (this.fractalType === 'mandelbrot') {
            return this.mandelbrotIterations(cx, cy);
        } else if (this.fractalType === 'julia') {
            return this.juliaIterations(cx, cy);
        } else if (this.fractalType === 'burning_ship') {
            return this.burningShipIterations(cx, cy);
        } else if (this.fractalType === 'newton') {
            return this.newtonIterations(cx, cy);
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
    
    // Core Burning Ship computation
    burningShipIterations(cx, cy) {
        let x = 0.0, y = 0.0;
        let iterations = 0;
        
        while (x*x + y*y < 4.0 && iterations < this.maxIterations) {
            // Burning Ship: z = (|Re(z)| + i|Im(z)|)² + c
            const abs_x = Math.abs(x);
            const abs_y = Math.abs(y);
            const xtemp = abs_x*abs_x - abs_y*abs_y + cx;
            y = 2*abs_x*abs_y + cy;
            x = xtemp;
            iterations++;
        }
        
        return iterations;
    }
    
    // Core Newton fractal computation
    newtonIterations(cx, cy) {
        let x = cx, y = cy;
        let iterations = 0;
        
        const convergenceThreshold = 1e-6;
        
        // Newton's method for z³ - 1 = 0
        while (iterations < this.maxIterations) {
            const x_old = x, y_old = y;
            
            // Compute z² and z³
            const x2 = x*x - y*y;
            const y2 = 2*x*y;
            const x3 = x2*x - y2*y;
            const y3 = x2*y + y2*x;
            
            // Newton iteration: z = (2z³ + 1) / (3z²)
            const denom = 3 * (x2*x2 + y2*y2);
            if (denom < 1e-10) break;
            
            x = (2*x3 + 1) / denom * x2 + (2*y3) / denom * y2;
            y = (2*y3) / denom * x2 - (2*x3 + 1) / denom * y2;
            
            // Check convergence
            if ((x - x_old)*(x - x_old) + (y - y_old)*(y - y_old) < convergenceThreshold) {
                // Determine which root we converged to and encode in iterations
                if (Math.abs(x - 1) < 0.1 && Math.abs(y) < 0.1) {
                    return this.maxIterations + 1; // Root 1 (red)
                } else if (Math.abs(x + 0.5) < 0.1 && Math.abs(y - 0.866) < 0.1) {
                    return this.maxIterations + 2; // Root 2 (green)
                } else if (Math.abs(x + 0.5) < 0.1 && Math.abs(y + 0.866) < 0.1) {
                    return this.maxIterations + 3; // Root 3 (blue)
                }
                break;
            }
            iterations++;
        }
        
        return iterations;
    }
    
    // Convert iterations to color
    iterationsToColor(iterations) {
        // Special handling for Newton fractal roots
        if (this.fractalType === 'newton') {
            if (iterations > this.maxIterations) {
                const root = iterations - this.maxIterations;
                switch (root) {
                    case 1: return [255, 100, 100]; // Red for root 1
                    case 2: return [100, 255, 100]; // Green for root 2  
                    case 3: return [100, 100, 255]; // Blue for root 3
                }
            }
            return [50, 50, 50]; // Dark gray for no convergence
        }
        
        if (iterations === this.maxIterations) {
            return [0, 0, 0]; // Black for points in the set
        }
        
        // Enhanced color mapping based on fractal type
        const t = iterations / this.maxIterations;
        let hue, saturation, value;
        
        if (this.fractalType === 'burning_ship') {
            // Fire-like colors for Burning Ship
            if (t < 0.16) {
                hue = 240 + t * 60 / 0.16;
                saturation = 1.0;
                value = 0.5 + t * 0.5 / 0.16;
            } else if (t < 0.42) {
                hue = 300 + (t - 0.16) * 60 / 0.26;
                saturation = 1.0;
                value = 1.0;
            } else if (t < 0.6425) {
                hue = 0 + (t - 0.42) * 30 / 0.2225;
                saturation = 1.0;
                value = 1.0;
            } else if (t < 0.8575) {
                hue = 30 + (t - 0.6425) * 30 / 0.215;
                saturation = 1.0;
                value = 1.0;
            } else {
                hue = 60;
                saturation = 1.0 - (t - 0.8575) / 0.1425;
                value = 1.0;
            }
        } else {
            // Standard smooth HSV coloring for Mandelbrot/Julia
            hue = t * 360;
            saturation = 1.0;
            value = t < 1.0 ? 1.0 : 0.0;
        }
        
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
