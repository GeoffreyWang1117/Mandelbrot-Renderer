# Contributing to Mandelbrot-Renderer

Thank you for your interest in contributing to the Mandelbrot-Renderer project! This document provides guidelines and instructions for contributing.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [How to Contribute](#how-to-contribute)
- [Code Style Guidelines](#code-style-guidelines)
- [Testing](#testing)
- [Pull Request Process](#pull-request-process)
- [Areas for Contribution](#areas-for-contribution)

## Code of Conduct

By participating in this project, you agree to maintain a respectful and inclusive environment. We expect all contributors to:

- Be respectful and considerate of others
- Welcome newcomers and help them learn
- Focus on what's best for the community
- Show empathy towards other community members

## Getting Started

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/YOUR-USERNAME/Mandelbrot-Renderer.git
   cd Mandelbrot-Renderer
   ```
3. **Add the upstream repository**:
   ```bash
   git remote add upstream https://github.com/GeoffreyWang1117/Mandelbrot-Renderer.git
   ```

## Development Setup

### Prerequisites

#### For C++ Development:
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10+
- OpenMP support (usually included with GCC/Clang)
- NVIDIA CUDA Toolkit 12.0+ (optional, for GPU version)

#### For Web Development:
- Node.js 14+ and npm 6+
- Modern web browser (Chrome, Firefox, Safari, Edge)
- Emscripten SDK (for WebAssembly builds)

### Installation

#### C++ Build:
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install build-essential cmake libomp-dev

# Build the project
mkdir build && cd build
cmake ..
make

# For CUDA version (requires NVIDIA GPU and CUDA toolkit)
cd ..
nvcc -O3 -o mandelbrot_cuda mandelbrot_cuda_standalone.cu
```

#### Web Development:
```bash
# Install Node.js dependencies
npm install

# Start local development server
npm run dev

# Or for the docs directory
npm run dev:docs
```

#### WebAssembly Build:
```bash
# Install Emscripten (if not already installed)
# Follow: https://emscripten.org/docs/getting_started/downloads.html

# Build WebAssembly
cd wasm
bash build.sh
```

## How to Contribute

### Reporting Bugs

If you find a bug, please create an issue with:
- **Clear title** describing the problem
- **Detailed description** of the bug
- **Steps to reproduce** the issue
- **Expected behavior** vs actual behavior
- **Environment details** (OS, compiler version, GPU model, etc.)
- **Screenshots or error messages** if applicable

### Suggesting Features

Feature requests are welcome! Please create an issue with:
- **Clear description** of the feature
- **Use cases** and why it would be valuable
- **Possible implementation** approach (optional)
- **Related examples** from other projects (optional)

### Submitting Code

1. **Create a branch** for your changes:
   ```bash
   git checkout -b feature/your-feature-name
   # or
   git checkout -b fix/your-bug-fix
   ```

2. **Make your changes** following our code style guidelines

3. **Test your changes** thoroughly

4. **Commit your changes** with clear, descriptive messages:
   ```bash
   git commit -m "Add: New Julia Set preset locations"
   git commit -m "Fix: Memory leak in CUDA kernel"
   git commit -m "Docs: Update README with new features"
   ```

5. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```

6. **Create a Pull Request** on GitHub

## Code Style Guidelines

### C++ Code Style

- **Indentation**: 4 spaces (no tabs)
- **Naming conventions**:
  - Classes: `PascalCase` (e.g., `FractalRenderer`)
  - Functions: `camelCase` (e.g., `calculateIterations`)
  - Variables: `camelCase` (e.g., `maxIterations`)
  - Constants: `UPPER_CASE` (e.g., `MAX_ITERATIONS`)
- **Comments**: Use `//` for single-line, `/* */` for multi-line
- **Headers**: Include guards using `#pragma once`
- **Formatting**: Follow existing code style

Example:
```cpp
// Good
class MandelbrotRenderer {
public:
    void renderFrame(int width, int height);

private:
    int maxIterations;
    double zoomLevel;
};

// Not preferred
class mandelbrot_renderer {
    void RenderFrame(int Width, int Height);
};
```

### JavaScript Code Style

We use **ESLint** and **Prettier** for consistent code formatting.

- **Indentation**: 4 spaces
- **Quotes**: Single quotes for strings
- **Semicolons**: Required
- **Naming conventions**:
  - Classes: `PascalCase`
  - Functions: `camelCase`
  - Variables: `camelCase`
  - Constants: `UPPER_CASE`

Run formatting before committing:
```bash
npm run format
npm run lint
```

Example:
```javascript
// Good
class FractalRenderer {
    constructor() {
        this.maxIterations = 100;
    }

    render() {
        // Implementation
    }
}

// Not preferred
class fractal_renderer{
  constructor(){
    this.MaxIterations=100
  }
}
```

### Python Code Style

- **Follow PEP 8** guidelines
- **Indentation**: 4 spaces
- **Line length**: Maximum 100 characters
- **Naming conventions**:
  - Functions: `snake_case`
  - Classes: `PascalCase`
  - Constants: `UPPER_CASE`

## Testing

### C++ Testing

Before submitting:
```bash
# Test CPU version
./build/mandelbrot_cpu -s 800x600 -i 100 -o test_cpu.ppm

# Test OpenMP version
./build/mandelbrot_omp -s 1920x1080 -i 200 -o test_omp.ppm

# Test CUDA version (if available)
./mandelbrot_cuda -s 800x600 -i 100 -o test_cuda.ppm
```

### Web Testing

```bash
# Start local server and test in browser
npm run dev

# Check for console errors
# Test on multiple browsers (Chrome, Firefox, Safari)
# Test on mobile devices (responsive design)
```

### Manual Testing Checklist

- [ ] Code compiles without warnings
- [ ] All existing features still work
- [ ] New features work as expected
- [ ] Tested on target platforms (CPU/GPU/Web)
- [ ] No performance regressions
- [ ] Memory leaks checked (Valgrind for C++, browser DevTools for web)

## Pull Request Process

1. **Update documentation** if you've added/changed functionality
2. **Add tests** for new features if applicable
3. **Run linters and formatters**:
   ```bash
   npm run format
   npm run lint
   ```
4. **Ensure builds succeed** on all platforms you can test
5. **Update README.md** if adding major features
6. **Create detailed PR description**:
   - What changes were made
   - Why these changes are necessary
   - How to test the changes
   - Screenshots/videos if UI changes

### PR Title Format

Use clear, descriptive titles with prefixes:
- `Add:` for new features
- `Fix:` for bug fixes
- `Refactor:` for code improvements
- `Docs:` for documentation changes
- `Perf:` for performance improvements
- `Test:` for test additions

Examples:
- `Add: WebWorker support for JavaScript renderer`
- `Fix: CUDA memory leak in high-resolution rendering`
- `Perf: Optimize Julia Set computation by 30%`

## Areas for Contribution

We welcome contributions in these areas:

### High Priority
- **New Fractal Types**: Sierpinski Triangle, Barnsley Fern, Dragon Curve
- **OpenGL GUI**: Real-time interactive visualization
- **Multi-GPU Support**: Distribute rendering across multiple GPUs
- **Performance Optimizations**: CUDA kernel improvements, SIMD instructions
- **Mobile Apps**: Native iOS/Android implementations

### Medium Priority
- **Additional Color Schemes**: More coloring algorithms
- **Animation Tools**: Easier zoom animation generation
- **Documentation**: More examples, tutorials, API docs
- **Testing**: Unit tests, integration tests, benchmarks
- **Precision Options**: Arbitrary precision arithmetic

### Good First Issues
- **Bug fixes**: Check GitHub issues tagged `good-first-issue`
- **Documentation improvements**: Fix typos, add examples
- **New presets**: Add interesting fractal locations
- **UI improvements**: Better controls, tooltips, help text
- **Code cleanup**: Refactoring, removing duplicates

## Questions?

If you have questions:
- **GitHub Issues**: Create an issue with the `question` label
- **GitHub Discussions**: Start a discussion
- **Email**: Contact the maintainers

## Recognition

Contributors will be:
- Listed in the project README
- Credited in commit messages
- Mentioned in release notes

Thank you for contributing to Mandelbrot-Renderer! Your efforts help make this project better for everyone.

---

**Last Updated**: November 2025
**Maintainer**: Geoffrey Wang
