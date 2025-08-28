#include "newton_fractal.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <tuple>

NewtonFractalCPU::NewtonFractalCPU(int width, int height, int max_iterations)
    : width_(width), height_(height), max_iterations_(max_iterations) {
    fractal_data_.resize(height_, std::vector<std::pair<int, int>>(width_, {-1, 0}));
}

void NewtonFractalCPU::render(double center_x, double center_y, double zoom) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Calculate the complex plane bounds
    double scale = 4.0 / zoom;
    double min_x = center_x - scale / 2.0;
    double max_x = center_x + scale / 2.0;
    double min_y = center_y - scale / 2.0;
    double max_y = center_y + scale / 2.0;
    
    // Render each pixel
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            // Map pixel coordinates to complex plane
            double cx = min_x + (max_x - min_x) * x / (width_ - 1);
            double cy = min_y + (max_y - min_y) * y / (height_ - 1);
            
            // Compute Newton iterations for this point
            std::complex<double> z(cx, cy);
            int iterations = 0;
            
            // Newton's method iteration
            while (iterations < max_iterations_) {
                std::complex<double> z_old = z;
                z = newtonIteration(z);
                
                // Check for convergence
                if (std::abs(z - z_old) < CONVERGENCE_THRESHOLD) {
                    break;
                }
                iterations++;
            }
            
            // Identify which root we converged to
            int root = identifyRoot(z);
            fractal_data_[y][x] = {root, iterations};
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "Newton Fractal 渲染完成！" << std::endl;
    std::cout << "参数: center = " << center_x << " + " << center_y << "i" << std::endl;
    std::cout << "分辨率: " << width_ << "x" << height_ << std::endl;
    std::cout << "渲染时间: " << duration.count() << " ms" << std::endl;
    std::cout << "性能: " << (width_ * height_) / (duration.count() / 1000.0) << " 像素/秒" << std::endl;
}

void NewtonFractalCPU::renderToFile(const std::string& filename, double center_x, 
                                   double center_y, double zoom) {
    render(center_x, center_y, zoom);
    saveAsPPM(filename);
    std::cout << "输出文件: " << filename << std::endl;
}

std::complex<double> NewtonFractalCPU::newtonIteration(const std::complex<double>& z) const {
    // For f(z) = z³ - 1, f'(z) = 3z²
    // Newton iteration: z_{n+1} = z_n - f(z_n)/f'(z_n)
    // Simplified: z_{n+1} = (2z³ + 1) / (3z²)
    
    std::complex<double> z2 = z * z;
    std::complex<double> z3 = z2 * z;
    
    if (std::abs(z2) < 1e-10) {
        // Avoid division by zero
        return z;
    }
    
    return (2.0 * z3 + 1.0) / (3.0 * z2);
}

int NewtonFractalCPU::identifyRoot(const std::complex<double>& z) const {
    // Check convergence to each of the three cube roots of unity
    
    // Root 1: 1 + 0i
    if (std::abs(z - std::complex<double>(ROOT1_REAL, ROOT1_IMAG)) < CONVERGENCE_THRESHOLD) {
        return 1;
    }
    
    // Root 2: -0.5 + 0.866i
    if (std::abs(z - std::complex<double>(ROOT2_REAL, ROOT2_IMAG)) < CONVERGENCE_THRESHOLD) {
        return 2;
    }
    
    // Root 3: -0.5 - 0.866i
    if (std::abs(z - std::complex<double>(ROOT3_REAL, ROOT3_IMAG)) < CONVERGENCE_THRESHOLD) {
        return 3;
    }
    
    // No convergence or converged to unexpected value
    return 0;
}

std::vector<uint8_t> NewtonFractalCPU::rootToRGB(int root, int iterations) const {
    // Base colors for each root
    std::vector<uint8_t> base_color;
    
    switch (root) {
        case 1: // Root 1 -> Red spectrum
            base_color = {255, 50, 50};
            break;
        case 2: // Root 2 -> Green spectrum  
            base_color = {50, 255, 50};
            break;
        case 3: // Root 3 -> Blue spectrum
            base_color = {50, 50, 255};
            break;
        default: // No convergence -> Black
            return {0, 0, 0};
    }
    
    // Modulate brightness based on convergence speed
    double convergence_factor = static_cast<double>(max_iterations_ - iterations) / max_iterations_;
    convergence_factor = std::max(0.3, convergence_factor); // Minimum brightness
    
    return {
        static_cast<uint8_t>(base_color[0] * convergence_factor),
        static_cast<uint8_t>(base_color[1] * convergence_factor),
        static_cast<uint8_t>(base_color[2] * convergence_factor)
    };
}

std::vector<uint8_t> NewtonFractalCPU::interpolateColor(const std::vector<uint8_t>& color1, 
                                                       const std::vector<uint8_t>& color2, 
                                                       double t) const {
    return {
        static_cast<uint8_t>(color1[0] * (1 - t) + color2[0] * t),
        static_cast<uint8_t>(color1[1] * (1 - t) + color2[1] * t),
        static_cast<uint8_t>(color1[2] * (1 - t) + color2[2] * t)
    };
}

void NewtonFractalCPU::saveAsPPM(const std::string& filename) const {
    std::ofstream file(filename);
    file << "P3\n" << width_ << " " << height_ << "\n255\n";
    
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            auto [root, iterations] = fractal_data_[y][x];
            auto color = rootToRGB(root, iterations);
            file << static_cast<int>(color[0]) << " " 
                 << static_cast<int>(color[1]) << " " 
                 << static_cast<int>(color[2]) << " ";
        }
        file << "\n";
    }
}

std::vector<std::tuple<std::string, double, double, double>> NewtonFractalCPU::getPresets() {
    return {
        std::make_tuple("Classic View", NewtonPresets::CLASSIC_X, NewtonPresets::CLASSIC_Y, NewtonPresets::CLASSIC_ZOOM),
        std::make_tuple("Boundary Detail", NewtonPresets::BOUNDARY_X, NewtonPresets::BOUNDARY_Y, NewtonPresets::BOUNDARY_ZOOM),
        std::make_tuple("Root Detail", NewtonPresets::DETAIL_X, NewtonPresets::DETAIL_Y, NewtonPresets::DETAIL_ZOOM),
        std::make_tuple("Fractal Edge", NewtonPresets::FRACTAL_EDGE_X, NewtonPresets::FRACTAL_EDGE_Y, NewtonPresets::FRACTAL_EDGE_ZOOM)
    };
}
