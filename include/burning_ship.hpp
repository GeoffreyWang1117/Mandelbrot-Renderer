#pragma once
#include <complex>
#include <vector>
#include <string>
#include <chrono>

/**
 * Burning Ship Fractal Renderer
 * 
 * The Burning Ship fractal is a variation of the Mandelbrot set that uses
 * absolute values in the iteration formula: z = (|Re(z)| + i|Im(z)|)² + c
 * 
 * This creates a distinctive "ship-like" structure with intricate details
 * and beautiful color gradients.
 * 
 * Mathematical Formula:
 * - z_{n+1} = (|Re(z_n)| + i|Im(z_n)|)² + c
 * - where z_0 = 0 and c is the complex coordinate
 * 
 * Author: Geoffrey Wang (with Claude AI assistance)
 * Date: August 26, 2025
 */

class BurningShipCPU {
public:
    // Constructor with default parameters
    BurningShipCPU(int width = 800, int height = 600, int max_iterations = 1000);
    
    // Core rendering methods
    void render(double center_x = -0.5, double center_y = -0.5, double zoom = 1.0);
    void renderToFile(const std::string& filename, double center_x = -0.5, 
                     double center_y = -0.5, double zoom = 1.0);
    
    // Burning Ship computation
    int computeBurningShip(double cx, double cy) const;
    
    // Color mapping
    std::vector<uint8_t> iterationsToRGB(int iterations) const;
    void saveAsPPM(const std::string& filename) const;
    
    // Getters
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    int getMaxIterations() const { return max_iterations_; }
    
    // Preset configurations
    static std::vector<std::tuple<std::string, double, double, double>> getPresets();
    
private:
    int width_;
    int height_;
    int max_iterations_;
    std::vector<std::vector<int>> fractal_data_;
    
    // HSV to RGB conversion for smooth coloring
    std::vector<uint8_t> hsvToRgb(double h, double s, double v) const;
};

// Preset configurations for interesting Burning Ship regions
struct BurningShipPresets {
    static constexpr double CLASSIC_X = -0.5;
    static constexpr double CLASSIC_Y = -0.5;
    static constexpr double CLASSIC_ZOOM = 1.0;
    
    static constexpr double SHIP_DETAIL_X = -1.7269;
    static constexpr double SHIP_DETAIL_Y = -0.0311;
    static constexpr double SHIP_DETAIL_ZOOM = 100.0;
    
    static constexpr double LIGHTNING_X = -1.775;
    static constexpr double LIGHTNING_Y = -0.01;
    static constexpr double LIGHTNING_ZOOM = 500.0;
    
    static constexpr double ANTENNA_X = -1.7795;
    static constexpr double ANTENNA_Y = -0.0045;
    static constexpr double ANTENNA_ZOOM = 2000.0;
};
