/**
 * Mandelbrot 分形渲染器 - CUDA GPU实现
 * 
 * 本文件实现了基于CUDA的GPU并行渲染
 * GPU优化策略:
 * 1. 每个线程计算一个像素
 * 2. 线程块大小优化 (16x16 = 256线程)
 * 3. 内存合并访问模式
 * 4. 避免分支发散
 * 5. 寄存器使用优化
 * 
 * 硬件目标: RTX 3090 (8704 CUDA Cores, 24GB VRAM)
 * 
 * 作者: Geoffrey Wang (with Claude AI assistance)
 * 日期: 2025-08-12
 */

#include "../include/render_cuda.hpp"
#include <iostream>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <vector>
#include <stdexcept>

namespace MandelbrotCUDA {

    /**
     * CUDA Device函数: 优化的Mandelbrot迭代计算
     * 运行在GPU上，每个线程调用一次
     */
    __device__ int mandelbrot_iterations_cuda(double cx, double cy, int max_iter) {
        double zx = 0.0, zy = 0.0;
        double zx2, zy2;
        int iter = 0;
        
        // 优化的迭代循环，减少浮点运算
        while (iter < max_iter) {
            zx2 = zx * zx;
            zy2 = zy * zy;
            
            // 早期退出条件: |z|^2 > 4
            if (zx2 + zy2 > 4.0) break;
            
            // z = z^2 + c
            zy = 2.0 * zx * zy + cy;
            zx = zx2 - zy2 + cx;
            ++iter;
        }
        
        return iter;
    }

    /**
     * CUDA Device函数: 迭代次数转RGB颜色
     * 使用快速HSV到RGB转换
     */
    __device__ void iterations_to_rgb_cuda(int iterations, int max_iter, 
                                          unsigned char* r, unsigned char* g, unsigned char* b) {
        if (iterations == max_iter) {
            // 属于Mandelbrot集合内部
            *r = *g = *b = 0;
            return;
        }
        
        // 归一化迭代值
        double t = (double)iterations / max_iter;
        
        // 彩虹色谱映射 (优化版本)
        int color_r, color_g, color_b;
        
        if (t < 0.16) {
            color_r = 255;
            color_g = (int)(255 * t / 0.16);
            color_b = 0;
        } else if (t < 0.33) {
            color_r = (int)(255 * (0.33 - t) / 0.17);
            color_g = 255;
            color_b = 0;
        } else if (t < 0.5) {
            color_r = 0;
            color_g = 255;
            color_b = (int)(255 * (t - 0.33) / 0.17);
        } else if (t < 0.66) {
            color_r = 0;
            color_g = (int)(255 * (0.66 - t) / 0.16);
            color_b = 255;
        } else if (t < 0.83) {
            color_r = (int)(255 * (t - 0.66) / 0.17);
            color_g = 0;
            color_b = 255;
        } else {
            color_r = 255;
            color_g = 0;
            color_b = (int)(255 * (1.0 - t) / 0.17);
        }
        
        *r = (unsigned char)color_r;
        *g = (unsigned char)color_g;
        *b = (unsigned char)color_b;
    }

    /**
     * CUDA Kernel: 并行Mandelbrot渲染
     * 每个线程处理一个像素
     */
    __global__ void mandelbrot_kernel(unsigned char* output,
                                     int width, int height,
                                     double x_min, double x_max,
                                     double y_min, double y_max,
                                     int max_iter) {
        // 计算当前线程对应的像素坐标
        int px = blockIdx.x * blockDim.x + threadIdx.x;
        int py = blockIdx.y * blockDim.y + threadIdx.y;
        
        // 边界检查
        if (px >= width || py >= height) return;
        
        // 像素坐标到复平面坐标的映射
        double cx = x_min + (x_max - x_min) * px / (width - 1);
        double cy = y_min + (y_max - y_min) * py / (height - 1);
        
        // 计算迭代次数
        int iterations = mandelbrot_iterations_cuda(cx, cy, max_iter);
        
        // 转换为RGB颜色
        unsigned char r, g, b;
        iterations_to_rgb_cuda(iterations, max_iter, &r, &g, &b);
        
        // 存储结果 (行主序存储，RGB交错)
        int pixel_index = (py * width + px) * 3;
        output[pixel_index] = r;     // Red
        output[pixel_index + 1] = g; // Green
        output[pixel_index + 2] = b; // Blue
    }

