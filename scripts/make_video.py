#!/usr/bin/env python3
"""
Mandelbrot 动画视频生成脚本

功能:
1. 批量调用C++渲染器生成帧序列
2. 使用FFmpeg合成视频  
3. 支持缩放动画、移动动画、参数变化动画

使用示例:
python scripts/make_video.py --type zoom --frames 120 --output mandelbrot_zoom.mp4
python scripts/make_video.py --type scan --frames 200 --resolution 1920x1080

依赖:
# Python依赖 (都是标准库，无需额外安装)
# - os, sys, subprocess, argparse, pathlib, json, time, math

# 外部工具依赖
apt install ffmpeg imagemagick    # Ubuntu/Debian
# 或
brew install ffmpeg imagemagick  # macOS  
# 或
pacman -S ffmpeg imagemagick     # Arch Linux

注意: 本脚本已移除numpy依赖，仅使用Python标准库

作者: Geoffrey Wang (with Claude AI assistance)
日期: 2025-08-12
"""

import os
import sys
import subprocess
import argparse
# numpy 只是用于数学计算，可以用标准库替代
# import numpy as np  
from pathlib import Path
import json
import time
import math

class MandelbrotVideoMaker:
    def __init__(self, executable_path=None):
        # 智能查找可执行文件路径
        if executable_path is None:
            possible_paths = [
                "./build/mandelbrot_cpu",      # 从项目根目录运行
                "../build/mandelbrot_cpu",     # 从scripts目录运行
                "./mandelbrot_cpu",            # build目录中直接运行
                "build/mandelbrot_cpu",        # 当前目录的build子目录
            ]
            
            for path in possible_paths:
                if Path(path).exists():
                    executable_path = path
                    break
            else:
                # 如果都找不到，使用默认路径
                executable_path = "./build/mandelbrot_cpu"
        
        self.executable = executable_path
        self.output_dir = Path("output")
        self.temp_dir = self.output_dir / "temp_frames"
        self.temp_dir.mkdir(parents=True, exist_ok=True)
        
    def clean_temp_files(self):
        """清理临时文件"""
        if self.temp_dir.exists():
            for file in self.temp_dir.glob("*.ppm"):
                file.unlink()
            for file in self.temp_dir.glob("*.png"):
                file.unlink()
                
    def render_frame(self, frame_num, width, height, max_iter, x_min, x_max, y_min, y_max):
        """渲染单帧图像"""
        frame_filename = self.temp_dir / f"frame_{frame_num:06d}.ppm"
        
        cmd = [
            self.executable,
            "--width", str(width),
            "--height", str(height), 
            "--iter", str(max_iter),
            "--xmin", str(x_min),
            "--xmax", str(x_max),
            "--ymin", str(y_min),
            "--ymax", str(y_max),
            "--output", str(frame_filename)
        ]
        
        print(f"[渲染] 第 {frame_num+1} 帧: {frame_filename.name}")
        
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=300)
            if result.returncode != 0:
                print(f"[错误] 渲染失败: {result.stderr}")
                return False
        except subprocess.TimeoutExpired:
            print(f"[错误] 渲染超时: 第 {frame_num+1} 帧")
            return False
            
        return frame_filename.exists()
    
    def convert_ppm_to_png(self, ppm_file, png_file):
        """将PPM转换为PNG格式"""
        cmd = ["convert", str(ppm_file), str(png_file)]
        try:
            subprocess.run(cmd, check=True, capture_output=True)
            return True
        except (subprocess.CalledProcessError, FileNotFoundError):
            print("[警告] ImageMagick未安装，跳过PNG转换")
            return False
    
    def create_zoom_animation(self, frames, width, height, max_iter, center_x, center_y, 
                            start_zoom, end_zoom, output_file):
        """创建缩放动画"""
        print(f"\n🎬 创建缩放动画: {frames} 帧")
        print(f"📍 中心点: ({center_x}, {center_y})")
        print(f"🔍 缩放: {start_zoom}x → {end_zoom}x")
        
        # 计算初始视口大小
        initial_width = 3.0 / start_zoom
        initial_height = 2.4 / start_zoom
        
        frame_files = []
        
        for frame in range(frames):
            progress = frame / (frames - 1)
            
            # 指数缩放插值
            current_zoom = start_zoom * (end_zoom / start_zoom) ** progress
            
            # 当前视口大小
            view_width = 3.0 / current_zoom
            view_height = 2.4 / current_zoom
            
            # 计算边界
            x_min = center_x - view_width / 2
            x_max = center_x + view_width / 2
            y_min = center_y - view_height / 2  
            y_max = center_y + view_height / 2
            
            # 渲染帧
            if self.render_frame(frame, width, height, max_iter, x_min, x_max, y_min, y_max):
                frame_files.append(self.temp_dir / f"frame_{frame:06d}.ppm")
            else:
                print(f"[错误] 第 {frame+1} 帧渲染失败")
                return False
                
        return self.create_video_from_frames(frame_files, output_file)
    
    def create_scan_animation(self, frames, width, height, max_iter, output_file):
        """创建扫描动画 (遍历有趣区域)"""
        print(f"\n🎬 创建扫描动画: {frames} 帧")
        
        # 有趣的Mandelbrot区域
        interesting_regions = [
            {"center": (-0.7269, 0.1889), "zoom": 100, "name": "螺旋区域"},
            {"center": (-0.8, 0.156), "zoom": 200, "name": "闪电区域"}, 
            {"center": (-0.16, 1.0405), "zoom": 150, "name": "海马谷"},
            {"center": (-1.25066, 0.02012), "zoom": 300, "name": "小岛"},
            {"center": (-0.749, 0.1102), "zoom": 400, "name": "触角区域"},
            {"center": (0.3, 0.5), "zoom": 80, "name": "右侧分支"},
        ]
        
        frames_per_region = frames // len(interesting_regions)
        frame_files = []
        
        for i, region in enumerate(interesting_regions):
            print(f"🔍 渲染区域 {i+1}/{len(interesting_regions)}: {region['name']}")
            
            for j in range(frames_per_region):
                frame_num = i * frames_per_region + j
                
                # 视口大小
                view_width = 3.0 / region["zoom"]
                view_height = 2.4 / region["zoom"]
                
                x_min = region["center"][0] - view_width / 2
                x_max = region["center"][0] + view_width / 2  
                y_min = region["center"][1] - view_height / 2
                y_max = region["center"][1] + view_height / 2
                
                if self.render_frame(frame_num, width, height, max_iter, x_min, x_max, y_min, y_max):
                    frame_files.append(self.temp_dir / f"frame_{frame_num:06d}.ppm")
                    
        return self.create_video_from_frames(frame_files, output_file)
    
    def create_video_from_frames(self, frame_files, output_file, fps=30):
        """使用FFmpeg合成视频"""
        if not frame_files:
            print("[错误] 没有可用的帧文件")
            return False
            
        print(f"\n🎞️  合成视频: {len(frame_files)} 帧 @ {fps} fps")
        print(f"📁 输出文件: {output_file}")
        
        # FFmpeg命令
        input_pattern = str(self.temp_dir / "frame_%06d.ppm")
        cmd = [
            "ffmpeg", "-y",  # 覆盖已存在文件
            "-r", str(fps),  # 帧率
            "-i", input_pattern,  # 输入模式
            "-c:v", "libx264",  # 视频编码器
            "-pix_fmt", "yuv420p",  # 像素格式
            "-crf", "18",  # 质量 (更低=更高质量)
            "-preset", "slow",  # 编码预设
            str(self.output_dir / output_file)
        ]
        
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=600)
            if result.returncode == 0:
                print("✅ 视频合成完成!")
                print(f"📄 输出: {self.output_dir / output_file}")
                
                # 显示文件大小
                video_file = self.output_dir / output_file
                if video_file.exists():
                    size_mb = video_file.stat().st_size / (1024 * 1024)
                    print(f"📊 文件大小: {size_mb:.1f} MB")
                return True
            else:
                print(f"[错误] FFmpeg失败: {result.stderr}")
                return False
        except subprocess.TimeoutExpired:
            print("[错误] 视频合成超时")
            return False
        except FileNotFoundError:
            print("[错误] FFmpeg未安装，请先安装: apt install ffmpeg")
            return False

