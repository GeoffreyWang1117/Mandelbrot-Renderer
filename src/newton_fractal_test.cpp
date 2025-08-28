#include "newton_fractal.hpp"
#include <iostream>

int main() {
    std::cout << "\n=== 渲染 Newton Fractal ===" << std::endl;
    
    // Create Newton fractal renderer
    NewtonFractalCPU renderer(800, 600, 100);
    
    // Render classic view showing all three basins
    renderer.renderToFile("newton_classic.ppm", 
                         NewtonPresets::CLASSIC_X, 
                         NewtonPresets::CLASSIC_Y, 
                         NewtonPresets::CLASSIC_ZOOM);
    
    std::cout << "\n=== 渲染 Boundary Detail View ===" << std::endl;
    
    // Render detailed boundary view
    renderer.renderToFile("newton_boundary.ppm", 
                         NewtonPresets::BOUNDARY_X, 
                         NewtonPresets::BOUNDARY_Y, 
                         NewtonPresets::BOUNDARY_ZOOM);
    
    std::cout << "\n=== 渲染 Fractal Edge Detail ===" << std::endl;
    
    // Render fractal edge detail
    renderer.renderToFile("newton_edge.ppm", 
                         NewtonPresets::FRACTAL_EDGE_X, 
                         NewtonPresets::FRACTAL_EDGE_Y, 
                         NewtonPresets::FRACTAL_EDGE_ZOOM);
    
    std::cout << "\n🎯 Newton Fractal 演示完成！" << std::endl;
    std::cout << "📁 生成的文件:" << std::endl;
    std::cout << "  • newton_classic.ppm - 经典三色区域视图" << std::endl;
    std::cout << "  • newton_boundary.ppm - 边界细节视图" << std::endl;
    std::cout << "  • newton_edge.ppm - 分形边缘细节" << std::endl;
    std::cout << "\n💡 特点:" << std::endl;
    std::cout << "  • 使用牛顿法求解 z³ - 1 = 0" << std::endl;
    std::cout << "  • 三个收敛根对应红/绿/蓝颜色" << std::endl;
    std::cout << "  • 颜色亮度表示收敛速度" << std::endl;
    std::cout << "  • 展示复杂的分形边界结构" << std::endl;
    
    return 0;
}
