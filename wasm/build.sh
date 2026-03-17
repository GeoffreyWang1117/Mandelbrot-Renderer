#!/bin/bash
# WebAssembly fractal renderer build script
# Requires Emscripten SDK

set -e

echo "=== WebAssembly Fractal Build ==="

if ! command -v emcc &> /dev/null; then
    echo "Error: Emscripten compiler (emcc) not found."
    echo "Install: https://emscripten.org/docs/getting_started/downloads.html"
    exit 1
fi

BUILD_DIR="build"
rm -rf "$BUILD_DIR"
mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
emmake make -j$(nproc)

echo ""
ls -lh fractals.js fractals.wasm

cd ..

# Copy to docs/ (GitHub Pages deployment)
cp build/fractals.js ../docs/
cp build/fractals.wasm ../docs/

echo ""
echo "Build complete. Files copied to docs/"
echo "Test: npx http-server ../docs -p 8080"
