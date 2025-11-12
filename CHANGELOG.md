# Changelog

All notable changes to the Mandelbrot-Renderer project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.0] - 2025-11-12

### Added - Project Modernization & Enhancement

#### 🎯 **New Features**

1. **Package.json & Modern Tooling**
   - Added `package.json` for professional Node.js dependency management
   - Integrated npm scripts for development, linting, and formatting
   - Set up modern development workflow
   - Added scripts: `dev`, `dev:docs`, `lint`, `format`, `build:wasm`

2. **URL Parameter Support & Sharing**
   - Share exact fractal views via URL parameters
   - Automatic clipboard copy functionality
   - URL parameters include: fractal type, center position, zoom, iterations, Julia parameters
   - Deep linking support for direct access to specific views
   - Browser history integration (URL updates without page reload)

3. **Enhanced Preset Locations**
   - Added 3 new preset locations per fractal type:
     - **Elephant Valley**: Detailed self-similar structures
     - **Galaxy Spiral**: Sweeping spiral formations
     - **Lightning Bolt**: Sharp fractal boundaries
   - Total of 6 presets per fractal (Classic, Spiral, Seahorse, Elephant, Galaxy, Lightning)
   - Presets now include optimized iteration counts for better detail

4. **Favorites System**
   - Save unlimited custom fractal views (localStorage-based)
   - Name your favorite discoveries
   - Manage favorites: load, delete individual, or clear all
   - Persistent across browser sessions
   - Timestamps for tracking when favorites were saved
   - Quick access to frequently explored regions

5. **Code Quality Tools**
   - **ESLint Configuration**: JavaScript linting with sensible defaults
   - **Prettier Configuration**: Automatic code formatting
   - `.prettierignore`: Exclude build artifacts and generated files
   - Code style consistency across the project

6. **Documentation**
   - **CONTRIBUTING.md**: Comprehensive contribution guidelines
     - Development setup instructions
     - Code style guidelines (C++, JavaScript, Python)
     - Pull request process
     - Testing procedures
     - Areas for contribution
   - **CHANGELOG.md**: Track project evolution

#### 🔧 **Improvements**

1. **Enhanced Web Interface**
   - Added "Share View" button with automatic clipboard copy
   - Better preset organization with more options
   - Favorites management UI
   - Improved button styling with gradient backgrounds
   - Better iteration count management in presets

2. **Better Developer Experience**
   - Modern npm scripts for common tasks
   - ESLint + Prettier for consistent code quality
   - Clear contribution guidelines
   - Structured project for easier onboarding

3. **Code Organization**
   - Synchronized web and docs directories
   - Consistent file structure
   - Better separation of concerns

#### 📚 **Documentation Updates**

- Added package.json with comprehensive metadata
- Created CONTRIBUTING.md with detailed guidelines
- Added CHANGELOG.md for tracking changes
- Enhanced code comments and documentation

### Technical Details

#### New JavaScript Functions
- `shareView()`: Generate and share URLs for current view
- `loadStateFromURL()`: Parse URL parameters and restore state
- `showShareDialog()`: Fallback for clipboard unavailable
- `saveFavorite()`: Save current view to localStorage
- `manageFavorites()`: Interactive favorites management
- `loadFavorite()`: Restore a saved favorite view
- Enhanced `loadPreset()`: Added 3 new presets per fractal type

#### New Configuration Files
- `.eslintrc.json`: ESLint configuration
- `.prettierrc.json`: Prettier formatting rules
- `.prettierignore`: Files to exclude from formatting
- `package.json`: Node.js project configuration
- `CONTRIBUTING.md`: Contribution guidelines
- `CHANGELOG.md`: This file

### Browser Compatibility

All new features work on:
- Chrome 57+ (full support)
- Firefox 52+ (full support)
- Safari 11+ (full support)
- Edge 79+ (full support)

URL Sharing requires:
- URLSearchParams API (all modern browsers)
- Clipboard API (HTTPS required for automatic copy)

LocalStorage for Favorites:
- All modern browsers with localStorage support
- ~5-10MB storage capacity (thousands of favorites)

---

## [1.9.0] - Previous Release

### Completed Features
- ✅ Mandelbrot Set rendering
- ✅ Julia Set implementation
- ✅ Burning Ship fractal
- ✅ Newton fractal (z³ - 1 = 0)
- ✅ WebAssembly high-performance version
- ✅ Interactive web demo (JavaScript)
- ✅ CUDA GPU acceleration
- ✅ OpenMP multi-threading
- ✅ Progressive rendering
- ✅ Click to zoom functionality
- ✅ Drag to pan
- ✅ Image export (PNG)
- ✅ Color gradients (HSV color space)
- ✅ Mobile responsive design
- ✅ Performance metrics display
- ✅ Multiple resolution options

---

## Future Roadmap

### High Priority (Next Release)
- [ ] OpenGL real-time GUI with GLFW
- [ ] Web Worker support for non-blocking rendering
- [ ] Additional fractal types (Sierpinski, Barnsley Fern)
- [ ] Advanced color schemes (palette editor)
- [ ] Keyboard shortcuts guide
- [ ] Touch gesture improvements (pinch-to-zoom)

### Medium Priority
- [ ] Animation export (GIF/MP4)
- [ ] Multi-GPU rendering support
- [ ] Precision mode (arbitrary precision arithmetic)
- [ ] Batch rendering CLI tool
- [ ] Docker containerization
- [ ] Unit test suite

### Long-term Vision
- [ ] Native mobile apps (iOS/Android)
- [ ] VR/AR fractal exploration
- [ ] Cloud rendering service
- [ ] Machine learning optimization
- [ ] Educational platform integration

---

**Contributors**: Geoffrey Wang (with Claude AI assistance)

**Repository**: https://github.com/GeoffreyWang1117/Mandelbrot-Renderer

**License**: MIT
