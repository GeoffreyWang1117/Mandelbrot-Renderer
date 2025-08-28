#include "burning_ship.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <tuple>

BurningShipCPU::BurningShipCPU(int width, int height, int max_iterations)
    : width_(width), height_(height), max_iterations_(max_iterations) {
    fractal_data_.resize(height_, std::vector<int>(width_, 0));
}

void BurningShipCPU::render(double center_x, double center_y, double zoom) {
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
            
            // Compute Burning Ship iterations for this point
            fractal_data_[y][x] = computeBurningShip(cx, cy);
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "Burning Ship 渲染完成！" << std::endl;
    std::cout << "参数: center = " << center_x << " + " << center_y << "i" << std::endl;
    std::cout << "分辨率: " << width_ << "x" << height_ << std::endl;
    std::cout << "渲染时间: " << duration.count() << " ms" << std::endl;
    std::cout << "性能: " << (width_ * height_) / (duration.count() / 1000.0) << " 像素/秒" << std::endl;
}

void BurningShipCPU::renderToFile(const std::string& filename, double center_x, 
                                 double center_y, double zoom) {
    render(center_x, center_y, zoom);
    saveAsPPM(filename);
    std::cout << "输出文件: " << filename << std::endl;
}

int BurningShipCPU::computeBurningShip(double cx, double cy) const {
    double zx = 0.0, zy = 0.0;
    int iterations = 0;
    
    // Burning Ship iteration: z = (|Re(z)| + i|Im(z)|)² + c
    while (zx * zx + zy * zy < 4.0 && iterations < max_iterations_) {
        // Take absolute values before squaring
        double abs_zx = std::abs(zx);
        double abs_zy = std::abs(zy);
        
        // Compute (|Re(z)| + i|Im(z)|)²
        double new_zx = abs_zx * abs_zx - abs_zy * abs_zy + cx;
        double new_zy = 2 * abs_zx * abs_zy + cy;
        
        zx = new_zx;
        zy = new_zy;
        iterations++;
    }
    
    return iterations;
}

std::vector<uint8_t> BurningShipCPU::iterationsToRGB(int iterations) const {
    if (iterations == max_iterations_) {
        return {0, 0, 0}; // Black for points in the set
    }
    
    // Enhanced color mapping for Burning Ship
    double t = static_cast<double>(iterations) / max_iterations_;
    
    // Use multiple color regions for more dramatic effect
    double hue, saturation, value;
    
    if (t < 0.16) {
        // Deep blue to purple (ship interior)
        hue = 240 + t * 60 / 0.16;
        saturation = 1.0;
        value = 0.5 + t * 0.5 / 0.16;
    } else if (t < 0.42) {
        // Purple to red (ship body)
        hue = 300 + (t - 0.16) * 60 / 0.26;
        saturation = 1.0;
        value = 1.0;
    } else if (t < 0.6425) {
        // Red to orange (flames)
        hue = 0 + (t - 0.42) * 30 / 0.2225;
        saturation = 1.0;
        value = 1.0;
    } else if (t < 0.8575) {
        // Orange to yellow (bright flames)
        hue = 30 + (t - 0.6425) * 30 / 0.215;
        saturation = 1.0;
        value = 1.0;
    } else {
        // Yellow to white (hottest part)
        hue = 60;
        saturation = 1.0 - (t - 0.8575) / 0.1425;
        value = 1.0;
    }
    
    return hsvToRgb(hue, saturation, value);
}

std::vector<uint8_t> BurningShipCPU::hsvToRgb(double h, double s, double v) const {
    double c = v * s;
    double x = c * (1 - std::abs(std::fmod(h / 60.0, 2) - 1));
    double m = v - c;
    
    double r, g, b;
    if (h < 60) { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }
    
    return {
        static_cast<uint8_t>((r + m) * 255),
        static_cast<uint8_t>((g + m) * 255),
        static_cast<uint8_t>((b + m) * 255)
    };
}

void BurningShipCPU::saveAsPPM(const std::string& filename) const {
    std::ofstream file(filename);
    file << "P3\n" << width_ << " " << height_ << "\n255\n";
    
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            auto color = iterationsToRGB(fractal_data_[y][x]);
            file << static_cast<int>(color[0]) << " " 
                 << static_cast<int>(color[1]) << " " 
                 << static_cast<int>(color[2]) << " ";
        }
        file << "\n";
    }
}

std::vector<std::tuple<std::string, double, double, double>> BurningShipCPU::getPresets() {
    return {
        std::make_tuple("Classic View", BurningShipPresets::CLASSIC_X, BurningShipPresets::CLASSIC_Y, BurningShipPresets::CLASSIC_ZOOM),
        std::make_tuple("Ship Detail", BurningShipPresets::SHIP_DETAIL_X, BurningShipPresets::SHIP_DETAIL_Y, BurningShipPresets::SHIP_DETAIL_ZOOM),
        std::make_tuple("Lightning", BurningShipPresets::LIGHTNING_X, BurningShipPresets::LIGHTNING_Y, BurningShipPresets::LIGHTNING_ZOOM),
        std::make_tuple("Antenna", BurningShipPresets::ANTENNA_X, BurningShipPresets::ANTENNA_Y, BurningShipPresets::ANTENNA_ZOOM)
    };
}
