/**
 * Julia Set CPU实现
 * 
 * 作者: Geoffrey Wang (with Claude AI assistance)
 * 日期: 2025年8月13日
 */

#include "julia.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>
#include <iomanip>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace fractal {

int JuliaRendererOMP::thread_count = 8;

double JuliaRenderer::render(const JuliaParams& params) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<int> image_data(params.width * params.height);
    
    // 计算像素步长
    double dx = (params.x_max - params.x_min) / params.width;
    double dy = (params.y_max - params.y_min) / params.height;
    
    // 渲染每个像素
    for (int py = 0; py < params.height; ++py) {
        for (int px = 0; px < params.width; ++px) {
            // 将像素坐标转换为复数坐标
            double x = params.x_min + px * dx;
            double y = params.y_min + py * dy;
            
            // 计算Julia集迭代次数
            int iterations = julia_iterations(x, y, params.cx, params.cy, params.max_iterations);
            
            // 存储结果
            image_data[py * params.width + px] = iterations;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 保存图像
    save_ppm(image_data, params.width, params.height, params.output_file);
    
    // 输出性能统计
    int total_pixels = params.width * params.height;
    double pixels_per_second = total_pixels / (duration.count() / 1000.0);
    
    std::cout << "Julia Set 渲染完成！" << std::endl;
    std::cout << "参数: c = " << params.cx << " + " << params.cy << "i" << std::endl;
    std::cout << "分辨率: " << params.width << "x" << params.height << std::endl;
    std::cout << "渲染时间: " << duration.count() << " ms" << std::endl;
    std::cout << "性能: " << std::fixed << std::setprecision(0) << pixels_per_second << " 像素/秒" << std::endl;
    std::cout << "输出文件: " << params.output_file << std::endl;
    
    return duration.count();
}

int JuliaRenderer::julia_iterations(double x, double y, double cx, double cy, int max_iter) {
    double zx = x;
    double zy = y;
    
    for (int i = 0; i < max_iter; ++i) {
        // 计算 |z|²
        double zx2 = zx * zx;
        double zy2 = zy * zy;
        
        // 检查发散条件
        if (zx2 + zy2 > 4.0) {
            return i;
        }
        
        // Julia集迭代: z = z² + c
        double new_zx = zx2 - zy2 + cx;
        double new_zy = 2.0 * zx * zy + cy;
        
        zx = new_zx;
        zy = new_zy;
    }
    
    return max_iter;
}

void JuliaRenderer::save_ppm(const std::vector<int>& data, int width, int height, const std::string& filename) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "错误：无法创建文件 " << filename << std::endl;
        return;
    }
    
    // 写入PPM头
    file << "P3\n";
    file << width << " " << height << "\n";
    file << "255\n";
    
    // 写入像素数据
    for (int py = 0; py < height; ++py) {
        for (int px = 0; px < width; ++px) {
            int iterations = data[py * width + px];
            int r, g, b;
            iterations_to_color(iterations, 1000, r, g, b);  // 使用固定最大迭代次数进行归一化
            file << r << " " << g << " " << b << " ";
        }
        file << "\n";
    }
    
    file.close();
}

void JuliaRenderer::iterations_to_color(int iterations, int max_iterations, int& r, int& g, int& b) {
    if (iterations == max_iterations) {
        // 在集合内的点使用黑色
        r = g = b = 0;
        return;
    }
    
    // 使用HSV色彩空间生成平滑颜色
    double hue = 360.0 * iterations / max_iterations;
    double saturation = 1.0;
    double value = 1.0;
    
    // HSV到RGB转换
    int hi = static_cast<int>(hue / 60) % 6;
    double f = hue / 60 - hi;
    double p = value * (1 - saturation);
    double q = value * (1 - f * saturation);
    double t = value * (1 - (1 - f) * saturation);
    
    double red, green, blue;
    switch (hi) {
        case 0: red = value; green = t; blue = p; break;
        case 1: red = q; green = value; blue = p; break;
        case 2: red = p; green = value; blue = t; break;
        case 3: red = p; green = q; blue = value; break;
        case 4: red = t; green = p; blue = value; break;
        case 5: red = value; green = p; blue = q; break;
        default: red = green = blue = 0; break;
    }
    
    r = static_cast<int>(red * 255);
    g = static_cast<int>(green * 255);
    b = static_cast<int>(blue * 255);
}

// OpenMP版本实现
double JuliaRendererOMP::render(const JuliaParams& params) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<int> image_data(params.width * params.height);
    
    // 计算像素步长
    double dx = (params.x_max - params.x_min) / params.width;
    double dy = (params.y_max - params.y_min) / params.height;
    
#ifdef _OPENMP
    omp_set_num_threads(thread_count);
    
    // OpenMP并行渲染
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (int py = 0; py < params.height; ++py) {
        for (int px = 0; px < params.width; ++px) {
            // 将像素坐标转换为复数坐标
            double x = params.x_min + px * dx;
            double y = params.y_min + py * dy;
            
            // 计算Julia集迭代次数
            int iterations = JuliaRenderer::julia_iterations(x, y, params.cx, params.cy, params.max_iterations);
            
            // 存储结果
            image_data[py * params.width + px] = iterations;
        }
    }
#else
    // 回退到单线程版本
    return JuliaRenderer::render(params);
#endif
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 保存图像
    JuliaRenderer::save_ppm(image_data, params.width, params.height, params.output_file);
    
    // 输出性能统计
    int total_pixels = params.width * params.height;
    double pixels_per_second = total_pixels / (duration.count() / 1000.0);
    
    std::cout << "Julia Set OpenMP 渲染完成！" << std::endl;
    std::cout << "线程数: " << thread_count << std::endl;
    std::cout << "参数: c = " << params.cx << " + " << params.cy << "i" << std::endl;
    std::cout << "分辨率: " << params.width << "x" << params.height << std::endl;
    std::cout << "渲染时间: " << duration.count() << " ms" << std::endl;
    std::cout << "性能: " << std::fixed << std::setprecision(0) << pixels_per_second << " 像素/秒" << std::endl;
    std::cout << "输出文件: " << params.output_file << std::endl;
    
    return duration.count();
}

void JuliaRendererOMP::set_thread_count(int threads) {
    thread_count = threads;
}

} // namespace fractal
