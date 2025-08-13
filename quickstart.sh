#!/bin/bash
# Mandelbrot 分形渲染器 - 快速开始脚本
# 
# 这个脚本演示了项目的主要功能
# 运行: chmod +x quickstart.sh && ./quickstart.sh

echo "🌀 Mandelbrot 分形渲染器 - 快速开始演示"
echo "================================================="

# 检查构建状态
if [ ! -f "build/mandelbrot_cpu" ]; then
    echo "⚠️  检测到项目尚未编译，开始编译..."
    mkdir -p build
    cd build
    cmake .. && make mandelbrot_cpu
    cd ..
    
    if [ ! -f "build/mandelbrot_cpu" ]; then
        echo "❌ 编译失败，请检查环境配置"
        exit 1
    fi
    echo "✅ 编译完成！"
fi

echo ""
echo "📊 演示 1: 基础渲染 (经典Mandelbrot视图)"
build/mandelbrot_cpu --width 400 --height 300 --iter 500 --output output/quickstart_classic.ppm

echo ""
echo "📊 演示 2: 高迭代渲染 (精细细节)"
build/mandelbrot_cpu --width 300 --height 200 --iter 2000 --output output/quickstart_detailed.ppm

echo ""
echo "📊 演示 3: 局部放大 (有趣区域)"
build/mandelbrot_cpu --width 300 --height 200 --iter 1000 \
    --xmin -0.8 --xmax -0.7 --ymin 0.0 --ymax 0.1 \
    --output output/quickstart_zoom.ppm

echo ""
echo "📊 演示 4: 微型动画 (5帧缩放)"
if command -v ffmpeg >/dev/null 2>&1; then
    echo "🎬 检测到FFmpeg，生成动画..."
    python3 scripts/make_video.py --type zoom --frames 5 --resolution 200x150 --output quickstart_animation.mp4 --fps 15 <<< "y"
    echo "✅ 动画已保存: output/quickstart_animation.mp4"
else
    echo "⚠️  FFmpeg未安装，跳过动画演示"
    echo "   安装命令: sudo apt install ffmpeg (Ubuntu/Debian)"
    echo "   或        brew install ffmpeg (macOS)"
fi

echo ""
echo "📁 生成的文件:"
ls -la output/quickstart_* 2>/dev/null || echo "   (文件在 output/ 目录中)"

echo ""
echo "🎯 下一步建议:"
echo "   1. 安装ImageMagick转换图像格式:"
echo "      convert output/quickstart_classic.ppm output/classic.png"
echo ""
echo "   2. 尝试更大分辨率渲染:"
echo "      build/mandelbrot_cpu --width 1920 --height 1080 --iter 1000"
echo ""
echo "   3. 生成更长的动画:"
echo "      python3 scripts/make_video.py --type zoom --frames 60 --resolution 800x600"
echo ""
echo "   4. 实现OpenMP并行版本获得更高性能!"

echo ""
echo "🌟 演示完成! 探索无限的分形之美吧！"