    // C接口包装函数
    extern "C" void mandelbrot_kernel_launcher(unsigned char* output,
                                              int width, int height,
                                              double x_min, double x_max,
                                              double y_min, double y_max,
                                              int max_iter,
                                              dim3 grid_size, dim3 block_size) {
        mandelbrot_kernel<<<grid_size, block_size>>>(output, width, height,
                                                     x_min, x_max, y_min, y_max, max_iter);
    }

    std::vector<GPUInfo> get_available_gpus() {
        std::vector<GPUInfo> gpus;
        int device_count;
        
        CUDA_CHECK(cudaGetDeviceCount(&device_count));
        
        for (int i = 0; i < device_count; ++i) {
            cudaDeviceProp prop;
            CUDA_CHECK(cudaGetDeviceProperties(&prop, i));
            
            // 获取内存信息
            size_t free_mem, total_mem;
            cudaSetDevice(i);
            CUDA_CHECK(cudaMemGetInfo(&free_mem, &total_mem));
            
            GPUInfo info;
            info.device_id = i;
            info.name = prop.name;
            info.total_memory = total_mem / (1024 * 1024);  // MB
            info.free_memory = free_mem / (1024 * 1024);    // MB
            info.compute_capability_major = prop.major;
            info.compute_capability_minor = prop.minor;
            info.multiprocessor_count = prop.multiProcessorCount;
            info.max_threads_per_block = prop.maxThreadsPerBlock;
            info.max_threads_per_multiprocessor = prop.maxThreadsPerMultiProcessor;
            info.warp_size = prop.warpSize;
            
            gpus.push_back(info);
        }
        
        return gpus;
    }

    int get_best_gpu_device() {
        auto gpus = get_available_gpus();
        if (gpus.empty()) {
            throw std::runtime_error("没有找到可用的GPU设备");
        }
        
        // 选择计算能力最高的GPU
        int best_device = 0;
        int best_score = 0;
        
        for (const auto& gpu : gpus) {
            int score = gpu.compute_capability_major * 100 + gpu.compute_capability_minor * 10 + 
                       (gpu.multiprocessor_count / 10);
            if (score > best_score) {
                best_score = score;
                best_device = gpu.device_id;
            }
        }
        
        return best_device;
    }

    void initialize_cuda(int device_id) {
        int device_count;
        CUDA_CHECK(cudaGetDeviceCount(&device_count));
        
        if (device_id >= device_count) {
            throw std::runtime_error("无效的GPU设备ID: " + std::to_string(device_id));
        }
        
        CUDA_CHECK(cudaSetDevice(device_id));
        
        // 预热GPU
        CUDA_CHECK(cudaFree(0));
        
        std::cout << "[CUDA] 初始化设备 " << device_id << " 完成" << std::endl;
    }

    void cleanup_cuda() {
        CUDA_CHECK(cudaDeviceReset());
    }

    std::pair<size_t, size_t> get_gpu_memory_info(int device_id) {
        cudaSetDevice(device_id);
        
        size_t free_mem, total_mem;
        CUDA_CHECK(cudaMemGetInfo(&free_mem, &total_mem));
        
        return {(total_mem - free_mem) / (1024 * 1024), total_mem / (1024 * 1024)};
    }

    std::pair<dim3, dim3> calculate_optimal_block_config(int width, int height) {
        // 根据图像尺寸选择最优的线程块大小
        dim3 block_size;
        
        if (width >= 1024 && height >= 1024) {
            block_size = dim3(32, 32);  // 1024线程/块，适用于大图像
        } else if (width >= 512 && height >= 512) {
            block_size = dim3(16, 16);  // 256线程/块，适用于中等图像
        } else {
            block_size = dim3(8, 8);    // 64线程/块，适用于小图像
        }
        
        // 计算网格大小
        dim3 grid_size((width + block_size.x - 1) / block_size.x,
                       (height + block_size.y - 1) / block_size.y);
        
        return {block_size, grid_size};
    }

