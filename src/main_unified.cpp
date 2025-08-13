/**
 * Mandelbrot 分形渲染器 - 通用主程序
 * 
 * 高性能、可视化、可交互的 Mandelbrot 分形图像渲染器
 * 支持多种渲染模式:
 * - [x] CPU 单线程版本
 * - [x] OpenMP 并行版本  
 * - [ ] CUDA GPU 版本 (待实现)
 * - [ ] OpenGL 实时渲染版本 (待实现)
 * 
 * 编译命令:
 * # CPU版本
 * g++ -std=c++17 -O3 src/main.cpp src/render.cpp -I include -o mandelbrot_cpu
 * # OpenMP版本  
 * g++ -std=c++17 -O3 -fopenmp src/main.cpp src/render.cpp src/render_omp.cpp -I include -o mandelbrot_omp
 * 
 * 使用示例:
 * ./mandelbrot_cpu --width 1920 --height 1080 --iter 2000 --output output/mandelbrot_hd.ppm
 * ./mandelbrot_omp --threads 8 --width 1920 --height 1080 --iter 2000 --output output/mandelbrot_omp.ppm
 * 
 * 作者: Geoffrey Wang (with Claude AI assistance)
 * 日期: 2025-08-12
 */

#include "../include/render.hpp"
#ifdef OPENMP_VERSION
#include "../include/render_omp.hpp"
#endif
#ifdef CUDA_VERSION
#include "../include/render_cuda.hpp"
#endif
#include <iostream>
#include <string>
#include <chrono>

enum class RenderMode {
    CPU,
    OPENMP,
    CUDA,
    OPENGL
};

RenderMode get_render_mode() {
    #ifdef OPENMP_VERSION
    return RenderMode::OPENMP;
    #elif defined(CUDA_VERSION)
    return RenderMode::CUDA;
    #elif defined(OPENGL_VERSION)
    return RenderMode::OPENGL;
    #else
    return RenderMode::CPU;
    #endif
}

std::string get_mode_name(RenderMode mode) {
    switch (mode) {
        case RenderMode::CPU: return "CPU单线程";
        case RenderMode::OPENMP: return "OpenMP并行";
        case RenderMode::CUDA: return "CUDA GPU";
        case RenderMode::OPENGL: return "OpenGL实时";
        default: return "未知";
    }
}

