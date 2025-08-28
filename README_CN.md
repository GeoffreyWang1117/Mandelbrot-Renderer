````markdown
# 高性能分形渲染器

*其他语言版本: [English](README.md) | [中文](README_CN.md)*

一个高性能的分形渲染引擎，具有多种并行计算实现，支持 Mandelbrot 集、Julia 集和其他美丽的数学分形。展示了从单线程CPU到大规模并行GPU计算的演进过程。

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![C++](https://img.shields.io/badge/C++-17-blue)
![CUDA](https://img.shields.io/badge/CUDA-12.6-green)
![License](https://img.shields.io/badge/license-MIT-blue)

## 🌐 交互式网页演示

**[🚀 在线体验演示](https://geoffreywang1117.github.io/Mandelbrot-Renderer/)** - 直接在浏览器中体验多种分形类型！

### 网页 JavaScript 版本
- **切换分形类型** - Mandelbrot 集、Julia 集、Burning Ship、Newton 分形
- **点击缩放** 到任意点
- **拖拽平移** 浏览分形
- **实时调整 Julia 集参数**
- **探索独特的数学算法** 和视觉特征
- **实时渲染** 显示性能指标  
- **移动端响应式** 设计

### WebAssembly 高性能版本
**[⚡ WebAssembly 分形渲染器](https://geoffreywang1117.github.io/Mandelbrot-Renderer/wasm-fractal.html)** - 在浏览器中体验接近原生的性能！

- **比 JavaScript 快 5-20 倍**
- **相同的交互功能** 但性能增强
- **从 C++ 编译** 使用 Emscripten
- **自动回退** 到 JavaScript（如果不支持 WebAssembly）

## 项目路线图与开发阶段

| 阶段 | 功能特性 | 技术栈 | 状态 |
|------|---------|--------|------|
| ✅ **第1步** | 基础CLI渲染（灰度PNG输出） | C++ + STL | ✅ **已完成** |
| ✅ **第2步** | OpenMP 并行加速 | OpenMP | ✅ **已完成** |
| ✅ **第3步** | 彩色渲染（逃逸时间→颜色映射） | HSV颜色插值 | ✅ **已完成** |
| ✅ **第4步** | 命令行参数控制区域/分辨率 | `argc/argv`, `getopt` | ✅ **已完成** |
| ✅ **第5步** | 生成缩放动画帧和视频合成 | Python+FFmpeg | ✅ **已完成** |
| ✅ **第6步** | CUDA/GPU加速版本对比 | CUDA | ✅ **已完成** |
| ✅ **第7步** | 交互式网页演示 | HTML5 + JavaScript | ✅ **已完成** |
| ✅ **第8步** | **Julia 集、Burning Ship & Newton 分形** | **C++/CUDA/Web** | ✅ **已完成** |
| ✅ **第9步** | **WebAssembly 高性能网页版本** | **Emscripten** | ✅ **已完成** |
| 🟪 **第10步** | OpenGL GUI 实时缩放/平移 | C++ + GLFW/GLAD | 💤 **已规划** |

## 特性

- **多种分形类型**:
  - ✅ **Mandelbrot 集**: 经典逃逸时间分形
  - ✅ **Julia 集**: 动态参数探索  
  - ✅ **Burning Ship**: 使用绝对值的 Mandelbrot 变体
  - ✅ **Newton 分形**: 根查找可视化
  - 📋 **几何分形**: Sierpinski、Barnsley Fern、Dragon Curve（未来）

- **多种实现方式**:
  - 纯CPU单线程实现
  - OpenMP多线程CPU并行化
  - CUDA GPU大规模并行实现
  - 交互式网页JavaScript版本
  - **WebAssembly 高性能网页版本**
  
- **高性能表现**:
  - 双RTX 3090配置下达到 **20亿像素/秒**
  - 相比网页JavaScript版本 **34,000倍加速**
  - 相比单线程CPU **1,216倍加速**
  - 支持从小图像到超高分辨率扩展（已测试至4K+）

- **灵活配置**:
  - 可调节图像分辨率
  - 可自定义迭代次数
  - 可配置复平面区域
  - 多GPU系统设备选择
  - 可调节CUDA块大小

- **专业输出**:
  - 高质量PPM图像格式
  - 使用HSV色彩空间的平滑色彩渐变
  - Python脚本动画生成
  - 批处理能力
  - 全面的性能报告

## 性能对比

| 实现方式 | 800x600 (毫秒) | 性能 | 加速比 | 平台 |
|---|---|---|---|---|
| **网页JavaScript** | **~10,000** | **5万像素/秒** | **1.0倍** | **浏览器** |
| **WebAssembly** | **~2,000** | **24万像素/秒** | **5倍** | **浏览器** |
| CPU单线程 | 292 | 160万像素/秒 | 32倍 | 原生C++ |
| OpenMP (32核) | 4 | 1.2亿像素/秒 | 2,400倍 | 多核CPU |
| **CUDA GPU** | **24** | **2000万像素/秒** | **400倍** | **GPU** |

对于4K分辨率（4000x3000）:
- **CUDA GPU**: 6毫秒，**20亿像素/秒**，相比网页版 **40,000倍加速**

## 快速开始

### 环境要求

- C++17兼容编译器
- OpenMP支持（多线程版本需要）
- NVIDIA CUDA toolkit 12.0+（GPU版本需要）
- NVIDIA GPU，计算能力6.0+

### 构建

```bash
# 克隆仓库
git clone https://github.com/GeoffreyWang1117/Mandelbrot-Renderer.git
cd Mandelbrot-Renderer

# 构建CPU和OpenMP版本
mkdir build && cd build
cmake ..
make

# 构建CUDA独立版本
cd ..
nvcc -O3 -o mandelbrot_cuda mandelbrot_cuda_standalone.cu
```

### 使用示例

#### CPU单线程
```bash
./build/mandelbrot_cpu -s 800x600 -i 1000 -o mandelbrot_cpu.ppm
```

#### OpenMP多线程
```bash
./build/mandelbrot_omp -s 1920x1080 -i 2000 -o mandelbrot_hd.ppm
```

#### CUDA GPU加速
```bash
# 基础用法
./mandelbrot_cuda -s 1920x1080 -i 1000 -o mandelbrot_gpu.ppm

# 高分辨率自定义区域
./mandelbrot_cuda -s 4000x3000 -i 2000 -r -0.5,0.5,-0.5,0.5 -o mandelbrot_4k.ppm

# 多GPU系统（选择设备）
./mandelbrot_cuda -d 1 -s 2000x1500 -i 1500 -o mandelbrot_gpu2.ppm

# 显示GPU信息
./mandelbrot_cuda --info
```

## 命令行选项

### 通用选项（所有版本）
- `-o <文件>`: 输出文件名（默认: mandelbrot.ppm）
- `-s <宽>x<高>`: 图像尺寸（默认: 800x600）
- `-i <迭代数>`: 最大迭代次数（默认: 1000）
- `-r <最小x,最大x,最小y,最大y>`: 复平面区域（默认: -2.5,1.5,-1.5,1.5）
- `-h, --help`: 显示帮助信息

### CUDA特定选项
- `-d <设备>`: CUDA设备ID（默认: 0）
- `-b <bx>x<by>`: 线程块大小（默认: 16x16）
- `--info`: 显示CUDA设备信息

## 项目结构

```
mandelbrot/
├── docs/                          # 🌐 GitHub Pages 部署
│   ├── index.html                 # 主要网页演示 (JavaScript)
│   └── wasm-fractal.html         # WebAssembly 高性能版本
├── web/                           # 🌐 开发网页演示
│   ├── index.html                 # 现代响应式界面
│   ├── mandelbrot.js             # JavaScript实现
│   └── README.md                 # 网页演示文档
├── web-wasm/                     # ⚡ WebAssembly 开发文件
│   ├── fractals_wasm.cpp         # C++ WebAssembly 实现
│   ├── CMakeLists.txt            # Emscripten 构建配置
│   ├── build.sh                  # WebAssembly 构建脚本
│   └── README.md                 # WebAssembly 文档
├── wasm/                         # ⚡ WebAssembly 构建输出
│   ├── src/fractals_wasm.cpp     # 源 C++ 实现
│   ├── CMakeLists.txt            # 构建配置
│   └── build.sh                  # 编译脚本
├── src/
│   ├── render.cpp/hpp            # CPU单线程实现
│   ├── render_omp.cpp/hpp        # OpenMP多线程实现
│   ├── render_cuda.cu/hpp        # CUDA GPU实现
│   ├── main.cpp                  # CPU版本主程序
│   ├── main_omp.cpp             # OpenMP版本主程序
│   └── main_unified.cpp         # 统一主程序支持所有模式
├── mandelbrot_cuda_standalone.cu # 独立CUDA实现
├── scripts/
│   ├── animate.py               # 生成动画帧
│   └── create_video.py          # 转换帧为视频
├── .github/workflows/           # GitHub Actions网页部署
├── CMakeLists.txt              # 构建配置
├── PERFORMANCE_REPORT.md       # 详细性能分析
└── README.md                   # 本文件
```

## 技术详情

### 算法实现
- **逃逸时间算法**: 经典Mandelbrot迭代方法
- **平滑着色**: HSV色彩空间渐变过渡
- **数值精度**: 双精度浮点确保准确性
- **收敛检测**: 可配置迭代限制

### 并行计算方法

#### OpenMP多线程
- **线程并行化**: 使用最优调度的并行for循环
- **负载均衡**: 动态工作分配
- **内存优化**: 线程本地存储提升性能
- **扩展性**: 线性扩展至可用CPU核心数

#### CUDA GPU计算
- **大规模并行化**: 每像素一个线程
- **内存合并**: 优化内存访问模式
- **线程块优化**: 可配置块大小（8x8, 16x16, 32x32）
- **设备利用**: 支持多GPU系统

### 性能优化
- **编译器优化**: -O3优化标志
- **架构定向**: 原生CPU和GPU架构支持
- **内存管理**: 高效的主机-设备传输
- **内核优化**: 最小化分支，最大化占用率

## 应用场景

### 教育用途
- **并行计算概念**: 比较不同并行化策略
- **性能分析**: 理解扩展特性
- **GPU计算**: 学习CUDA编程基础
- **分形数学**: 可视化复杂数学概念

### 专业应用
- **高分辨率渲染**: 生成出版级分形图像
- **批处理**: 渲染多个区域或参数集
- **性能基准测试**: 测试GPU计算能力
- **研究应用**: 为数学分析生成数据集

### 创意应用
- **数字艺术**: 创造令人惊叹的分形艺术作品
- **动画制作**: 生成平滑缩放序列
- **桌面壁纸**: 制作高分辨率桌面背景
- **教学材料**: 为教学创建可视化内容

## 系统需求

### 最低配置
- **CPU**: 多核处理器（推荐4+核心）
- **内存**: 4GB RAM
- **GPU**: NVIDIA GPU with CUDA支持（可选）
- **存储**: 100MB用于构建工具和输出

### 推荐配置
- **CPU**: 高核心数处理器（16+核心）
- **内存**: 16GB+ RAM用于大图像
- **GPU**: 现代NVIDIA GPU（RTX系列或同等）
- **存储**: SSD用于更快I/O操作

### 测试配置
- **CPU**: 32核系统
- **GPU**: 双NVIDIA GeForce RTX 3090（各24GB VRAM）
- **内存**: 48GB总GPU VRAM
- **CUDA**: 版本12.6

## 故障排除

### 常见问题

#### CUDA编译
```bash
# 如果找不到nvcc
export PATH=/usr/local/cuda/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH

# 检查CUDA安装
nvcc --version
nvidia-smi
```

#### OpenMP问题
```bash  
# 安装OpenMP开发包
sudo apt-get install libomp-dev  # Ubuntu/Debian
sudo yum install libgomp-devel    # CentOS/RHEL
```

#### 内存问题
- **大图像**: 降低分辨率或使用多GPU设置
- **系统内存**: 关闭其他应用程序
- **GPU内存**: 使用更小的批量大小或更低精度

## 贡献

欢迎贡献！改进方向：

- **多GPU渲染**: 实现多GPU工作分配
- **双精度**: 添加更高数值精度支持
- **附加颜色方案**: 实现不同着色算法
- **实时渲染**: 添加交互式可视化功能
- **内存优化**: 实现超大图像的分块渲染
- **附加分形**: 添加更多分形类型支持（Sierpinski、Barnsley Fern等）
- **OpenGL GUI**: 实时交互缩放平移界面
- **性能优化**: 进一步的 WebAssembly 和 CUDA 改进

## 技术关键词

`#C++` `#OpenMP` `#CUDA` `#分形` `#并行计算` `#FFmpeg`  
`#图像渲染` `#数值精度` `#缩放动画` `#WebGL` `#GPGPU`

## 性能与基准测试计划

| 配置 | 分辨率 | 单核CPU | OpenMP 4线程 | OpenMP 16线程 | CUDA GPU |
|------|--------|---------|--------------|---------------|----------|
| Intel i7-12800HX | 800×600 | 3.2秒 | 1.2秒 | 0.42秒 | 0.024秒 |
| AMD 5950X | 1920×1080 | ~8秒 | ~2秒 | ~0.7秒 | ~0.06秒 |
| RTX 3090 | 4000×3000 | N/A | N/A | N/A | 0.006秒 |

📊 基准测试输出格式（CSV + 图表）：

```csv
分辨率,实现方式,线程数,时间_毫秒,像素每秒
800x600,cpu,1,3200,150000
800x600,openmp,16,420,1140000  
800x600,cuda,1024,24,20000000
4000x3000,cuda,1024,6,2000000000
```

## 未来开发计划

### 短期（1-2个月）
- [ ] **OpenGL实时渲染器**: 交互式缩放/平移及即时反馈
- [ ] **多GPU支持**: 在多个GPU间分配渲染任务
- [ ] **高级着色**: 附加颜色方案和算法
- [ ] **性能分析**: 详细分析工具和优化

### 中期（3-6个月）
- [ ] **附加分形**: Sierpinski 三角形、Barnsley 蕨类、Dragon 曲线
- [ ] **精度选项**: 任意精度算术支持
- [ ] **分布式计算**: 基于网络的多机渲染
- [ ] **移动应用**: 原生 iOS/Android 应用程序

### 长期（6+个月）
- [ ] **VR/AR可视化**: 沉浸式分形探索
- [ ] **机器学习优化**: AI辅助参数优化
- [ ] **云渲染服务**: 可扩展的服务器端渲染
- [ ] **教育平台**: 分形数学交互式学习工具

## 许可证

MIT许可证 - 详见LICENSE文件。

## 作者

**Geoffrey Wang创建**（Claude AI辅助）

本项目是渐进式并行计算优化的综合演示，展示了从单线程CPU实现到大规模并行GPU计算的完整历程。该实现展示了现代软件工程实践、高性能计算技术，以及并行计算概念的教育价值。

---

**基准测试结果**: 在双RTX 3090系统上，大图像达到**20亿像素/秒**，相比单线程CPU实现**1,216倍加速**。
