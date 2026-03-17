# 分形渲染器

高性能分形渲染引擎，支持 6 种分形、多种计算后端（CPU、OpenMP、CUDA、WebAssembly），以及可部署在 EC2 上的服务端渲染 API。

![C++](https://img.shields.io/badge/C++-17-blue)
![CUDA](https://img.shields.io/badge/CUDA-12.6-green)
![Docker](https://img.shields.io/badge/Docker-ready-blue)
![License](https://img.shields.io/badge/license-MIT-blue)

**[在线演示](https://geoffreywang1117.github.io/Mandelbrot-Renderer/)** | **[WebAssembly 版本](https://geoffreywang1117.github.io/Mandelbrot-Renderer/wasm-fractal.html)**

## 支持的分形类型

| 分形 | 公式 | 说明 |
|------|------|------|
| Mandelbrot | z = z² + c | 经典逃逸时间分形，含心形区域优化 |
| Julia | z = z² + c (固定 c) | 参数空间探索，交互式 c 控制 |
| Burning Ship | z = (\|Re(z)\| + i\|Im(z)\|)² + c | 绝对值变体，火焰状图案 |
| Newton | z = z - f(z)/f'(z), f = z³ - 1 | 牛顿法收敛域（RGB三色） |
| Tricorn | z = conj(z)² + c | 共轭变体，三对称图案 |
| Phoenix | z = z² + p_re + p_im * z_{n-1} | 使用迭代历史，羽毛状图案 |

## 性能数据

| 后端 | 800x600 | 像素/秒 | 加速比 |
|------|---------|---------|--------|
| Web JavaScript | ~3,000 ms | ~160K | 1x |
| WebAssembly | ~600 ms | ~800K | 5x |
| CPU 单线程 | 292 ms | 1.6M | 10x |
| OpenMP (32核) | 4 ms | 120M | 750x |
| CUDA GPU | 0.024 ms | 20B | 125,000x |

## 快速开始

### 在线演示

访问 **[在线演示](https://geoffreywang1117.github.io/Mandelbrot-Renderer/)** — 滚轮缩放、点击定位、拖拽平移。

### 本地开发

```bash
git clone https://github.com/GeoffreyWang1117/Mandelbrot-Renderer.git
cd Mandelbrot-Renderer
make dev        # 静态文件服务器
make server     # 完整后端（C++ API + Node.js）
```

### EC2 部署（t3a.micro 安全）

```bash
# 一键部署（预构建镜像，推荐）
curl -fsSL https://raw.githubusercontent.com/GeoffreyWang1117/Mandelbrot-Renderer/main/deploy.sh | bash

# 或手动
docker compose -f docker-compose.prod.yml pull && docker compose -f docker-compose.prod.yml up -d
```

## 服务端 API

```
GET /api/render?fractal=mandelbrot&width=1920&height=1080&zoom=1&iter=1000&format=png
GET /api/wallpaper/mandelbrot-spiral?resolution=3840x2160
GET /api/health
```

最大分辨率 3840x2160，并发渲染上限 2（`MAX_RENDERS` 环境变量可配置），繁忙时返回 503。

## Docker 部署

| 模式 | 命令 | 内存需求 | 适用实例 |
|------|------|---------|---------|
| 预构建拉取 | `make docker-prod` | 峰值 290 MB | t3a.micro ($6.80/月) |
| 服务器上构建 | `make docker-up` | 峰值 570 MB | t3a.small+ |

## 许可证

MIT

## 作者

Geoffrey Wang（Claude AI 辅助）
