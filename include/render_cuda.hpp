#pragma once

#include "render.hpp"
#include <cuda_runtime.h>
#include <cuda.h>

/**
 * Mandelbrot 分形渲染器 - CUDA GPU版本
 * 
 * 本文件定义了基于CUDA的GPU并行渲染接口
 * 相比OpenMP版本，预期性能提升:
 * - RTX 3090: 5-10x 额外加速
 * - 双GPU: 潜在2x并行提升
 * - 目标: 4K分辨率实时渲染
 * 
 * GPU架构优化:
 * - 线程块大小优化 (16x16, 32x32)
 * - 共享内存利用
 * - 线程束(warp)效率
 * - 内存合并访问
 */

namespace MandelbrotCUDA {

    // 继承CPU版本的参数结构体
    using RenderParams = MandelbrotCPU::RenderParams;
    using RGB = MandelbrotCPU::RGB;

    // CUDA错误检查宏
    #define CUDA_CHECK(call) \
        do { \
            cudaError_t cuda_status = call; \
            if (cuda_status != cudaSuccess) { \
                throw std::runtime_error("CUDA错误: " + std::string(cudaGetErrorString(cuda_status))); \
            } \
        } while(0)

    // GPU设备信息结构体
    struct GPUInfo {
        int device_id;
        std::string name;
        size_t total_memory;
        size_t free_memory;
        int compute_capability_major;
        int compute_capability_minor;
        int multiprocessor_count;
        int max_threads_per_block;
        int max_threads_per_multiprocessor;
        int warp_size;
    };

    /**
     * CUDA GPU版本 - Mandelbrot集合渲染
     * @param params 渲染参数
     * @param device_id GPU设备ID (默认0)
     * @param block_size 线程块大小 (默认16，即16x16=256线程/块)
     * @return RGB像素数据向量 (size = width * height * 3)
     */
    std::vector<unsigned char> render_mandelbrot_cuda(const RenderParams& params, 
                                                      int device_id = 0, 
                                                      int block_size = 16);

    /**
     * 获取可用GPU设备列表
     * @return GPU设备信息向量
     */
    std::vector<GPUInfo> get_available_gpus();

    /**
     * 选择最优GPU设备
     * @return 推荐的设备ID
     */
    int get_best_gpu_device();

    /**
     * 初始化CUDA环境
     * @param device_id 要使用的GPU设备ID
     */
    void initialize_cuda(int device_id = 0);

    /**
     * 清理CUDA资源
     */
    void cleanup_cuda();

    /**
     * 获取GPU内存使用情况
     * @param device_id 设备ID
     * @return pair<已用内存MB, 总内存MB>
     */
    std::pair<size_t, size_t> get_gpu_memory_info(int device_id = 0);

    /**
     * 计算最优线程块配置
     * @param width 图像宽度
     * @param height 图像高度
     * @return pair<block_dim, grid_dim>
     */
    std::pair<dim3, dim3> calculate_optimal_block_config(int width, int height);

    /**
     * 获取CUDA版本和GPU信息
     * @return 详细信息字符串
     */
    std::string get_cuda_info();

    /**
     * 多GPU并行渲染 (实验性功能)
     * @param params 渲染参数
     * @param device_ids 要使用的GPU设备列表
     * @return RGB像素数据向量
     */
    std::vector<unsigned char> render_mandelbrot_multi_gpu(const RenderParams& params,
                                                           const std::vector<int>& device_ids);

} // namespace MandelbrotCUDA

// CUDA Kernel函数声明 (在.cu文件中实现)
extern "C" {
    /**
     * CUDA Kernel: 计算Mandelbrot像素值
     * @param output 输出图像数据 (device pointer)
     * @param width 图像宽度
     * @param height 图像高度
     * @param x_min 复平面X最小值
     * @param x_max 复平面X最大值
     * @param y_min 复平面Y最小值
     * @param y_max 复平面Y最大值
     * @param max_iter 最大迭代次数
     */
    void mandelbrot_kernel_launcher(unsigned char* output,
                                   int width, int height,
                                   double x_min, double x_max,
                                   double y_min, double y_max,
                                   int max_iter,
                                   dim3 grid_size, dim3 block_size);
}
