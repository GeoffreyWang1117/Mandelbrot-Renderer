# 输出目录

这个目录用于存放渲染生成的图像和视频文件。

**项目作者**: Geoffrey Wang (with Claude AI assistance)  
**项目地址**: https://github.com/GeoffreyWang1117/Mandelbrot-Renderer

## 性能对比示例

本目录中的图像展示了不同实现方式的性能特点:

| 实现方式 | 分辨率 | 渲染时间 | 性能 | 加速比 |
|---------|--------|----------|------|--------|
| Web JavaScript | 800×600 | ~10s | 50K pixels/sec | 1x |
| CPU单线程 | 800×600 | 292ms | 1.6M pixels/sec | 32x |
| OpenMP并行 | 800×600 | 4ms | 120M pixels/sec | 2,400x |
| **CUDA GPU** | **4K** | **7ms** | **1.7B pixels/sec** | **34,000x** |

## 文件类型

- `*.ppm` - PPM格式图像 (可用ImageMagick转换为PNG)
- `*.png` - PNG格式图像 
- `*.mp4` - 生成的动画视频

## 示例命令

### 生成图像
```bash
# 基础渲染
../build/mandelbrot_cpu --width 800 --height 600 --output classic.ppm

# 高质量渲染  
../build/mandelbrot_cpu --width 1920 --height 1080 --iter 2000 --output hd.ppm

# 局部放大
../build/mandelbrot_cpu --xmin -0.8 --xmax -0.7 --ymin 0.0 --ymax 0.1 --output zoom.ppm
```

### 生成动画
```bash
# 缩放动画
python3 ../scripts/make_video.py --type zoom --frames 60 --output zoom.mp4

# 区域扫描
python3 ../scripts/make_video.py --type scan --frames 120 --output scan.mp4
```

### 格式转换
```bash
# PPM转PNG
convert image.ppm image.png

# 批量转换
for f in *.ppm; do convert "$f" "${f%.ppm}.png"; done
```

## 注意事项

- PPM文件通常较大，建议转换为PNG以节省空间
- 高分辨率渲染会生成大文件，注意磁盘空间
- 临时动画帧存储在 `temp_frames/` 子目录中，可安全删除
