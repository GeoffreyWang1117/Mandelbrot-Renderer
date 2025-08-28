cmake_minimum_required(VERSION 3.10)
project(FractalsWASM)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 检查是否使用 Emscripten
if(NOT EMSCRIPTEN)
    message(FATAL_ERROR "This project must be built with Emscripten. Use: emcmake cmake ..")
endif()

# 源文件
set(SOURCES
    src/fractals_wasm.cpp
)

# 创建可执行文件
add_executable(fractals ${SOURCES})

# Emscripten 特定设置
set_target_properties(fractals PROPERTIES
    SUFFIX ".js"
    LINK_FLAGS "-s WASM=1 -s EXPORTED_RUNTIME_METHODS=['cwrap','ccall','HEAPU8'] -s MODULARIZE=1 -s EXPORT_NAME='FractalsModule' -s ALLOW_MEMORY_GROWTH=1 -s MAXIMUM_MEMORY=2GB -s EXPORTED_FUNCTIONS=['_malloc','_free','_testFunction','_mandelbrotIterations','_juliaIterations','_burningShipIterations','_newtonIterations','_computeFractalBatch','_renderFractalImage'] -O3 -s ASSERTIONS=1 --bind"
)

# 优化设置
target_compile_options(fractals PRIVATE
    -O3
    -ffast-math
    -DNDEBUG
)

# 生成类型定义文件
add_custom_command(TARGET fractals POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Generating TypeScript definitions..."
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# 复制到 web 目录
add_custom_command(TARGET fractals POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy fractals.js ${CMAKE_SOURCE_DIR}/web/
    COMMAND ${CMAKE_COMMAND} -E copy fractals.wasm ${CMAKE_SOURCE_DIR}/web/
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Copying WASM files to web directory"
)