void print_usage(const char* program_name, RenderMode mode) {
    std::cout << "\n=== Mandelbrot 分形渲染器 (" << get_mode_name(mode) << "版本) ===" << std::endl;
    std::cout << "用法: " << program_name << " [选项]" << std::endl;
    std::cout << "\n基础选项:" << std::endl;
    std::cout << "  --width <w>     图像宽度 (默认: 800)" << std::endl;
    std::cout << "  --height <h>    图像高度 (默认: 600)" << std::endl;
    std::cout << "  --iter <n>      最大迭代次数 (默认: 1000)" << std::endl;
    std::cout << "  --xmin <x>      复平面X最小值 (默认: -2.0)" << std::endl;
    std::cout << "  --xmax <x>      复平面X最大值 (默认: 1.0)" << std::endl;
    std::cout << "  --ymin <y>      复平面Y最小值 (默认: -1.2)" << std::endl;
    std::cout << "  --ymax <y>      复平面Y最大值 (默认: 1.2)" << std::endl;
    std::cout << "  --output <file> 输出文件名 (默认: output/mandelbrot_" 
              << (mode == RenderMode::CPU ? "cpu" : mode == RenderMode::OPENMP ? "omp" : "gpu") 
              << ".ppm)" << std::endl;
    std::cout << "  --help          显示此帮助信息" << std::endl;
    
    if (mode == RenderMode::OPENMP) {
        std::cout << "\nOpenMP专用选项:" << std::endl;
        std::cout << "  --threads <n>   线程数 (默认: 自动检测)" << std::endl;
        std::cout << "  --info          显示OpenMP配置信息" << std::endl;
    }
    
    if (mode == RenderMode::CUDA) {
        std::cout << "\nCUDA专用选项:" << std::endl;
        std::cout << "  --device <n>    GPU设备ID (默认: 自动选择)" << std::endl;
        std::cout << "  --block-size <n> 线程块大小 (默认: 16)" << std::endl;
        std::cout << "  --info          显示CUDA和GPU信息" << std::endl;
    }
    
    std::cout << "\n示例:" << std::endl;
    std::cout << "  " << program_name << " --width 1920 --height 1080 --iter 2000" << std::endl;
    std::cout << "  " << program_name << " --xmin -0.8 --xmax -0.7 --ymin 0.0 --ymax 0.1" << std::endl;
    
    if (mode == RenderMode::OPENMP) {
        std::cout << "  " << program_name << " --threads 8 --width 2048 --height 1536 --iter 5000" << std::endl;
    }
    
    if (mode == RenderMode::CUDA) {
        std::cout << "  " << program_name << " --device 0 --width 3840 --height 2160 --iter 2000" << std::endl;
        std::cout << "  " << program_name << " --block-size 32 --width 1920 --height 1080" << std::endl;
    }
    
    std::cout << "\n说明:" << std::endl;
    std::cout << "  - 生成的图像为PPM格式，可用ImageMagick转换:" << std::endl;
    std::cout << "    convert output/image.ppm output/image.png" << std::endl;
    
    if (mode == RenderMode::CPU) {
        std::cout << "  - 建议使用OpenMP版本获得更高性能: mandelbrot_omp" << std::endl;
    } else if (mode == RenderMode::OPENMP) {
        std::cout << "  - 支持自动线程数检测和负载均衡优化" << std::endl;
        std::cout << "  - 预期性能: 4-8核CPU可达到3-6倍加速" << std::endl;
    }
    
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    RenderMode mode = get_render_mode();
    
    std::cout << "\n🌀 Mandelbrot 分形渲染器启动..." << std::endl;
    std::cout << "📦 当前版本: " << get_mode_name(mode) << "实现" << std::endl;
    
    if (mode == RenderMode::OPENMP) {
        std::cout << "🚀 多线程并行: 已启用" << std::endl;
        #ifdef OPENMP_VERSION
        std::cout << "🔧 OpenMP信息: 最大 " << omp_get_max_threads() << " 线程可用" << std::endl;
        #endif
    } else {
        std::cout << "🚀 计划功能: OpenMP并行 + CUDA GPU + OpenGL可视化" << std::endl;
    }
    
    // 默认渲染参数
    MandelbrotCPU::RenderParams params;
    std::string mode_suffix = (mode == RenderMode::CPU ? "cpu" : mode == RenderMode::OPENMP ? "omp" : mode == RenderMode::CUDA ? "cuda" : "gpu");
    std::string output_filename = "output/mandelbrot_" + mode_suffix + ".ppm";
    int num_threads = 0;  // 0 = 自动检测
    int device_id = -1;   // -1 = 自动选择
    int block_size = 16;  // CUDA线程块大小
    bool show_info = false;
    
    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0], mode);
            return 0;
        }
        else if (arg == "--width" && i + 1 < argc) {
            params.width = std::stoi(argv[++i]);
        }
        else if (arg == "--height" && i + 1 < argc) {
            params.height = std::stoi(argv[++i]);
        }
        else if (arg == "--iter" && i + 1 < argc) {
            params.max_iter = std::stoi(argv[++i]);
        }
        else if (arg == "--xmin" && i + 1 < argc) {
            params.x_min = std::stod(argv[++i]);
        }
        else if (arg == "--xmax" && i + 1 < argc) {
            params.x_max = std::stod(argv[++i]);
        }
        else if (arg == "--ymin" && i + 1 < argc) {
            params.y_min = std::stod(argv[++i]);
        }
        else if (arg == "--ymax" && i + 1 < argc) {
            params.y_max = std::stod(argv[++i]);
        }
        else if (arg == "--output" && i + 1 < argc) {
            output_filename = argv[++i];
        }
        else if (arg == "--threads" && i + 1 < argc && mode == RenderMode::OPENMP) {
            num_threads = std::stoi(argv[++i]);
        }
        else if (arg == "--device" && i + 1 < argc && mode == RenderMode::CUDA) {
            device_id = std::stoi(argv[++i]);
        }
        else if (arg == "--block-size" && i + 1 < argc && mode == RenderMode::CUDA) {
            block_size = std::stoi(argv[++i]);
        }
        else if (arg == "--info" && (mode == RenderMode::OPENMP || mode == RenderMode::CUDA)) {
            show_info = true;
        }
        else {
            std::cerr << "未知参数: " << arg << std::endl;
            print_usage(argv[0], mode);
            return 1;
        }
    }
    
    // 显示OpenMP/CUDA信息
    #ifdef OPENMP_VERSION
    if (show_info) {
        std::cout << "\n=== OpenMP配置信息 ===" << std::endl;
        std::cout << MandelbrotOMP::get_openmp_info() << std::endl;
        return 0;
    }
    #endif
    
    #ifdef CUDA_VERSION
    if (show_info) {
        std::cout << "\n=== CUDA和GPU信息 ===" << std::endl;
        std::cout << MandelbrotCUDA::get_cuda_info() << std::endl;
        return 0;
    }
    #endif
    
    // 参数验证
    if (params.width <= 0 || params.height <= 0) {
        std::cerr << "[ERROR] 图像尺寸必须为正数!" << std::endl;
        return 1;
    }
    if (params.max_iter <= 0) {
        std::cerr << "[ERROR] 迭代次数必须为正数!" << std::endl;
        return 1;
    }
    if (params.x_min >= params.x_max || params.y_min >= params.y_max) {
        std::cerr << "[ERROR] 坐标范围无效!" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== 渲染配置 ===" << std::endl;
    std::cout << "🖼️  图像尺寸: " << params.width << " x " << params.height 
              << " (" << (params.width * params.height / 1000000.0) << " MP)" << std::endl;
    std::cout << "🔢 最大迭代: " << params.max_iter << std::endl;
    std::cout << "📍 复平面区域: [" << params.x_min << ", " << params.x_max 
              << "] × [" << params.y_min << ", " << params.y_max << "]" << std::endl;
    std::cout << "📁 输出文件: " << output_filename << std::endl;
    
    if (mode == RenderMode::OPENMP && num_threads > 0) {
        std::cout << "🧵 线程数: " << num_threads << std::endl;
    }
    
    try {
        // 选择渲染模式
        auto start_time = std::chrono::high_resolution_clock::now();
        std::vector<unsigned char> image_data;
        
        switch (mode) {
            case RenderMode::CPU:
                image_data = MandelbrotCPU::render_mandelbrot_cpu(params);
                break;
                
            #ifdef OPENMP_VERSION
            case RenderMode::OPENMP:
                image_data = MandelbrotOMP::render_mandelbrot_omp(params, num_threads);
                break;
            #endif
            
            #ifdef CUDA_VERSION
            case RenderMode::CUDA:
                if (device_id == -1) {
                    device_id = MandelbrotCUDA::get_best_gpu_device();
                }
                image_data = MandelbrotCUDA::render_mandelbrot_cuda(params, device_id, block_size);
                break;
            #endif
                
            default:
                std::cerr << "[ERROR] 不支持的渲染模式!" << std::endl;
                return 1;
        }
        
        auto render_time = std::chrono::high_resolution_clock::now();
        
        // 保存图像
        MandelbrotCPU::save_ppm(output_filename, image_data, params.width, params.height);
        auto save_time = std::chrono::high_resolution_clock::now();
        
        // 性能统计
        auto total_render_ms = std::chrono::duration_cast<std::chrono::milliseconds>(render_time - start_time).count();
        auto total_save_ms = std::chrono::duration_cast<std::chrono::milliseconds>(save_time - render_time).count();
        
        std::cout << "\n=== 性能报告 ===" << std::endl;
        std::cout << "⏱️  渲染耗时: " << total_render_ms << " ms" << std::endl;
        std::cout << "💾 保存耗时: " << total_save_ms << " ms" << std::endl;
        std::cout << "🚀 总耗时: " << (total_render_ms + total_save_ms) << " ms" << std::endl;
        std::cout << "📊 渲染速度: " << (params.width * params.height * 1000.0 / total_render_ms) << " 像素/秒" << std::endl;
        
        if (mode == RenderMode::OPENMP) {
            #ifdef OPENMP_VERSION
            std::cout << "🧵 使用线程: " << omp_get_max_threads() << " 个" << std::endl;
            #endif
        } else if (mode == RenderMode::CUDA) {
            std::cout << "🎮 使用GPU设备: " << device_id << std::endl;
        }
        
        std::cout << "\n✅ 渲染完成!" << std::endl;
        std::cout << "💡 提示: 使用 'convert " << output_filename << " output.png' 转换为PNG格式" << std::endl;
        
        if (mode == RenderMode::CPU) {
            std::cout << "🔮 建议: 使用OpenMP版本 (mandelbrot_omp) 获得更高性能!" << std::endl;
        } else if (mode == RenderMode::OPENMP) {
            std::cout << "🎯 下一步: 实现CUDA版本获得更大性能突破!" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] 渲染失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
