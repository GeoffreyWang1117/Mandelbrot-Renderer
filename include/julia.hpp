/**
 * Julia Set 分形渲染器
 * 
 * Julia集是与Mandelbrot集密切相关的分形，通过固定参数c来生成不同的分形图案
 * 
 * 作者: Geoffrey Wang (with Claude AI assistance)
 * 日期: 2025年8月13日
 */

#ifndef JULIA_HPP
#define JULIA_HPP

#include <vector>
#include <string>

namespace fractal {

/**
 * Julia集分形参数结构
 */
struct JuliaParams {
    // 复数参数 c = cx + i*cy
    double cx = -0.7269;  // 默认参数：经典Julia集
    double cy = 0.1889;
    
    // 渲染参数
    int width = 800;
    int height = 600;
    int max_iterations = 1000;
    
    // 复平面范围
    double x_min = -2.0;
    double x_max = 2.0;
    double y_min = -1.5;
    double y_max = 1.5;
    
    // 输出文件
    std::string output_file = "julia.ppm";
};

/**
 * 预定义的经典Julia集参数
 */
namespace julia_presets {
    const JuliaParams CLASSIC = {-0.7269, 0.1889, 800, 600, 1000, -2.0, 2.0, -1.5, 1.5, "julia_classic.ppm"};
    const JuliaParams DRAGON = {-0.8, 0.156, 800, 600, 1000, -2.0, 2.0, -1.5, 1.5, "julia_dragon.ppm"};
    const JuliaParams SPIRAL = {-0.75, 0.11, 800, 600, 1000, -2.0, 2.0, -1.5, 1.5, "julia_spiral.ppm"};
    const JuliaParams DENDRITE = {-0.235125, 0.827215, 800, 600, 1000, -2.0, 2.0, -1.5, 1.5, "julia_dendrite.ppm"};
}

/**
 * Julia集CPU单线程渲染器
 */
class JuliaRenderer {
public:
    /**
     * 渲染Julia集分形
     * @param params Julia集参数
     * @return 渲染用时（毫秒）
     */
    static double render(const JuliaParams& params);
    
    /**
     * 计算单个点的Julia集迭代次数
     * @param x 实部坐标
     * @param y 虚部坐标
     * @param cx Julia集参数实部
     * @param cy Julia集参数虚部
     * @param max_iter 最大迭代次数
     * @return 迭代次数
     */
    static int julia_iterations(double x, double y, double cx, double cy, int max_iter);
    
    /**
     * 保存PPM格式图像
     * @param data 图像数据
     * @param width 图像宽度
     * @param height 图像高度
     * @param filename 文件名
     */
    static void save_ppm(const std::vector<int>& data, int width, int height, const std::string& filename);
    
    /**
     * 根据迭代次数计算HSV颜色
     * @param iterations 迭代次数
     * @param max_iterations 最大迭代次数
     * @param r 红色分量输出
     * @param g 绿色分量输出
     * @param b 蓝色分量输出
     */
    static void iterations_to_color(int iterations, int max_iterations, int& r, int& g, int& b);
};

/**
 * Julia集OpenMP多线程渲染器
 */
class JuliaRendererOMP {
public:
    static double render(const JuliaParams& params);
    static void set_thread_count(int threads);
private:
    static int thread_count;
};

} // namespace fractal

#endif // JULIA_HPP
