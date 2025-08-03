# 🧮 Mandelbrot Renderer · 渲染器计划书（持续更新）

## 📌 项目简介

本项目致力于构建一个高性能、可视化、可交互的 **Mandelbrot 分形图像渲染器**，使用 C++/OpenMP/OpenGL/Python/FFmpeg 等工具，支持 CLI 渲染与图形展示，适合作为：

- C++ 并行计算 & OpenMP 入门项目
- GPU 与 CPU 性能比较实验
- Fractal 分形图像艺术探索与视频生成工具
- GitHub 展示型项目（包含动态图、性能 benchmark）

---

## 🧭 项目目标（分阶段）

| 阶段 | 内容 | 工具 | 状态 |
|------|------|------|------|
| ✅ Step 1 | 基础 CLI 渲染（灰度图输出 PNG） | C++ + STL | ✅ 已完成 |
| ✅ Step 2 | 加入 OpenMP 并行加速 | OpenMP | ✅ 已完成 |
| 🔄 Step 3 | 渲染彩色图像（escape time → color mapping） | colormap 插值 | ⏳ 进行中 |
| 🟨 Step 4 | 命令行参数控制区域 / 分辨率 | `argc/argv`, `getopt` | ⏳ 未开始 |
| 🟦 Step 5 | 生成 zoom 动画帧并合成 gif/mp4 | Python+FFmpeg | ⏳ 未开始 |
| 🟪 Step 6 | 加入 OpenGL GUI 实时拖动缩放 | C++ + GLFW/GLAD | 💤 待规划 |
| 🟥 Step 7 | CUDA/GPU 加速版本对比 | CUDA or OpenCL | 💤 待规划 |
| 🟫 Step 8 | WebAssembly移植展示 | Emscripten | 💤 待规划 |

---

## 📂 项目结构建议

```
Mandelbrot-Renderer/
├── src/                      # C++ 源码
│   ├── main.cpp
│   ├── render.cpp
│   └── render_omp.cpp
├── include/                  # 头文件
│   └── render.hpp
├── output/                   # 输出图像
│   ├── frame_0001.png
│   └── ...
├── scripts/                  # Python 动画合成脚本
│   └── make_video.py
├── CMakeLists.txt
├── README.md
└── LICENSE
```

---

## 🧪 性能与测试计划

| 配置 | 分辨率 | 单核 | OpenMP 4 线程 | OpenMP 16 线程 |
|------|--------|-------|----------------|----------------|
| Intel i7-12800HX | 800×600 | 3.2s | 1.2s | 0.42s |
| AMD 5950X | 1920×1080 | TBD | TBD | TBD |
| WSL Ubuntu 24.04 | 任意 | ✅ 测试中 | ✅ |

📊 Benchmark 将输出为 CSV + plot：

```csv
resolution,threads,time_ms
800x600,1,3200
800x600,4,1200
800x600,16,420
```

---

## 🖼️ 预览图/动画展示（更新中）

- ![example](output/frame_0100.png)
- ![zoom_gif](output/mandelbrot_zoom.gif)

---

## 📌 待办事项 TODO

- [x] CLI 渲染核心逻辑
- [x] OpenMP 简单加速验证
- [ ] 彩色映射（HSV 或 Jet 色谱）
- [ ] 输出自动命名帧文件
- [ ] Python 脚本批量合成 mp4
- [ ] 添加 Zoom Path 轨迹配置
- [ ] Web demo 尝试

---

## 🧠 技术关键词

`#C++` `#OpenMP` `#Fractal` `#Multithread` `#FFmpeg`  
`#ImageRendering` `#NumericalPrecision` `#ZoomAnimation`

---

## 🤝 项目贡献指南（可选）

- 欢迎提交 Pull Request 增加图像风格 / 算法优化
- 或者 fork 后加入其他分形（如 Julia Set）

---

## 📝 LICENSE

MIT License - 仅供学习使用。
