/**
 * Mandelbrot 分形渲染器 - CPU单线程实现
 * 
 * 这是基础的CPU版本实现，用于:
 * 1. 验证算法正确性
 * 2. 作为后续OpenMP/CUDA版本的基准对比
 * 3. 小规模图像的快速渲染
 * 
 * 作者: Geoffrey Wang (with Claude AI assistance)
 * 日期: 2025-08-12
 */

#include "../include/render.hpp"
#include <complex>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>

namespace MandelbrotCPU {

    int mandelbrot_iterations(double real, double imag, int max_iter) {
        std::complex<double> c(real, imag);
        std::complex<double> z(0.0, 0.0);
        
        int iterations = 0;
        while (std::abs(z) <= 2.0 && iterations < max_iter) {
            z = z * z + c;
            ++iterations;
        }
        
        return iterations;
    }

    RGB iterations_to_color(int iterations, int max_iter) {
        if (iterations == max_iter) {
            // 属于Mandelbrot集合内部，使用黑色
            return RGB(0, 0, 0);
        }
        
        // 使用HSV到RGB的彩色映射
        double t = static_cast<double>(iterations) / max_iter;
        
        // 创建彩虹色谱
        int r, g, b;
        if (t < 0.16) {
            r = 255;
            g = static_cast<int>(255 * t / 0.16);
            b = 0;
        } else if (t < 0.33) {
            r = static_cast<int>(255 * (0.33 - t) / 0.17);
            g = 255;
            b = 0;
        } else if (t < 0.5) {
            r = 0;
            g = 255;
            b = static_cast<int>(255 * (t - 0.33) / 0.17);
        } else if (t < 0.66) {
            r = 0;
            g = static_cast<int>(255 * (0.66 - t) / 0.16);
            b = 255;
        } else if (t < 0.83) {
            r = static_cast<int>(255 * (t - 0.66) / 0.17);
            g = 0;
            b = 255;
        } else {
            r = 255;
            g = 0;
            b = static_cast<int>(255 * (1.0 - t) / 0.17);
        }
        
        return RGB(static_cast<unsigned char>(r), 
                  static_cast<unsigned char>(g), 
                  static_cast<unsigned char>(b));
    }

    std::vector<unsigned char> render_mandelbrot_cpu(const RenderParams& params) {
        std::cout << "[CPU] 开始渲染 Mandelbrot 集合..." << std::endl;
        std::cout << "[CPU] 分辨率: " << params.width << "x" << params.height << std::endl;
        std::cout << "[CPU] 最大迭代: " << params.max_iter << std::endl;
        std::cout << "[CPU] 复平面范围: [" << params.x_min << "," << params.x_max 
                  << "] x [" << params.y_min << "," << params.y_max << "]" << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        int total_pixels = params.width * params.height;
        std::vector<unsigned char> image_data(total_pixels * 3);
        
        // CPU单线程渲染
        for (int py = 0; py < params.height; ++py) {
            for (int px = 0; px < params.width; ++px) {
                // 将像素坐标映射到复平面坐标
                double real = params.x_min + (params.x_max - params.x_min) * px / (params.width - 1);
                double imag = params.y_min + (params.y_max - params.y_min) * py / (params.height - 1);
                
                // 计算该点的迭代次数
                int iterations = mandelbrot_iterations(real, imag, params.max_iter);
                
                // 转换为颜色
                RGB color = iterations_to_color(iterations, params.max_iter);
                
                // 存储RGB数据
                int pixel_index = (py * params.width + px) * 3;
                image_data[pixel_index] = color.r;     // Red
                image_data[pixel_index + 1] = color.g; // Green  
                image_data[pixel_index + 2] = color.b; // Blue
            }
            
            // 显示进度
            if (py % (params.height / 10) == 0) {
                int progress = (py * 100) / params.height;
                std::cout << "[CPU] 渲染进度: " << progress << "%" << std::endl;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "[CPU] 渲染完成! 耗时: " << duration.count() << " ms" << std::endl;
        std::cout << "[CPU] 性能: " << (total_pixels * 1000.0 / duration.count()) << " 像素/秒" << std::endl;
        
        return image_data;
    }

    void save_ppm(const std::string& filename, 
                  const std::vector<unsigned char>& image_data,
                  int width, int height) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "[ERROR] 无法创建文件: " << filename << std::endl;
            return;
        }
        
        // PPM文件头
        file << "P6\n";
        file << width << " " << height << "\n";
        file << "255\n";
        
        // 写入像素数据
        file.write(reinterpret_cast<const char*>(image_data.data()), image_data.size());
        file.close();
        
        std::cout << "[CPU] 图像已保存: " << filename << std::endl;
    }

} // namespace MandelbrotCPU
