/**
 * Mandelbrot 分形渲染器 - OpenMP并行实现
 * 
 * 本文件实现了基于OpenMP的多线程并行渲染
 * 主要优化策略:
 * 1. 像素级并行化 (#pragma omp parallel for)
 * 2. 动态负载均衡 (schedule(dynamic))
 * 3. 内存访问优化 (避免false sharing)
 * 4. 自适应线程数配置
 * 
 * 作者: Geoffrey Wang (with Claude AI assistance)
 * 日期: 2025-08-12
 */

#include "../include/render_omp.hpp"
#include <iostream>
#include <chrono>
#include <sstream>
#include <algorithm>

namespace MandelbrotOMP {

    int get_optimal_thread_count() {
        int hw_threads = omp_get_max_threads();
        int recommended = std::min(hw_threads, 16);  // 避免过度并行化
        
        return recommended;
    }

    void configure_openmp(int num_threads, int chunk_size) {
        if (num_threads <= 0) {
            num_threads = get_optimal_thread_count();
        }
        
        omp_set_num_threads(num_threads);
        
        // 设置动态调度的块大小
        if (chunk_size > 0) {
            // OpenMP环境变量方式设置 (如果支持)
            char chunk_env[32];
            snprintf(chunk_env, sizeof(chunk_env), "%d", chunk_size);
            // setenv("OMP_SCHEDULE", "dynamic,chunk", 1);  // 可选
        }
        
        std::cout << "[OpenMP] 配置完成: " << num_threads << " 线程" << std::endl;
    }

    std::string get_openmp_info() {
        std::ostringstream info;
        
        info << "OpenMP版本: " << _OPENMP << std::endl;
        info << "最大线程数: " << omp_get_max_threads() << std::endl;
        info << "当前线程数: " << omp_get_num_threads() << std::endl;
        info << "处理器数量: " << omp_get_num_procs() << std::endl;
        
        return info.str();
    }

    std::vector<unsigned char> render_mandelbrot_omp(const RenderParams& params, int num_threads) {
        std::cout << "[OpenMP] 开始并行渲染 Mandelbrot 集合..." << std::endl;
        std::cout << "[OpenMP] 分辨率: " << params.width << "x" << params.height << std::endl;
        std::cout << "[OpenMP] 最大迭代: " << params.max_iter << std::endl;
        std::cout << "[OpenMP] 复平面范围: [" << params.x_min << "," << params.x_max 
                  << "] x [" << params.y_min << "," << params.y_max << "]" << std::endl;
        
        // 配置OpenMP
        configure_openmp(num_threads);
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        int total_pixels = params.width * params.height;
        std::vector<unsigned char> image_data(total_pixels * 3);
        
        // 预计算常量避免重复计算
        const double x_scale = (params.x_max - params.x_min) / (params.width - 1);
        const double y_scale = (params.y_max - params.y_min) / (params.height - 1);
        
        // 进度统计变量
        int progress_counter = 0;
        const int progress_step = params.height / 10;
        
        std::cout << "[OpenMP] 使用 " << omp_get_max_threads() << " 个线程并行渲染" << std::endl;
        
        // OpenMP并行化主循环
        #pragma omp parallel for schedule(dynamic, 1) shared(progress_counter)
        for (int py = 0; py < params.height; ++py) {
            // 计算当前行的Y坐标
            double imag = params.y_min + py * y_scale;
            
            for (int px = 0; px < params.width; ++px) {
                // 计算当前像素的X坐标
                double real = params.x_min + px * x_scale;
                
                // 计算该点的迭代次数 (使用优化版本)
                int iterations = mandelbrot_iterations_omp(real, imag, params.max_iter);
                
                // 转换为颜色 (复用CPU版本的着色函数)
                MandelbrotCPU::RGB color = MandelbrotCPU::iterations_to_color(iterations, params.max_iter);
                
                // 存储RGB数据
                int pixel_index = (py * params.width + px) * 3;
                image_data[pixel_index] = color.r;     // Red
                image_data[pixel_index + 1] = color.g; // Green  
                image_data[pixel_index + 2] = color.b; // Blue
            }
            
            // 线程安全的进度显示 (仅主线程更新)
            #pragma omp critical
            {
                ++progress_counter;
                if (progress_counter % progress_step == 0) {
                    int progress = (progress_counter * 100) / params.height;
                    std::cout << "[OpenMP] 渲染进度: " << progress << "% (线程 " 
                              << omp_get_thread_num() << ")" << std::endl;
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "[OpenMP] 渲染完成! 耗时: " << duration.count() << " ms" << std::endl;
        std::cout << "[OpenMP] 性能: " << (total_pixels * 1000.0 / duration.count()) << " 像素/秒" << std::endl;
        std::cout << "[OpenMP] 使用的线程数: " << omp_get_max_threads() << std::endl;
        
        return image_data;
    }

} // namespace MandelbrotOMP