def main():
    parser = argparse.ArgumentParser(description='Mandelbrot动画视频生成器')
    parser.add_argument('--type', choices=['zoom', 'scan'], default='zoom',
                       help='动画类型: zoom(缩放) 或 scan(扫描)')
    parser.add_argument('--frames', type=int, default=120,
                       help='总帧数 (默认: 120)')
    parser.add_argument('--resolution', default='800x600',
                       help='分辨率 WxH (默认: 800x600)')
    parser.add_argument('--max-iter', type=int, default=1000,
                       help='最大迭代次数 (默认: 1000)')
    parser.add_argument('--fps', type=int, default=30,
                       help='视频帧率 (默认: 30)')
    parser.add_argument('--output', default='mandelbrot_animation.mp4',
                       help='输出视频文件名')
    parser.add_argument('--executable', default='./build/mandelbrot_cpu',
                       help='渲染器可执行文件路径')
    parser.add_argument('--clean', action='store_true',
                       help='仅清理临时文件')
    
    # 缩放动画专用参数
    parser.add_argument('--center', default='-0.7269,0.1889',
                       help='缩放中心 x,y (默认: -0.7269,0.1889)')
    parser.add_argument('--start-zoom', type=float, default=1,
                       help='起始缩放倍数 (默认: 1)')
    parser.add_argument('--end-zoom', type=float, default=1000,
                       help='结束缩放倍数 (默认: 1000)')
    
    args = parser.parse_args()
    
    # 解析分辨率
    try:
        width, height = map(int, args.resolution.split('x'))
    except ValueError:
        print("[错误] 分辨率格式错误，应为 WIDTHxHEIGHT")
        return 1
    
    # 解析中心点
    try:
        center_x, center_y = map(float, args.center.split(','))
    except ValueError:
        print("[错误] 中心点格式错误，应为 x,y")
        return 1
    
    # 创建视频生成器
    video_maker = MandelbrotVideoMaker(args.executable)
    
    if args.clean:
        print("🧹 清理临时文件...")
        video_maker.clean_temp_files()
        return 0
    
    # 检查可执行文件
    if not Path(args.executable).exists():
        print(f"[错误] 找不到渲染器: {args.executable}")
        print("请先编译项目: mkdir build && cd build && cmake .. && make")
        return 1
    
    print("🌀 Mandelbrot 动画生成器")
    print(f"🎬 动画类型: {args.type}")
    print(f"📐 分辨率: {width}x{height}")
    print(f"🎞️  总帧数: {args.frames}")
    print(f"⚡ 最大迭代: {args.max_iter}")
    
    start_time = time.time()
    
    try:
        if args.type == 'zoom':
            success = video_maker.create_zoom_animation(
                args.frames, width, height, args.max_iter,
                center_x, center_y, args.start_zoom, args.end_zoom, args.output
            )
        elif args.type == 'scan':
            success = video_maker.create_scan_animation(
                args.frames, width, height, args.max_iter, args.output  
            )
        else:
            print(f"[错误] 未知动画类型: {args.type}")
            return 1
            
        elapsed_time = time.time() - start_time
        
        if success:
            print(f"\n🎉 动画生成完成!")
            print(f"⏱️  总耗时: {elapsed_time:.1f} 秒")
            print(f"📊 平均每帧: {elapsed_time/args.frames:.2f} 秒")
            
            # 可选清理
            clean_input = input("\n🧹 是否清理临时帧文件? (y/N): ").lower()
            if clean_input == 'y':
                video_maker.clean_temp_files()
                print("✅ 临时文件已清理")
        else:
            print("\n❌ 动画生成失败!")
            return 1
            
    except KeyboardInterrupt:
        print("\n\n⏹️  用户中断")
        video_maker.clean_temp_files()
        return 1
    except Exception as e:
        print(f"\n❌ 意外错误: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
