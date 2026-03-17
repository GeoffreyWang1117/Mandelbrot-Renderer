/**
 * Fractal Renderer - Server-side API binary
 *
 * Standalone program for server-side fractal rendering.
 * Outputs raw PPM image data to stdout for use with Node.js API server.
 *
 * Usage:
 *   ./fractal_api --fractal mandelbrot --width 1920 --height 1080 \
 *                 --cx -0.5 --cy 0.0 --zoom 1.0 --iter 1000
 *
 * Supported fractals: mandelbrot, julia, burning_ship, newton
 */

#include <iostream>
#include <string>
#include <cmath>
#include <complex>
#include <vector>
#include <cstdint>

using Complex = std::complex<double>;

struct RGB {
    uint8_t r, g, b;
    RGB(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0) : r(r), g(g), b(b) {}
};

struct RenderParams {
    std::string fractal = "mandelbrot";
    int width = 800;
    int height = 600;
    double cx = -0.5;
    double cy = 0.0;
    double zoom = 1.0;
    int maxIter = 1000;
    double juliaReal = -0.7269;
    double juliaImag = 0.1889;
    double phoenixPx = 0.5667;
    double phoenixPy = 0.0;
};

// --- Fractal computation functions ---

int mandelbrotIterations(double real, double imag, int maxIter) {
    // Cardioid check
    double cy2 = imag * imag;
    double q = (real - 0.25) * (real - 0.25) + cy2;
    if (q * (q + (real - 0.25)) <= 0.25 * cy2) return maxIter;
    // Period-2 bulb
    if ((real + 1.0) * (real + 1.0) + cy2 <= 0.0625) return maxIter;

    double zx = 0.0, zy = 0.0, zx2 = 0.0, zy2 = 0.0;
    for (int i = 0; i < maxIter; i++) {
        if (zx2 + zy2 > 4.0) return i;
        zy = 2.0 * zx * zy + imag;
        zx = zx2 - zy2 + real;
        zx2 = zx * zx;
        zy2 = zy * zy;
    }
    return maxIter;
}

int juliaIterations(double real, double imag, double cReal, double cImag, int maxIter) {
    double zx = real, zy = imag;
    for (int i = 0; i < maxIter; i++) {
        if (zx * zx + zy * zy > 4.0) return i;
        double tmp = zx * zx - zy * zy + cReal;
        zy = 2.0 * zx * zy + cImag;
        zx = tmp;
    }
    return maxIter;
}

int burningShipIterations(double real, double imag, int maxIter) {
    double zx = 0.0, zy = 0.0;
    for (int i = 0; i < maxIter; i++) {
        if (zx * zx + zy * zy > 4.0) return i;
        double ax = std::abs(zx), ay = std::abs(zy);
        double tmp = ax * ax - ay * ay + real;
        zy = 2.0 * ax * ay + imag;
        zx = tmp;
    }
    return maxIter;
}

int newtonIterations(double real, double imag, int maxIter) {
    double zx = real, zy = imag;
    const double tol = 1e-6;
    const double roots[][2] = {{1.0, 0.0}, {-0.5, 0.866025403784}, {-0.5, -0.866025403784}};

    for (int i = 0; i < maxIter; i++) {
        double z2x = zx * zx - zy * zy;
        double z2y = 2.0 * zx * zy;
        double z3x = z2x * zx - z2y * zy;
        double z3y = z2x * zy + z2y * zx;

        double fx = z3x - 1.0;
        double fy = z3y;
        double fpx = 3.0 * z2x;
        double fpy = 3.0 * z2y;

        double denom = fpx * fpx + fpy * fpy;
        if (denom < tol) break;

        double qx = (fx * fpx + fy * fpy) / denom;
        double qy = (fy * fpx - fx * fpy) / denom;
        zx -= qx;
        zy -= qy;

        for (int j = 0; j < 3; j++) {
            double dx = zx - roots[j][0];
            double dy = zy - roots[j][1];
            if (dx * dx + dy * dy < tol)
                return (j + 1) * 1000 + i;
        }
    }
    return 0;
}

// Tricorn (Mandelbar): z_{n+1} = conj(z)^2 + c
int tricornIterations(double real, double imag, int maxIter) {
    double zx = 0.0, zy = 0.0, zx2 = 0.0, zy2 = 0.0;
    for (int i = 0; i < maxIter; i++) {
        if (zx2 + zy2 > 4.0) return i;
        zy = -2.0 * zx * zy + imag;
        zx = zx2 - zy2 + real;
        zx2 = zx * zx;
        zy2 = zy * zy;
    }
    return maxIter;
}

// Phoenix: z_{n+1} = z_n^2 + p_re + p_im * z_{n-1}
int phoenixIterations(double real, double imag, double pReal, double pImag, int maxIter) {
    double zx = real, zy = imag;
    double prevX = 0.0, prevY = 0.0;
    double zx2 = zx * zx, zy2 = zy * zy;
    for (int i = 0; i < maxIter; i++) {
        if (zx2 + zy2 > 4.0) return i;
        double nx = zx2 - zy2 + pReal + pImag * prevX;
        double ny = 2.0 * zx * zy + pImag * prevY;
        prevX = zx; prevY = zy;
        zx = nx; zy = ny;
        zx2 = zx * zx; zy2 = zy * zy;
    }
    return maxIter;
}

// --- Color mapping ---

RGB hsvToRgb(double h, double s, double v) {
    double c = v * s;
    double x = c * (1.0 - std::abs(std::fmod(h / 60.0, 2.0) - 1.0));
    double m = v - c;
    double r, g, b;
    if (h < 60) { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }
    return RGB(uint8_t((r + m) * 255), uint8_t((g + m) * 255), uint8_t((b + m) * 255));
}

