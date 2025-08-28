// WebAssembly 分形渲染器 JavaScript 接口
class FractalWASMRenderer {
    constructor() {
        this.module = null;
        this.canvas = null;
        this.ctx = null;
        this.imageData = null;
        this.isRendering = false;
        this.renderStartTime = 0;
        
        // 当前渲染参数
        this.currentParams = {
            fractalType: 0,
            width: 800,
            height: 600,
            centerX: 0,
            centerY: 0,
            zoom: 1.0,
            iterations: 256,
            juliaReal: -0.7269,
            juliaImag: 0.1889
        };
        
        // 分形信息
        this.fractalInfo = {
            0: {
                name: "Mandelbrot Set",
                algorithm: "z<sub>n+1</sub> = z<sub>n</sub>² + c",
                description: "经典逃逸时间分形，展示复平面稳定性"
            },
            1: {
                name: "Julia Set", 
                algorithm: "z<sub>n+1</sub> = z² + c (固定参数c)",
                description: "参数化复多项式迭代，展示连通性变化"
            },
            2: {
                name: "Burning Ship",
                algorithm: "z<sub>n+1</sub> = (|Re(z)| + i|Im(z)|)² + c", 
                description: "Mandelbrot变体，使用绝对值变换产生船形结构"
            },
            3: {
                name: "Newton Fractal",
                algorithm: "z<sub>n+1</sub> = z - f(z)/f'(z), f(z) = z³ - 1",
                description: "牛顿法求根收敛域，三色区域对应立方根"
            }
        };
    }

    async initialize() {
        try {
            // 加载 WebAssembly 模块
            this.updateStatus("正在加载 WebAssembly 模块...");
            this.module = await FractalsModule();
            
            // 测试基本函数调用
            try {
                const testResult = this.module._testFunction();
                console.log("WASM 测试函数返回:", testResult);
                if (testResult !== 42) {
                    throw new Error("测试函数返回值不正确");
                }
            } catch (testError) {
                console.error("WASM 测试函数调用失败:", testError);
                throw new Error("WASM 模块基本测试失败");
            }
            
            // 检查关键函数是否存在
            console.log("可用的函数:", Object.keys(this.module).filter(k => typeof this.module[k] === 'function'));
            
            // 初始化 Canvas
            this.canvas = document.getElementById('fractalCanvas');
            this.ctx = this.canvas.getContext('2d');
            
            // 设置事件监听器
            this.setupEventListeners();
            
            this.updateStatus("WebAssembly 模块加载完成");
            document.getElementById('wasmStatus').textContent = '已就绪';
            
            // 渲染初始分形
            await this.renderFractal();
            
            return true;
        } catch (error) {
            console.error('WebAssembly 初始化失败:', error);
            this.updateStatus("WebAssembly 加载失败: " + error.message);
            document.getElementById('wasmStatus').textContent = '加载失败';
            return false;
        }
    }

    setupEventListeners() {
        // 控件事件
        document.getElementById('fractalType').addEventListener('change', (e) => {
            this.currentParams.fractalType = parseInt(e.target.value);
            this.updateFractalInfo();
            this.showJuliaControls(this.currentParams.fractalType === 1);
        });

        document.getElementById('imageSize').addEventListener('change', (e) => {
            const [width, height] = e.target.value.split(',').map(Number);
            this.currentParams.width = width;
            this.currentParams.height = height;
            this.resizeCanvas();
        });

        document.getElementById('iterations').addEventListener('input', (e) => {
            this.currentParams.iterations = parseInt(e.target.value);
            document.getElementById('iterationsValue').textContent = e.target.value;
        });

        document.getElementById('zoom').addEventListener('input', (e) => {
            this.currentParams.zoom = parseFloat(e.target.value);
            document.getElementById('zoomValue').textContent = e.target.value;
            document.getElementById('zoomInfo').textContent = `缩放: ${e.target.value}x`;
        });

        document.getElementById('centerX').addEventListener('input', (e) => {
            this.currentParams.centerX = parseFloat(e.target.value);
        });

        document.getElementById('centerY').addEventListener('input', (e) => {
            this.currentParams.centerY = parseFloat(e.target.value);
        });

        document.getElementById('juliaReal').addEventListener('input', (e) => {
            this.currentParams.juliaReal = parseFloat(e.target.value);
        });

        document.getElementById('juliaImag').addEventListener('input', (e) => {
            this.currentParams.juliaImag = parseFloat(e.target.value);
        });

        // 按钮事件
        document.getElementById('renderBtn').addEventListener('click', () => this.renderFractal());
        document.getElementById('resetBtn').addEventListener('click', () => this.resetView());
        document.getElementById('saveBtn').addEventListener('click', () => this.saveImage());

        // Canvas 鼠标事件
        this.canvas.addEventListener('click', (e) => this.handleCanvasClick(e));
        this.canvas.addEventListener('mousemove', (e) => this.handleMouseMove(e));
        this.canvas.addEventListener('wheel', (e) => this.handleWheel(e));
    }

