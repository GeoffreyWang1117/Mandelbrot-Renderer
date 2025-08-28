#include <emscripten.h>
#include <emscripten/bind.h>
#include <complex>
#include <vector>
#include <cmath>

using namespace std;
using namespace emscripten;

// 复数类型定义
using Complex = complex<double>;

// 颜色结构体
struct Color {
    uint8_t r, g, b, a;
    Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255) 
        : r(r), g(g), b(b), a(a) {}
};

// HSV转RGB的辅助函数
Color hsvToRgb(double h, double s, double v) {
    double c = v * s;
    double x = c * (1 - abs(fmod(h / 60.0, 2) - 1));
    double m = v - c;
    
    double r, g, b;
    if (h >= 0 && h < 60) {
        r = c; g = x; b = 0;
    } else if (h >= 60 && h < 120) {
        r = x; g = c; b = 0;
    } else if (h >= 120 && h < 180) {
        r = 0; g = c; b = x;
    } else if (h >= 180 && h < 240) {
        r = 0; g = x; b = c;
    } else if (h >= 240 && h < 300) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }
    
    return Color(
        (uint8_t)((r + m) * 255),
        (uint8_t)((g + m) * 255),
        (uint8_t)((b + m) * 255)
    );
}

// Mandelbrot Set 计算
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int mandelbrotIterations(double real, double imag, int maxIter) {
        Complex c(real, imag);
        Complex z(0, 0);
        
        for (int i = 0; i < maxIter; i++) {
            if (abs(z) > 2.0) {
                return i;
            }
            z = z * z + c;
        }
        return maxIter;
    }
}

// Julia Set 计算
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int juliaIterations(double real, double imag, double cReal, double cImag, int maxIter) {
        Complex z(real, imag);
        Complex c(cReal, cImag);
        
        for (int i = 0; i < maxIter; i++) {
            if (abs(z) > 2.0) {
                return i;
            }
            z = z * z + c;
        }
        return maxIter;
    }
}

// Burning Ship 计算
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int burningShipIterations(double real, double imag, int maxIter) {
        Complex c(real, imag);
        Complex z(0, 0);
        
        for (int i = 0; i < maxIter; i++) {
            if (abs(z) > 2.0) {
                return i;
            }
            // Burning Ship 变换: z = (|Re(z)| + i|Im(z)|)² + c
            double zx = abs(z.real());
            double zy = abs(z.imag());
            z = Complex(zx * zx - zy * zy, 2 * zx * zy) + c;
        }
        return maxIter;
    }
}

// Newton Fractal 计算
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int newtonIterations(double real, double imag, int maxIter) {
        Complex z(real, imag);
        const double tolerance = 1e-6;
        
        // 三个立方根: 1, (-1+i√3)/2, (-1-i√3)/2
        Complex root1(1.0, 0.0);
        Complex root2(-0.5, sqrt(3.0)/2.0);
        Complex root3(-0.5, -sqrt(3.0)/2.0);
        
        for (int i = 0; i < maxIter; i++) {
            // Newton 迭代: z = z - f(z)/f'(z)
            // f(z) = z³ - 1, f'(z) = 3z²
            Complex z3 = z * z * z;
            Complex z2 = z * z;
            Complex numerator = z3 - Complex(1.0, 0.0);
            Complex denominator = 3.0 * z2;
            
            if (abs(denominator) < tolerance) break;
            
            z = z - numerator / denominator;
            
            // 检查收敛到哪个根
            if (abs(z - root1) < tolerance) return i + 1000;      // 红色根
            if (abs(z - root2) < tolerance) return i + 2000;      // 绿色根  
            if (abs(z - root3) < tolerance) return i + 3000;      // 蓝色根
        }
        return 0; // 不收敛
    }
}

// 高性能批量计算函数
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void computeFractalBatch(
        int fractalType, 
        double* realValues, 
        double* imagValues,
        double cReal,
        double cImag,
        int count,
        int maxIter,
        int* results
    ) {
        for (int i = 0; i < count; i++) {
            switch (fractalType) {
                case 0: // Mandelbrot
                    results[i] = mandelbrotIterations(realValues[i], imagValues[i], maxIter);
                    break;
                case 1: // Julia
                    results[i] = juliaIterations(realValues[i], imagValues[i], cReal, cImag, maxIter);
                    break;
                case 2: // Burning Ship
                    results[i] = burningShipIterations(realValues[i], imagValues[i], maxIter);
                    break;
                case 3: // Newton
                    results[i] = newtonIterations(realValues[i], imagValues[i], maxIter);
                    break;
                default:
                    results[i] = 0;
            }
        }
    }
}