    std::string get_cuda_info() {
        std::ostringstream info;
        
        int runtime_version, driver_version;
        CUDA_CHECK(cudaRuntimeGetVersion(&runtime_version));
        CUDA_CHECK(cudaDriverGetVersion(&driver_version));
        
        info << "CUDA Runtime版本: " << runtime_version << std::endl;
        info << "CUDA Driver版本: " << driver_version << std::endl;
        
        auto gpus = get_available_gpus();
        info << "可用GPU数量: " << gpus.size() << std::endl;
        
        for (const auto& gpu : gpus) {
            info << "设备 " << gpu.device_id << ": " << gpu.name << std::endl;
            info << "  计算能力: " << gpu.compute_capability_major << "." << gpu.compute_capability_minor << std::endl;
            info << "  多处理器: " << gpu.multiprocessor_count << " 个" << std::endl;
            info << "  总内存: " << gpu.total_memory << " MB" << std::endl;
            info << "  可用内存: " << gpu.free_memory << " MB" << std::endl;
        }
        
        return info.str();
    }

    std::vector<unsigned char> render_mandelbrot_cuda(const RenderParams& params, 
                                                      int device_id, 
                                                      int block_size) {
        std::cout << "[CUDA] 开始GPU渲染 Mandelbrot 集合..." << std::endl;
        std::cout << "[CUDA] 分辨率: " << params.width << "x" << params.height << std::endl;
        std::cout << "[CUDA] 最大迭代: " << params.max_iter << std::endl;
        std::cout << "[CUDA] 复平面范围: [" << params.x_min << "," << params.x_max 
                  << "] x [" << params.y_min << "," << params.y_max << "]" << std::endl;
        
        // 初始化CUDA
        initialize_cuda(device_id);
        
        // 显示GPU信息
        auto gpus = get_available_gpus();
        if (device_id < gpus.size()) {
            const auto& gpu = gpus[device_id];
            std::cout << "[CUDA] 使用GPU: " << gpu.name 
                      << " (计算能力 " << gpu.compute_capability_major << "." << gpu.compute_capability_minor << ")"
                      << std::endl;
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // 分配主机内存
        int total_pixels = params.width * params.height;
        size_t image_size = total_pixels * 3 * sizeof(unsigned char);
        std::vector<unsigned char> host_image(total_pixels * 3);
        
        // 分配GPU内存
        unsigned char* device_image;
        CUDA_CHECK(cudaMalloc(&device_image, image_size));
        
        // 计算最优线程配置
        auto [block_dim, grid_dim] = calculate_optimal_block_config(params.width, params.height);
        
        std::cout << "[CUDA] 线程配置: " << block_dim.x << "x" << block_dim.y << " 块大小, "
                  << grid_dim.x << "x" << grid_dim.y << " 网格大小" << std::endl;
        std::cout << "[CUDA] 总线程数: " << block_dim.x * block_dim.y * grid_dim.x * grid_dim.y << std::endl;
        
        // 启动CUDA kernel
        mandelbrot_kernel<<<grid_dim, block_dim>>>(device_image,
                                                   params.width, params.height,
                                                   params.x_min, params.x_max,
                                                   params.y_min, params.y_max,
                                                   params.max_iter);
        
        // 检查kernel执行是否成功
        CUDA_CHECK(cudaGetLastError());
        CUDA_CHECK(cudaDeviceSynchronize());
        
        // 将结果从GPU复制到CPU
        CUDA_CHECK(cudaMemcpy(host_image.data(), device_image, image_size, cudaMemcpyDeviceToHost));
        
        // 清理GPU内存
        CUDA_CHECK(cudaFree(device_image));
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        std::cout << "[CUDA] 渲染完成! 耗时: " << duration.count() / 1000.0 << " ms" << std::endl;
        std::cout << "[CUDA] 性能: " << (total_pixels * 1000000.0 / duration.count()) << " 像素/秒" << std::endl;
        
        // 显示内存使用情况
        auto [used_mem, total_mem] = get_gpu_memory_info(device_id);
        std::cout << "[CUDA] GPU内存使用: " << used_mem << " / " << total_mem << " MB" << std::endl;
        
        return host_image;
    }

} // namespace MandelbrotCUDA
