#!/bin/bash

# WebAssembly 分形渲染器构建脚本
# 需要预先安装 Emscripten SDK

set -e  # 遇到错误立即退出

echo "🎯 WebAssembly 分形渲染器构建脚本"
echo "=================================="

# 检查 Emscripten 环境
if ! command -v emcc &> /dev/null; then
    echo "❌ 错误: 未找到 Emscripten 编译器 (emcc)"
    echo "请先安装 Emscripten SDK:"
    echo "  git clone https://github.com/emscripten-core/emsdk.git"
    echo "  cd emsdk"
    echo "  ./emsdk install latest"
    echo "  ./emsdk activate latest"
    echo "  source ./emsdk_env.sh"
    exit 1
fi

echo "✅ Emscripten 环境检查通过"

# 创建构建目录
BUILD_DIR="build"
if [ -d "$BUILD_DIR" ]; then
    echo "🧹 清理旧的构建文件..."
    rm -rf "$BUILD_DIR"
fi

mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

echo "📁 创建构建目录: $BUILD_DIR"

# 配置 CMake
echo "⚙️  配置 CMake..."
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
echo "🔨 编译 WebAssembly 模块..."
emmake make -j$(nproc)

# 检查生成的文件
echo "📋 检查生成的文件:"
ls -la fractals.*

# 返回上级目录
cd ..

# 复制到 web 目录
echo "📦 复制文件到 web 目录..."
cp build/fractals.js web/
cp build/fractals.wasm web/

echo "✅ 构建完成！"
echo ""
echo "📂 生成的文件:"
echo "  • fractals.js   - WebAssembly JavaScript 胶水代码"
echo "  • fractals.wasm - WebAssembly 二进制模块"
echo ""
echo "🌐 启动方式:"
echo "  cd web"
echo "  python3 -m http.server 8080"
echo "  # 然后访问 http://localhost:8080"
echo ""
echo "⚡ 性能预期:"
echo "  • 比纯 JavaScript 快 5-20 倍"
echo "  • 支持高分辨率实时渲染"
echo "  • 内存使用优化，支持大图像"
