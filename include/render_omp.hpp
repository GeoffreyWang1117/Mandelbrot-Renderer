#pragma once

#include "render.hpp"
#include <omp.h>

/**
 * Mandelbrot 分形渲染器 - OpenMP并行版本
 * 
 * 本文件定义了基于OpenMP的多线程并行渲染接口
 * 相比CPU单线程版本，预期性能提升:
 * - 4核CPU: 3-4x 加速
 * - 8核CPU: 6-7x 加速
 * - 16核CPU: 10-12x 加速
 * 
 * 特性:
 * - 自动线程数检测和配置
 * - 动态负载均衡优化
 * - 内存访问局部性优化
 * - 与CPU版本完全兼容的接口
 */

namespace MandelbrotOMP {

    // 继承CPU版本的参数结构体
    using RenderParams = MandelbrotCPU::RenderParams;
    using RGB = MandelbrotCPU::RGB;

    /**
     * OpenMP并行版本 - Mandelbrot集合渲染
     * @param params 渲染参数
     * @param num_threads 线程数 (0=自动检测)
     * @return RGB像素数据向量 (size = width * height * 3)
     */
    std::vector<unsigned char> render_mandelbrot_omp(const RenderParams& params, int num_threads = 0);

    /**
     * 获取系统最优线程数
     * @return 推荐的线程数
     */
    int get_optimal_thread_count();

    /**
     * 设置OpenMP线程数和调度策略
     * @param num_threads 线程数
     * @param chunk_size 块大小 (用于动态调度)
     */
    void configure_openmp(int num_threads = 0, int chunk_size = 1);

    /**
     * OpenMP并行版本的迭代计算 (内联优化)
     * @param real 实部坐标
     * @param imag 虚部坐标  
     * @param max_iter 最大迭代次数
     * @return 迭代次数
     */
    inline int mandelbrot_iterations_omp(double real, double imag, int max_iter) {
        double zr = 0.0, zi = 0.0;
        double zr2, zi2;
        int iter = 0;
        
        // 使用优化的迭代计算避免复数库开销
        while (iter < max_iter) {
            zr2 = zr * zr;
            zi2 = zi * zi;
            
            if (zr2 + zi2 > 4.0) break;  // |z|^2 > 4 等价于 |z| > 2
            
            zi = 2.0 * zr * zi + imag;
            zr = zr2 - zi2 + real;
            ++iter;
        }
        
        return iter;
    }

    /**
     * 获取OpenMP版本信息和配置
     * @return 版本和线程信息字符串
     */
    std::string get_openmp_info();

} // namespace MandelbrotOMP
