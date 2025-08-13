# Mandelbrot 分形渲染器项目
# 支持多种并行计算方式: CPU/OpenMP/CUDA/OpenGL
#
# 编译选项:
# - CPU版本: cmake .. && make mandelbrot_cpu
# - OpenMP版本: cmake -DENABLE_OPENMP=ON .. && make mandelbrot_omp  
# - CUDA版本: cmake -DENABLE_CUDA=ON .. && make mandelbrot_cuda
# - 完整版本: cmake -DENABLE_ALL=ON .. && make

cmake_minimum_required(VERSION 3.12)
project(MandelbrotRenderer LANGUAGES CXX)

# C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")

# 默认Release模式
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 创建输出目录
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

# =============================================================================
# CPU版本 (基础实现)
# =============================================================================
add_executable(mandelbrot_cpu
    src/main.cpp
    src/render.cpp
)

target_compile_definitions(mandelbrot_cpu PRIVATE CPU_VERSION)

# =============================================================================
# 可选组件配置
# =============================================================================

# OpenMP并行版本
option(ENABLE_OPENMP "Enable OpenMP parallel rendering" OFF)
if(ENABLE_OPENMP OR ENABLE_ALL)
    find_package(OpenMP REQUIRED)
    
    add_executable(mandelbrot_omp
        src/main_unified.cpp
        src/render.cpp
        src/render_omp.cpp
    )
    
    target_link_libraries(mandelbrot_omp OpenMP::OpenMP_CXX)
    target_compile_definitions(mandelbrot_omp PRIVATE OPENMP_VERSION)
    
    message(STATUS "OpenMP版本已启用")
endif()

# CUDA GPU版本  
option(ENABLE_CUDA "Enable CUDA GPU rendering" OFF)
if(ENABLE_CUDA OR ENABLE_ALL)
    enable_language(CUDA)
    find_package(CUDAToolkit REQUIRED)
    
    # Set CUDA properties for compatibility
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
    set(CMAKE_CUDA_ARCHITECTURES "80;86;89") # RTX 3090 (Ampere) + RTX 4090 (Ada Lovelace)
    
    # CUDA compiler flags for compatibility
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --expt-relaxed-constexpr --extended-lambda")
    
    add_executable(mandelbrot_cuda
        src/main.cpp
        src/render.cpp
        src/render_cuda.cu  # 待实现
    )
    
    set_target_properties(mandelbrot_cuda PROPERTIES
        CUDA_RUNTIME_LIBRARY Shared
        CUDA_ARCHITECTURES "50;60;70;75;80;86"
    )
    
    target_compile_definitions(mandelbrot_cuda PRIVATE CUDA_VERSION)
    
    message(STATUS "CUDA版本已启用")
endif()

# OpenGL实时渲染版本
option(ENABLE_OPENGL "Enable OpenGL interactive rendering" OFF)
if(ENABLE_OPENGL OR ENABLE_ALL)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)
    
    add_executable(mandelbrot_gl
        src/main.cpp
        src/render.cpp
        src/render_gl.cpp   # 待实现
        src/window.cpp      # 待实现
    )
    
    target_link_libraries(mandelbrot_gl OpenGL::GL glfw GLEW::GLEW)
    target_compile_definitions(mandelbrot_gl PRIVATE OPENGL_VERSION)
    
    message(STATUS "OpenGL版本已启用")
endif()

# 完整版本 (包含所有功能)
option(ENABLE_ALL "Enable all rendering modes" OFF)

# =============================================================================
# 安装配置
# =============================================================================
install(TARGETS mandelbrot_cpu
    RUNTIME DESTINATION bin
)

if(TARGET mandelbrot_omp)
    install(TARGETS mandelbrot_omp RUNTIME DESTINATION bin)
endif()

if(TARGET mandelbrot_cuda)
    install(TARGETS mandelbrot_cuda RUNTIME DESTINATION bin)
endif()

if(TARGET mandelbrot_gl)
    install(TARGETS mandelbrot_gl RUNTIME DESTINATION bin)
endif()

# =============================================================================
# 自定义目标
# =============================================================================

# 生成示例图像
add_custom_target(run_demo
    COMMAND ${CMAKE_BINARY_DIR}/mandelbrot_cpu --width 800 --height 600 --output output/demo_cpu.ppm
    DEPENDS mandelbrot_cpu
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "生成示例Mandelbrot图像"
)

# 性能基准测试
add_custom_target(benchmark
    COMMAND echo "=== CPU性能基准测试 ==="
    COMMAND ${CMAKE_BINARY_DIR}/mandelbrot_cpu --width 1920 --height 1080 --iter 1000 --output output/benchmark_cpu.ppm
    DEPENDS mandelbrot_cpu
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "运行性能基准测试"
)

# 清理输出文件
add_custom_target(clean_output
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_SOURCE_DIR}/output/*.ppm
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_SOURCE_DIR}/output/*.png  
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_SOURCE_DIR}/output/*.mp4
    COMMENT "清理输出文件"
)

# =============================================================================
# 项目信息
# =============================================================================
message(STATUS "=== Mandelbrot渲染器项目配置 ===")
message(STATUS "项目目录: ${CMAKE_SOURCE_DIR}")
message(STATUS "构建目录: ${CMAKE_BINARY_DIR}")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "CPU版本: 已启用")
message(STATUS "OpenMP版本: ${ENABLE_OPENMP}")
message(STATUS "CUDA版本: ${ENABLE_CUDA}")
message(STATUS "OpenGL版本: ${ENABLE_OPENGL}")
message(STATUS "")
message(STATUS "编译命令:")
message(STATUS "  mkdir build && cd build")
message(STATUS "  cmake .. && make")
message(STATUS "")
message(STATUS "运行示例:")
message(STATUS "  make run_demo")
message(STATUS "  make benchmark")