RGB getColor(int iterations, const std::string& fractal, int maxIter) {
    if (fractal == "newton") {
        if (iterations >= 3000) {
            double t = 1.0 - double(iterations - 3000) / maxIter;
            return RGB(0, 0, uint8_t(255 * t));
        } else if (iterations >= 2000) {
            double t = 1.0 - double(iterations - 2000) / maxIter;
            return RGB(0, uint8_t(255 * t), 0);
        } else if (iterations >= 1000) {
            double t = 1.0 - double(iterations - 1000) / maxIter;
            return RGB(uint8_t(255 * t), 0, 0);
        }
        return RGB(0, 0, 0);
    }

    if (iterations == maxIter) return RGB(0, 0, 0);

    // Smooth sine-wave palette (matches JS frontend)
    double t = double(iterations) / maxIter;
    uint8_t r = uint8_t(127.5 * (1.0 + std::cos(2.0 * M_PI * (t * 5 + 0.0))));
    uint8_t g = uint8_t(127.5 * (1.0 + std::cos(2.0 * M_PI * (t * 5 + 0.33))));
    uint8_t b = uint8_t(127.5 * (1.0 + std::cos(2.0 * M_PI * (t * 5 + 0.67))));
    return RGB(r, g, b);
}

// --- Main ---

void printUsage(const char* prog) {
    std::cerr << "Usage: " << prog << " [options]\n"
              << "  --fractal <type>   mandelbrot|julia|burning_ship|newton|tricorn|phoenix (default: mandelbrot)\n"
              << "  --width <w>        Image width (default: 800)\n"
              << "  --height <h>       Image height (default: 600)\n"
              << "  --cx <x>           Center X coordinate (default: -0.5)\n"
              << "  --cy <y>           Center Y coordinate (default: 0.0)\n"
              << "  --zoom <z>         Zoom level (default: 1.0)\n"
              << "  --iter <n>         Max iterations (default: 1000)\n"
              << "  --julia-real <r>   Julia C real part (default: -0.7269)\n"
              << "  --julia-imag <i>   Julia C imaginary part (default: 0.1889)\n"
              << "  --format <fmt>     Output format: ppm (default: ppm)\n"
              << "\nOutputs PPM image data to stdout.\n";
}

int main(int argc, char* argv[]) {
    RenderParams p;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") { printUsage(argv[0]); return 0; }
        if (i + 1 >= argc) { std::cerr << "Missing value for " << arg << "\n"; return 1; }

        std::string val = argv[++i];
        if (arg == "--fractal") p.fractal = val;
        else if (arg == "--width") p.width = std::stoi(val);
        else if (arg == "--height") p.height = std::stoi(val);
        else if (arg == "--cx") p.cx = std::stod(val);
        else if (arg == "--cy") p.cy = std::stod(val);
        else if (arg == "--zoom") p.zoom = std::stod(val);
        else if (arg == "--iter") p.maxIter = std::stoi(val);
        else if (arg == "--julia-real") p.juliaReal = std::stod(val);
        else if (arg == "--julia-imag") p.juliaImag = std::stod(val);
        else if (arg == "--phoenix-px") p.phoenixPx = std::stod(val);
        else if (arg == "--phoenix-py") p.phoenixPy = std::stod(val);
        else if (arg == "--format") { /* only ppm for now */ }
        else { std::cerr << "Unknown option: " << arg << "\n"; return 1; }
    }

    // Validate
    if (p.width <= 0 || p.width > 3840) { std::cerr << "Invalid width\n"; return 1; }
    if (p.height <= 0 || p.height > 2160) { std::cerr << "Invalid height\n"; return 1; }
    if (p.maxIter <= 0 || p.maxIter > 10000) { std::cerr << "Invalid iterations\n"; return 1; }
    if (p.zoom <= 0) { std::cerr << "Invalid zoom\n"; return 1; }

    // Compute scale
    double scale = 4.0 / p.zoom;
    double startX = p.cx - scale / 2.0;
    double startY = p.cy - scale / 2.0;
    double stepX = scale / p.width;
    double stepY = scale / p.height;

    // Output PPM header
    std::cout << "P6\n" << p.width << " " << p.height << "\n255\n";

    // Render
    std::vector<uint8_t> row(p.width * 3);

    for (int y = 0; y < p.height; y++) {
        double imag = startY + y * stepY;
        for (int x = 0; x < p.width; x++) {
            double real = startX + x * stepX;

            int iter = 0;
            if (p.fractal == "mandelbrot")
                iter = mandelbrotIterations(real, imag, p.maxIter);
            else if (p.fractal == "julia")
                iter = juliaIterations(real, imag, p.juliaReal, p.juliaImag, p.maxIter);
            else if (p.fractal == "burning_ship")
                iter = burningShipIterations(real, imag, p.maxIter);
            else if (p.fractal == "newton")
                iter = newtonIterations(real, imag, p.maxIter);
            else if (p.fractal == "tricorn")
                iter = tricornIterations(real, imag, p.maxIter);
            else if (p.fractal == "phoenix")
                iter = phoenixIterations(real, imag, p.phoenixPx, p.phoenixPy, p.maxIter);

            RGB c = getColor(iter, p.fractal, p.maxIter);
            int idx = x * 3;
            row[idx] = c.r;
            row[idx + 1] = c.g;
            row[idx + 2] = c.b;
        }
        std::cout.write(reinterpret_cast<const char*>(row.data()), row.size());
    }

    return 0;
}
