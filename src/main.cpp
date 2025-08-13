/**
 * Mandelbrot 分形渲染器 - 主程序
 * 
 * 高性能、可视化、可交互的 Mandelbrot 分形图像渲染器
 * 支持多种渲染模式:
 * - [x] CPU 单线程版本 (当前实现)
 * - [ ] OpenMP 并行版本 (待实现)
 * - [ ] CUDA GPU 版本 (待实现)
 * - [ ] OpenGL 实时渲染版本 (待实现)
 * 
 * 编译命令:
 * g++ -std=c++17 -O3 src/main.cpp src/render.cpp -I include -o mandelbrot_cpu
 * 
 * 使用示例:
 * ./mandelbrot_cpu --width 1920 --height 1080 --iter 2000 --output output/mandelbrot_hd.ppm
 * 
 * 作者: Geoffrey Wang (with Claude AI assistance)
 * 日期: 2025-08-12
 */

#include "../include/render.hpp"
#include <iostream>
#include <string>
#include <chrono>

void print_usage(const char* program_name) {
    std::cout << "\n=== Mandelbrot 分形渲染器 (CPU版本) ===" << std::endl;
    std::cout << "用法: " << program_name << " [选项]" << std::endl;
    std::cout << "\n选项:" << std::endl;
    std::cout << "  --width <w>     图像宽度 (默认: 800)" << std::endl;
    std::cout << "  --height <h>    图像高度 (默认: 600)" << std::endl;
    std::cout << "  --iter <n>      最大迭代次数 (默认: 1000)" << std::endl;
    std::cout << "  --xmin <x>      复平面X最小值 (默认: -2.0)" << std::endl;
    std::cout << "  --xmax <x>      复平面X最大值 (默认: 1.0)" << std::endl;
    std::cout << "  --ymin <y>      复平面Y最小值 (默认: -1.2)" << std::endl;
    std::cout << "  --ymax <y>      复平面Y最大值 (默认: 1.2)" << std::endl;
    std::cout << "  --output <file> 输出文件名 (默认: output/mandelbrot_cpu.ppm)" << std::endl;
    std::cout << "  --help          显示此帮助信息" << std::endl;
    std::cout << "\n示例:" << std::endl;
    std::cout << "  " << program_name << " --width 1920 --height 1080 --iter 2000" << std::endl;
    std::cout << "  " << program_name << " --xmin -0.8 --xmax -0.7 --ymin 0.0 --ymax 0.1" << std::endl;
    std::cout << "\n说明:" << std::endl;
    std::cout << "  - 生成的图像为PPM格式，可用ImageMagick转换:" << std::endl;
    std::cout << "    convert output/mandelbrot_cpu.ppm output/mandelbrot_cpu.png" << std::endl;
    std::cout << "  - 后续版本将支持OpenMP并行和CUDA GPU加速" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "\n🌀 Mandelbrot 分形渲染器启动..." << std::endl;
    std::cout << "📦 当前版本: CPU单线程实现" << std::endl;
    std::cout << "🚀 计划功能: OpenMP并行 + CUDA GPU + OpenGL可视化" << std::endl;
    
    // 默认渲染参数
    MandelbrotCPU::RenderParams params;
    std::string output_filename = "output/mandelbrot_cpu.ppm";
    
    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
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
        else {
            std::cerr << "未知参数: " << arg << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }
    
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
    
    try {
        // CPU版本渲染
        auto start_time = std::chrono::high_resolution_clock::now();
        auto image_data = MandelbrotCPU::render_mandelbrot_cpu(params);
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
        
        std::cout << "\n✅ 渲染完成!" << std::endl;
        std::cout << "💡 提示: 使用 'convert " << output_filename << " output.png' 转换为PNG格式" << std::endl;
        std::cout << "🔮 下一步: 实现OpenMP并行版本以获得更高性能!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] 渲染失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