    updateFractalInfo() {
        const info = this.fractalInfo[this.currentParams.fractalType];
        const infoElement = document.getElementById('fractalInfo');
        infoElement.innerHTML = `
            <strong>${info.name}</strong><br>
            算法: ${info.algorithm}<br>
            ${info.description}
        `;
    }

    showJuliaControls(show) {
        document.getElementById('juliaControls').style.display = show ? 'block' : 'none';
    }

    resizeCanvas() {
        this.canvas.width = this.currentParams.width;
        this.canvas.height = this.currentParams.height;
        this.imageData = this.ctx.createImageData(this.currentParams.width, this.currentParams.height);
    }

    async renderFractal() {
        if (this.isRendering || !this.module) return;
        
        this.isRendering = true;
        this.renderStartTime = performance.now();
        
        try {
            // 显示加载动画
            document.getElementById('loading').style.display = 'block';
            document.getElementById('renderBtn').disabled = true;
            this.updateStatus("正在渲染...");

            // 创建图像数据缓冲区
            const imageSize = this.currentParams.width * this.currentParams.height * 4;
            const imageDataPtr = this.module._malloc(imageSize);
            
            if (!imageDataPtr) {
                throw new Error("内存分配失败");
            }
            
            // 调用 WebAssembly 渲染函数 - 使用 emscripten 绑定的函数
            try {
                this.module.renderFractalImage(
                    this.currentParams.fractalType,
                    this.currentParams.width,
                    this.currentParams.height,
                    this.currentParams.centerX,
                    this.currentParams.centerY,
                    this.currentParams.zoom,
                    this.currentParams.juliaReal,
                    this.currentParams.juliaImag,
                    this.currentParams.iterations,
                    imageDataPtr
                );
            } catch (bindError) {
                console.warn("绑定函数调用失败，尝试直接调用:", bindError);
                // 备用方案：直接调用 C 函数
                this.module._renderFractalImage(
                    this.currentParams.fractalType,
                    this.currentParams.width,
                    this.currentParams.height,
                    this.currentParams.centerX,
                    this.currentParams.centerY,
                    this.currentParams.zoom,
                    this.currentParams.juliaReal,
                    this.currentParams.juliaImag,
                    this.currentParams.iterations,
                    imageDataPtr
                );
            }

            // 从 WebAssembly 内存复制图像数据
            const imageArray = new Uint8ClampedArray(
                this.module.HEAPU8.buffer, 
                imageDataPtr, 
                imageSize
            );
            
            // 更新 Canvas
            this.imageData = new ImageData(
                new Uint8ClampedArray(imageArray),
                this.currentParams.width,
                this.currentParams.height
            );
            
            this.ctx.putImageData(this.imageData, 0, 0);
            
            // 释放内存
            this.module._free(imageDataPtr);
            
            // 计算性能统计
            const renderTime = performance.now() - this.renderStartTime;
            const totalPixels = this.currentParams.width * this.currentParams.height;
            const pixelsPerSec = Math.round(totalPixels / (renderTime / 1000));
            
            // 更新UI
            document.getElementById('renderTime').textContent = renderTime.toFixed(1) + 'ms';
            document.getElementById('pixelsPerSec').textContent = pixelsPerSec.toLocaleString();
            document.getElementById('memoryUsage').textContent = (imageSize / 1024 / 1024).toFixed(1) + 'MB';
            
            this.updateStatus('渲染完成 - ' + renderTime.toFixed(1) + 'ms');
            
        } catch (error) {
            console.error('渲染失败:', error);
            this.updateStatus("渲染失败: " + error.message);
        } finally {
            this.isRendering = false;
            document.getElementById('loading').style.display = 'none';
            document.getElementById('renderBtn').disabled = false;
        }
    }

