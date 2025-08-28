#pragma once
#include <complex>
#include <vector>
#include <string>
#include <chrono>

/**
 * Newton Fractal Renderer
 * 
 * Newton fractals are generated using Newton's method for finding roots of polynomials.
 * Each point in the complex plane represents a starting value for Newton's iteration,
 * and the color represents which root the iteration converges to.
 * 
 * This implementation uses the polynomial f(z) = z³ - 1, which has three roots:
 * - 1 (red)
 * - e^(2πi/3) ≈ -0.5 + 0.866i (green)  
 * - e^(4πi/3) ≈ -0.5 - 0.866i (blue)
 * 
 * Mathematical Formula:
 * - z_{n+1} = z_n - f(z_n)/f'(z_n)
 * - For f(z) = z³ - 1: z_{n+1} = z_n - (z_n³ - 1)/(3z_n²) = (2z_n³ + 1)/(3z_n²)
 * 
 * Author: Geoffrey Wang (with Claude AI assistance)
 * Date: August 26, 2025
 */

class NewtonFractalCPU {
public:
    // Constructor with default parameters
    NewtonFractalCPU(int width = 800, int height = 600, int max_iterations = 100);
    
    // Core rendering methods
    void render(double center_x = 0.0, double center_y = 0.0, double zoom = 1.0);
    void renderToFile(const std::string& filename, double center_x = 0.0, 
                     double center_y = 0.0, double zoom = 1.0);
    
    // Newton method computation
    int computeNewton(double cx, double cy) const;
    
    // Root identification
    int identifyRoot(const std::complex<double>& z) const;
    
    // Color mapping based on converged root
    std::vector<uint8_t> rootToRGB(int root, int iterations) const;
    void saveAsPPM(const std::string& filename) const;
    
    // Getters
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    int getMaxIterations() const { return max_iterations_; }
    
    // Preset configurations for interesting Newton fractal regions
    static std::vector<std::tuple<std::string, double, double, double>> getPresets();
    
private:
    int width_;
    int height_;
    int max_iterations_;
    std::vector<std::vector<std::pair<int, int>>> fractal_data_; // {root, iterations}
    
    // The three cube roots of unity
    static constexpr double ROOT1_REAL = 1.0;
    static constexpr double ROOT1_IMAG = 0.0;
    static constexpr double ROOT2_REAL = -0.5;
    static constexpr double ROOT2_IMAG = 0.866025403784;  // sqrt(3)/2
    static constexpr double ROOT3_REAL = -0.5;
    static constexpr double ROOT3_IMAG = -0.866025403784; // -sqrt(3)/2
    
    static constexpr double CONVERGENCE_THRESHOLD = 1e-6;
    
    // Newton iteration for z³ - 1 = 0
    std::complex<double> newtonIteration(const std::complex<double>& z) const;
    
    // Smooth color transitions
    std::vector<uint8_t> interpolateColor(const std::vector<uint8_t>& color1, 
                                         const std::vector<uint8_t>& color2, 
                                         double t) const;
};

// Preset configurations for interesting Newton fractal regions
struct NewtonPresets {
    static constexpr double CLASSIC_X = 0.0;
    static constexpr double CLASSIC_Y = 0.0;
    static constexpr double CLASSIC_ZOOM = 1.0;
    
    static constexpr double BOUNDARY_X = 0.0;
    static constexpr double BOUNDARY_Y = 0.0;
    static constexpr double BOUNDARY_ZOOM = 3.0;
    
    static constexpr double DETAIL_X = 0.5;
    static constexpr double DETAIL_Y = 0.866;
    static constexpr double DETAIL_ZOOM = 20.0;
    
    static constexpr double FRACTAL_EDGE_X = -0.2;
    static constexpr double FRACTAL_EDGE_Y = 0.3;
    static constexpr double FRACTAL_EDGE_ZOOM = 50.0;
};
