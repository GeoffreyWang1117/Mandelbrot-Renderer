#include <emscripten.h>
#include <emscripten/bind.h>
#include <cmath>
#include <cstdint>

using namespace emscripten;

// Color helper
struct Color {
    uint8_t r, g, b, a;
    Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}
};

Color hsvToRgb(double h, double s, double v) {
    double c = v * s;
    double x = c * (1.0 - fabs(fmod(h / 60.0, 2.0) - 1.0));
    double m = v - c;
    double r, g, b;
    if      (h < 60)  { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else               { r = c; g = 0; b = x; }
    return Color((uint8_t)((r+m)*255), (uint8_t)((g+m)*255), (uint8_t)((b+m)*255));
}

// ============================================================================
// Fractal iteration functions — optimized with raw doubles, no std::complex
// ============================================================================

extern "C" {

EMSCRIPTEN_KEEPALIVE
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

EMSCRIPTEN_KEEPALIVE
int juliaIterations(double real, double imag, double cReal, double cImag, int maxIter) {
    double zx = real, zy = imag;
    double zx2 = zx * zx, zy2 = zy * zy;
    for (int i = 0; i < maxIter; i++) {
        if (zx2 + zy2 > 4.0) return i;
        zy = 2.0 * zx * zy + cImag;
        zx = zx2 - zy2 + cReal;
        zx2 = zx * zx;
        zy2 = zy * zy;
    }
    return maxIter;
}

EMSCRIPTEN_KEEPALIVE
int burningShipIterations(double real, double imag, int maxIter) {
    double zx = 0.0, zy = 0.0, zx2 = 0.0, zy2 = 0.0;
    for (int i = 0; i < maxIter; i++) {
        if (zx2 + zy2 > 4.0) return i;
        double ax = fabs(zx), ay = fabs(zy);
        zy = 2.0 * ax * ay + imag;
        zx = ax * ax - ay * ay + real;
        zx2 = zx * zx;
        zy2 = zy * zy;
    }
    return maxIter;
}

EMSCRIPTEN_KEEPALIVE
int newtonIterations(double real, double imag, int maxIter) {
    double zx = real, zy = imag;
    const double tol = 1e-6;
    static const double roots[][2] = {
        {1.0, 0.0}, {-0.5, 0.866025403784}, {-0.5, -0.866025403784}
    };

    for (int i = 0; i < maxIter; i++) {
        double z2x = zx * zx - zy * zy;
        double z2y = 2.0 * zx * zy;
        double z3x = z2x * zx - z2y * zy;
        double z3y = z2x * zy + z2y * zx;

        double fx = z3x - 1.0, fy = z3y;
        double fpx = 3.0 * z2x, fpy = 3.0 * z2y;
        double denom = fpx * fpx + fpy * fpy;
        if (denom < tol) break;

        zx -= (fx * fpx + fy * fpy) / denom;
        zy -= (fy * fpx - fx * fpy) / denom;

        for (int j = 0; j < 3; j++) {
            double dx = zx - roots[j][0], dy = zy - roots[j][1];
            if (dx * dx + dy * dy < tol)
                return (j + 1) * 1000 + i;
        }
    }
    return 0;
}

// Tricorn (Mandelbar): z_{n+1} = conj(z)^2 + c
EMSCRIPTEN_KEEPALIVE
int tricornIterations(double real, double imag, int maxIter) {
    double zx = 0.0, zy = 0.0, zx2 = 0.0, zy2 = 0.0;
    for (int i = 0; i < maxIter; i++) {
        if (zx2 + zy2 > 4.0) return i;
        zy = -2.0 * zx * zy + imag;  // conjugate flips sign
        zx = zx2 - zy2 + real;
        zx2 = zx * zx;
        zy2 = zy * zy;
    }
    return maxIter;
}

// Phoenix: z_{n+1} = z_n^2 + p_re + p_im * z_{n-1}
EMSCRIPTEN_KEEPALIVE
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

// Batch computation for higher throughput
EMSCRIPTEN_KEEPALIVE
void computeFractalBatch(
    int fractalType,
    double* realValues, double* imagValues,
    double cReal, double cImag,
    int count, int maxIter, int* results
) {
    for (int i = 0; i < count; i++) {
        switch (fractalType) {
            case 0: results[i] = mandelbrotIterations(realValues[i], imagValues[i], maxIter); break;
            case 1: results[i] = juliaIterations(realValues[i], imagValues[i], cReal, cImag, maxIter); break;
            case 2: results[i] = burningShipIterations(realValues[i], imagValues[i], maxIter); break;
            case 3: results[i] = newtonIterations(realValues[i], imagValues[i], maxIter); break;
            case 4: results[i] = tricornIterations(realValues[i], imagValues[i], maxIter); break;
            case 5: results[i] = phoenixIterations(realValues[i], imagValues[i], cReal, cImag, maxIter); break;
            default: results[i] = 0;
        }
    }
}

EMSCRIPTEN_KEEPALIVE
int testFunction() { return 42; }

// Full image render in WASM (avoids per-pixel JS-to-WASM call overhead)
EMSCRIPTEN_KEEPALIVE
void renderFractalImage(
    int fractalType, int width, int height,
    double centerX, double centerY, double zoom,
    double cReal, double cImag, int maxIter,
    uint8_t* imageData
) {
    if (!imageData || width <= 0 || height <= 0 || maxIter <= 0) return;
    if (zoom <= 0) zoom = 1.0;
    if (maxIter > 10000) maxIter = 10000;

    double scale = 4.0 / zoom;
    double stepX = scale / width;
    double stepY = scale / height;
    double startX = centerX - scale / 2.0;
    double startY = centerY - scale / 2.0;

    for (int y = 0; y < height; y++) {
        double imag = startY + y * stepY;
        for (int x = 0; x < width; x++) {
            double real = startX + x * stepX;
            int iterations;

            switch (fractalType) {
                case 0: iterations = mandelbrotIterations(real, imag, maxIter); break;
                case 1: iterations = juliaIterations(real, imag, cReal, cImag, maxIter); break;
                case 2: iterations = burningShipIterations(real, imag, maxIter); break;
                case 3: iterations = newtonIterations(real, imag, maxIter); break;
                case 4: iterations = tricornIterations(real, imag, maxIter); break;
                case 5: iterations = phoenixIterations(real, imag, cReal, cImag, maxIter); break;
                default: iterations = 0;
            }

            Color color;
            if (fractalType == 3) { // Newton
                if      (iterations >= 3000) { double t = 1.0 - double(iterations-3000)/maxIter; color = Color(0,0,(uint8_t)(255*t)); }
                else if (iterations >= 2000) { double t = 1.0 - double(iterations-2000)/maxIter; color = Color(0,(uint8_t)(255*t),0); }
                else if (iterations >= 1000) { double t = 1.0 - double(iterations-1000)/maxIter; color = Color((uint8_t)(255*t),0,0); }
                else color = Color(0,0,0);
            } else if (iterations == maxIter) {
                color = Color(0,0,0);
            } else {
                // Smooth sine-wave palette matching JS version
                double t = double(iterations) / maxIter;
                uint8_t r = (uint8_t)(127.5 * (1.0 + cos(2.0*M_PI*(t*5 + 0.0))));
                uint8_t g = (uint8_t)(127.5 * (1.0 + cos(2.0*M_PI*(t*5 + 0.33))));
                uint8_t b = (uint8_t)(127.5 * (1.0 + cos(2.0*M_PI*(t*5 + 0.67))));
                color = Color(r, g, b);
            }

            int idx = (y * width + x) * 4;
            imageData[idx]   = color.r;
            imageData[idx+1] = color.g;
            imageData[idx+2] = color.b;
            imageData[idx+3] = 255;
        }
    }
}

} // extern "C"

// Emscripten bindings
EMSCRIPTEN_BINDINGS(fractals) {
    function("mandelbrotIterations", &mandelbrotIterations);
    function("juliaIterations", &juliaIterations);
    function("burningShipIterations", &burningShipIterations);
    function("newtonIterations", &newtonIterations);
    function("tricornIterations", &tricornIterations);
    function("phoenixIterations", &phoenixIterations);
    function("computeFractalBatch", &computeFractalBatch, allow_raw_pointers());
    function("renderFractalImage", &renderFractalImage, allow_raw_pointers());
}
