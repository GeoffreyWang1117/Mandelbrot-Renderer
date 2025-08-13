#pragma once

#include <vector>
#include <string>

/**
 * Mandelbrot 分形渲染器 - 头文件定义
 * 
 * 定义了基础渲染器的接口和数据结构
 * 支持不同的实现方式 (CPU/OpenMP/CUDA)
 * 
 * 作者: Geoffrey Wang (with Claude AI assistance)
 * 日期: 2025-08-12
 */

#ifndef RENDER_HPP

namespace MandelbrotCPU {

    // 渲染参数结构体
    struct RenderParams {
        int width;          // 图像宽度
        int height;         // 图像高度
        int max_iter;       // 最大迭代次数
        double x_min;       // 复平面X轴最小值
        double x_max;       // 复平面X轴最大值
        double y_min;       // 复平面Y轴最小值
        double y_max;       // 复平面Y轴最大值
        
        // 默认构造函数 - 经典Mandelbrot视图
        RenderParams(int w = 800, int h = 600, int iter = 1000,
                    double xmin = -2.0, double xmax = 1.0,
                    double ymin = -1.2, double ymax = 1.2)
            : width(w), height(h), max_iter(iter),
              x_min(xmin), x_max(xmax), y_min(ymin), y_max(ymax) {}
    };

    // RGB颜色结构体
    struct RGB {
        unsigned char r, g, b;
        RGB(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0)
            : r(red), g(green), b(blue) {}
    };

    /**
     * CPU单线程版本 - Mandelbrot集合渲染
     * @param params 渲染参数
     * @return RGB像素数据向量 (size = width * height * 3)
     */
    std::vector<unsigned char> render_mandelbrot_cpu(const RenderParams& params);

    /**
     * 将像素数据保存为PPM格式文件
     * @param filename 输出文件名
     * @param image_data RGB像素数据
     * @param width 图像宽度
     * @param height 图像高度
     */
    void save_ppm(const std::string& filename, 
                  const std::vector<unsigned char>& image_data,
                  int width, int height);

    /**
     * 计算单个像素的迭代次数
     * @param c 复数坐标
     * @param max_iter 最大迭代次数
     * @return 迭代次数
     */
    int mandelbrot_iterations(double real, double imag, int max_iter);

    /**
     * 将迭代次数转换为颜色
     * @param iterations 迭代次数
     * @param max_iter 最大迭代次数
     * @return RGB颜色
     */
    RGB iterations_to_color(int iterations, int max_iter);

} // namespace MandelbrotCPU