    handleCanvasClick(event) {
        const rect = this.canvas.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;
        
        // 转换为复平面坐标
        const scale = 4.0 / this.currentParams.zoom;
        const realX = this.currentParams.centerX + (x / this.currentParams.width - 0.5) * scale;
        const realY = this.currentParams.centerY + (y / this.currentParams.height - 0.5) * scale;
        
        // 更新中心点
        this.currentParams.centerX = realX;
        this.currentParams.centerY = realY;
        
        // 更新UI
        document.getElementById('centerX').value = realX.toFixed(6);
        document.getElementById('centerY').value = realY.toFixed(6);
        
        // 重新渲染
        this.renderFractal();
    }

    handleMouseMove(event) {
        const rect = this.canvas.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;
        
        // 转换为复平面坐标
        const scale = 4.0 / this.currentParams.zoom;
        const realX = this.currentParams.centerX + (x / this.currentParams.width - 0.5) * scale;
        const realY = this.currentParams.centerY + (y / this.currentParams.height - 0.5) * scale;
        
        document.getElementById('coordinates').textContent = 
            `坐标: (${realX.toFixed(4)}, ${realY.toFixed(4)})`;
    }

    handleWheel(event) {
        event.preventDefault();
        
        const zoomFactor = event.deltaY > 0 ? 0.9 : 1.1;
        this.currentParams.zoom *= zoomFactor;
        
        // 限制缩放范围
        this.currentParams.zoom = Math.max(0.1, Math.min(10000, this.currentParams.zoom));
        
        // 更新UI
        document.getElementById('zoom').value = this.currentParams.zoom;
        document.getElementById('zoomValue').textContent = this.currentParams.zoom.toFixed(1);
        document.getElementById('zoomInfo').textContent = `缩放: ${this.currentParams.zoom.toFixed(1)}x`;
        
        // 重新渲染
        this.renderFractal();
    }

    resetView() {
        this.currentParams.centerX = 0;
        this.currentParams.centerY = 0;
        this.currentParams.zoom = 1.0;
        
        // 更新UI
        document.getElementById('centerX').value = 0;
        document.getElementById('centerY').value = 0;
        document.getElementById('zoom').value = 1;
        document.getElementById('zoomValue').textContent = '1.0';
        document.getElementById('zoomInfo').textContent = '缩放: 1.0x';
        
        this.renderFractal();
    }

    saveImage() {
        if (!this.canvas) return;
        
        const link = document.createElement('a');
        link.download = `fractal_${this.fractalInfo[this.currentParams.fractalType].name.replace(' ', '_')}_${Date.now()}.png`;
        link.href = this.canvas.toDataURL();
        link.click();
    }

    updateStatus(message) {
        document.getElementById('progressInfo').textContent = message;
    }
}

// 全局渲染器实例
let fractalRenderer;

// 页面加载完成后初始化
document.addEventListener('DOMContentLoaded', async () => {
    fractalRenderer = new FractalWASMRenderer();
    
    // 等待 WebAssembly 模块加载
    const success = await fractalRenderer.initialize();
    
    if (!success) {
        alert('WebAssembly 模块加载失败，请检查浏览器兼容性或网络连接。');
    }
});

// 导出供其他脚本使用
window.FractalWASMRenderer = FractalWASMRenderer;
