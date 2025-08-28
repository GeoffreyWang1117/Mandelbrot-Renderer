/**
 * Julia Set 分形渲染器 - 测试程序
 * 
 * 演示不同的Julia集变体和性能对比
 * 
 * 作者: Geoffrey Wang (with Claude AI assistance)
 * 日期: 2025年8月13日
 */

#include "julia.hpp"
#include <iostream>
#include <vector>

using namespace fractal;

void print_usage(const char* program_name) {
    std::cout << "Julia Set 分形渲染器" << std::endl;
    std::cout << "用法: " << program_name << " [选项]" << std::endl;
    std::cout << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  -p <preset>    预设参数 (classic, dragon, spiral, dendrite)" << std::endl;
    std::cout << "  -c <cx> <cy>   自定义Julia集参数" << std::endl;
    std::cout << "  -s <w>x<h>     图像尺寸 (默认: 800x600)" << std::endl;
    std::cout << "  -i <iterations> 最大迭代次数 (默认: 1000)" << std::endl;
    std::cout << "  -o <filename>  输出文件名" << std::endl;
    std::cout << "  -t <threads>   OpenMP线程数 (默认: 8)" << std::endl;
    std::cout << "  --omp          使用OpenMP并行渲染" << std::endl;
    std::cout << "  --demo         演示所有预设参数" << std::endl;
    std::cout << "  -h, --help     显示此帮助信息" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  " << program_name << " -p classic" << std::endl;
    std::cout << "  " << program_name << " -c -0.8 0.156 -s 1920x1080" << std::endl;
    std::cout << "  " << program_name << " --demo" << std::endl;
}

void render_preset(const JuliaParams& preset, bool use_omp = false) {
    std::cout << "\n=== 渲染 Julia Set ===" << std::endl;
    
    if (use_omp) {
        JuliaRendererOMP::render(preset);
    } else {
        JuliaRenderer::render(preset);
    }
}

void demo_all_presets(bool use_omp = false) {
    std::cout << "\n🎨 Julia Set 演示 - 所有预设参数" << std::endl;
    std::cout << "================================================" << std::endl;
    
    render_preset(julia_presets::CLASSIC, use_omp);
    render_preset(julia_presets::DRAGON, use_omp);
    render_preset(julia_presets::SPIRAL, use_omp);
    render_preset(julia_presets::DENDRITE, use_omp);
    
    std::cout << "\n✅ 演示完成！生成的文件:" << std::endl;
    std::cout << "   - julia_classic.ppm" << std::endl;
    std::cout << "   - julia_dragon.ppm" << std::endl;
    std::cout << "   - julia_spiral.ppm" << std::endl;
    std::cout << "   - julia_dendrite.ppm" << std::endl;
    std::cout << "\n💡 提示: 使用 ImageMagick 转换为常见格式:" << std::endl;
    std::cout << "   convert julia_classic.ppm julia_classic.png" << std::endl;
}

int main(int argc, char* argv[]) {
    JuliaParams params = julia_presets::CLASSIC;
    bool use_omp = false;
    bool demo_mode = false;
    
    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            return 0;
        }
        else if (arg == "--demo") {
            demo_mode = true;
        }
        else if (arg == "--omp") {
            use_omp = true;
        }
        else if (arg == "-p" && i + 1 < argc) {
            std::string preset = argv[++i];
            if (preset == "classic") params = julia_presets::CLASSIC;
            else if (preset == "dragon") params = julia_presets::DRAGON;
            else if (preset == "spiral") params = julia_presets::SPIRAL;
            else if (preset == "dendrite") params = julia_presets::DENDRITE;
            else {
                std::cerr << "错误: 未知的预设参数 '" << preset << "'" << std::endl;
                return 1;
            }
        }
        else if (arg == "-c" && i + 2 < argc) {
            params.cx = std::stod(argv[++i]);
            params.cy = std::stod(argv[++i]);
        }
        else if (arg == "-s" && i + 1 < argc) {
            std::string size = argv[++i];
            size_t x_pos = size.find('x');
            if (x_pos != std::string::npos) {
                params.width = std::stoi(size.substr(0, x_pos));
                params.height = std::stoi(size.substr(x_pos + 1));
            }
        }
        else if (arg == "-i" && i + 1 < argc) {
            params.max_iterations = std::stoi(argv[++i]);
        }
        else if (arg == "-o" && i + 1 < argc) {
            params.output_file = argv[++i];
        }
        else if (arg == "-t" && i + 1 < argc) {
            int threads = std::stoi(argv[++i]);
            JuliaRendererOMP::set_thread_count(threads);
        }
    }
    
    if (demo_mode) {
        demo_all_presets(use_omp);
    } else {
        render_preset(params, use_omp);
    }
    
    return 0;
}
