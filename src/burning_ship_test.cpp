#include "burning_ship.hpp"
#include <iostream>

int main() {
    std::cout << "\n=== 渲染 Burning Ship Fractal ===" << std::endl;
    
    // Create Burning Ship renderer
    BurningShipCPU renderer(800, 600, 1000);
    
    // Render classic view
    renderer.renderToFile("burning_ship_classic.ppm", 
                         BurningShipPresets::CLASSIC_X, 
                         BurningShipPresets::CLASSIC_Y, 
                         BurningShipPresets::CLASSIC_ZOOM);
    
    std::cout << "\n=== 渲染 Ship Detail View ===" << std::endl;
    
    // Render detailed ship view
    renderer.renderToFile("burning_ship_detail.ppm", 
                         BurningShipPresets::SHIP_DETAIL_X, 
                         BurningShipPresets::SHIP_DETAIL_Y, 
                         BurningShipPresets::SHIP_DETAIL_ZOOM);
    
    std::cout << "\n🔥 Burning Ship Fractal 演示完成！" << std::endl;
    std::cout << "📁 生成的文件:" << std::endl;
    std::cout << "  • burning_ship_classic.ppm - 经典全景视图" << std::endl;
    std::cout << "  • burning_ship_detail.ppm - 船体细节视图" << std::endl;
    std::cout << "\n💡 特点:" << std::endl;
    std::cout << "  • 使用绝对值变换: z = (|Re(z)| + i|Im(z)|)² + c" << std::endl;
    std::cout << "  • 创造独特的'燃烧船'形状" << std::endl;
    std::cout << "  • 增强的颜色映射模拟火焰效果" << std::endl;
    
    return 0;
}
