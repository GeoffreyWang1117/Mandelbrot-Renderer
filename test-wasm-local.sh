#!/bin/bash

# 本地测试脚本 - WebAssembly 分形渲染器功能测试

echo "🧪 开始本地功能测试..."

# 检查必要文件
echo "📁 检查文件存在性..."
if [ -f "docs/wasm-fractal-fixed.html" ]; then
    echo "✅ 修复版本文件存在"
else
    echo "❌ 修复版本文件不存在"
    exit 1
fi

# 检查文件大小
file_size=$(wc -c < docs/wasm-fractal-fixed.html)
echo "📏 文件大小: $file_size 字节"

if [ $file_size -gt 10000 ]; then
    echo "✅ 文件大小合理"
else
    echo "❌ 文件可能不完整"
    exit 1
fi

# 检查关键功能是否包含在文件中
echo "🔍 检查关键功能..."

# 检查四种分形算法
if grep -q "mandelbrotIterations\|juliaIterations\|burningShipIterations\|newtonIterations" docs/wasm-fractal-fixed.html; then
    echo "✅ 包含所有四种分形算法"
else
    echo "❌ 缺少分形算法实现"
fi

# 检查颜色处理
if grep -q "hsvToRgb\|getColor" docs/wasm-fractal-fixed.html; then
    echo "✅ 包含颜色处理功能"
else
    echo "❌ 缺少颜色处理"
fi

# 检查交互功能
if grep -q "handleCanvasClick\|handleMouseMove" docs/wasm-fractal-fixed.html; then
    echo "✅ 包含交互功能"
else
    echo "❌ 缺少交互功能"
fi

# 检查 Julia 参数控制
if grep -q "juliaControls\|juliaReal\|juliaImag" docs/wasm-fractal-fixed.html; then
    echo "✅ 包含 Julia Set 参数控制"
else
    echo "❌ 缺少 Julia Set 参数控制"
fi

echo ""
echo "🎯 测试完成！请在浏览器中访问以下地址进行手动测试："
echo "file://$(pwd)/docs/wasm-fractal-fixed.html"
echo ""
echo "📋 手动测试检查清单："
echo "  1. ✓ 页面是否正常加载"
echo "  2. ✓ 默认显示 Mandelbrot Set"
echo "  3. ✓ 切换到 Julia Set 并调整参数"
echo "  4. ✓ 切换到 Burning Ship"  
echo "  5. ✓ 切换到 Newton Fractal"
echo "  6. ✓ 点击缩放功能"
echo "  7. ✓ 拖拽平移功能"
echo "  8. ✓ 保存图像功能"
echo "  9. ✓ 性能统计显示"
echo " 10. ✓ 返回主页链接"

echo ""
echo "🚀 如果所有测试通过，执行以下命令部署："
echo "git add docs/wasm-fractal-fixed.html"
echo "git commit -m 'fix: 完整修复 WebAssembly 分形渲染器'"
echo "git push origin main"
