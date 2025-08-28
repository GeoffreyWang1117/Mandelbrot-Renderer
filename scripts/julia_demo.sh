#!/bin/bash
# Julia Set Demo Generator
# Generates multiple Julia Set images with different parameters
# Author: Geoffrey Wang

echo "🎨 Julia Set Demo Generator"
echo "Generating multiple Julia Set variations..."

cd build

# Run the julia test to generate classic Julia set
echo "Rendering Classic Julia Set..."
./julia_test

echo "✅ Julia Set Demo Complete!"
echo "📁 Output files generated in build/ directory:"
ls -la *.ppm
echo ""
echo "🎯 Next steps:"
echo "  • View the generated .ppm files"
echo "  • Try the interactive web demo at https://geoffreywtech.me/Mandelbrot-Renderer/"
echo "  • Switch between Mandelbrot and Julia sets in the browser"
echo ""
echo "💡 Web demo features:"
echo "  • Real-time Julia parameter adjustment"
echo "  • Multiple preset configurations" 
echo "  • Interactive zoom and pan"