// 完整图像渲染函数
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int testFunction() {
        return 42; // 简单测试函数
    }
    
    EMSCRIPTEN_KEEPALIVE
    void renderFractalImage(
        int fractalType,
        int width,
        int height,
        double centerX,
        double centerY,
        double zoom,
        double cReal,
        double cImag,
        int maxIter,
        uint8_t* imageData
    ) {
        // 参数验证
        if (!imageData || width <= 0 || height <= 0 || maxIter <= 0) {
            return;
        }
        
        if (zoom <= 0) zoom = 1.0;
        if (maxIter > 10000) maxIter = 1000; // 限制最大迭代次数
        
        double scale = 4.0 / zoom;
        double stepX = scale / width;
        double stepY = scale / height;
        double startX = centerX - scale / 2.0;
        double startY = centerY - scale / 2.0;
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                double real = startX + x * stepX;
                double imag = startY + y * stepY;
                
                int iterations;
                switch (fractalType) {
                    case 0: // Mandelbrot
                        iterations = mandelbrotIterations(real, imag, maxIter);
                        break;
                    case 1: // Julia
                        iterations = juliaIterations(real, imag, cReal, cImag, maxIter);
                        break;
                    case 2: // Burning Ship
                        iterations = burningShipIterations(real, imag, maxIter);
                        break;
                    case 3: // Newton
                        iterations = newtonIterations(real, imag, maxIter);
                        break;
                    default:
                        iterations = 0;
                }
                
                // 颜色映射
                Color color;
                if (fractalType == 3) { // Newton 特殊处理
                    if (iterations >= 3000) { // 蓝色根
                        int iter = iterations - 3000;
                        double intensity = 1.0 - (double)iter / maxIter;
                        color = Color(0, 0, (uint8_t)(255 * intensity));
                    } else if (iterations >= 2000) { // 绿色根
                        int iter = iterations - 2000;
                        double intensity = 1.0 - (double)iter / maxIter;
                        color = Color(0, (uint8_t)(255 * intensity), 0);
                    } else if (iterations >= 1000) { // 红色根
                        int iter = iterations - 1000;
                        double intensity = 1.0 - (double)iter / maxIter;
                        color = Color((uint8_t)(255 * intensity), 0, 0);
                    } else {
                        color = Color(0, 0, 0); // 黑色不收敛区域
                    }
                } else if (fractalType == 2) { // Burning Ship 火焰色
                    if (iterations == maxIter) {
                        color = Color(0, 0, 0);
                    } else {
                        double t = (double)iterations / maxIter;
                        double hue = 60.0 * (1.0 - t); // 从红色到黄色
                        double saturation = 1.0;
                        double value = sqrt(t);
                        color = hsvToRgb(hue, saturation, value);
                    }
                } else { // Mandelbrot/Julia 标准色
                    if (iterations == maxIter) {
                        color = Color(0, 0, 0);
                    } else {
                        double t = (double)iterations / maxIter;
                        double hue = 240.0 * (1.0 - t);
                        double saturation = 1.0;
                        double value = t;
                        color = hsvToRgb(hue, saturation, value);
                    }
                }
                
                // 写入图像数据 (RGBA 格式)
                int index = (y * width + x) * 4;
                imageData[index] = color.r;
                imageData[index + 1] = color.g;
                imageData[index + 2] = color.b;
                imageData[index + 3] = color.a;
            }
        }
    }
}

// Emscripten 绑定
EMSCRIPTEN_BINDINGS(fractals) {
    emscripten::function("mandelbrotIterations", &mandelbrotIterations);
    emscripten::function("juliaIterations", &juliaIterations);
    emscripten::function("burningShipIterations", &burningShipIterations);
    emscripten::function("newtonIterations", &newtonIterations);
    emscripten::function("computeFractalBatch", &computeFractalBatch, allow_raw_pointers());
    emscripten::function("renderFractalImage", &renderFractalImage, allow_raw_pointers());
}
