
# PhotoEditor - Qt 2D 图像编辑器

基于 Qt 6 开发的桌面版 2D 图像编辑器，功能类似美图秀秀，适用于 Windows 平台。

## 功能特性

### 文件操作
- **新建**：创建指定尺寸的空白画布
- **打开**：支持 PNG、JPEG、BMP、GIF、WebP 等格式
- **保存/另存为**：导出为 PNG、JPEG、BMP 格式
- **打印**：打印当前图像

### 编辑工具
- **选择**：默认工具
- **裁剪**：拖拽选择区域后，点击“裁剪”菜单应用
- **画笔**：自由绘画，可调大小和颜色
- **橡皮擦**：擦除图像内容，可调大小
- **文字**：点击添加文字，支持字体和颜色选择
- **取色器**：点击图像获取颜色并设为画笔颜色

### 图像调整
- **亮度**：0-200%
- **对比度**：0-200%
- **饱和度**：0-200%

### 滤镜效果
- 灰度、怀旧/复古、黑白、暖色、冷色
- 锐化、模糊、浮雕、反相

### 图像变换
- 向左/右旋转 90°
- 水平/垂直翻转
- 调整图像尺寸

### 其他
- **撤销/重做**：最多 50 步
- **复制/粘贴**：与剪贴板互操作
- **缩放**：Ctrl+滚轮 或 工具栏按钮，支持适应窗口

## 环境要求

- Windows 10/11
- CMake 3.16+
- Qt 6（Core, Gui, Widgets, PrintSupport）
- C++17 编译器（MSVC、MinGW 或 GCC）

## 构建步骤

### 1. 安装 Qt 6

从 [Qt 官网](https://www.qt.io/download) 下载并安装 Qt 6，勾选 MSVC 或 MinGW 套件。

### 2. 配置 CMake

在项目根目录打开终端，执行：

```powershell
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"   # 替换为你的 Qt 安装路径
```

### 3. 编译

```powershell
cmake --build . --config Release
```

### 4. 运行

```powershell
.\Release\PhotoEditor.exe
```

或在 Qt Creator 中打开 `CMakeLists.txt` 直接运行。

## 项目结构

```
cursor_graphic_editor/
├── CMakeLists.txt
├── README.md
└── src/
    ├── main.cpp           # 程序入口
    ├── MainWindow.h/cpp   # 主窗口、菜单、工具栏
    ├── ImageCanvas.h/cpp  # 画布、绘图、编辑逻辑
    ├── AdjustmentPanel.h/cpp   # 亮度/对比度/饱和度面板
    ├── FilterPanel.h/cpp      # 滤镜选择面板
    ├── ToolOptionsPanel.h/cpp # 画笔/橡皮擦选项
    └── ImageProcessor.h/cpp   # 图像处理算法
```

## 快捷键

| 快捷键 | 功能 |
|--------|------|
| Ctrl+N | 新建 |
| Ctrl+O | 打开 |
| Ctrl+S | 保存 |
| Ctrl+Shift+S | 另存为 |
| Ctrl+Z | 撤销 |
| Ctrl+Y | 重做 |
| Ctrl+C | 复制 |
| Ctrl+V | 粘贴 |
| Ctrl+P | 打印 |
| Ctrl+Shift+C | 裁剪 |
| Ctrl+滚轮 | 缩放 |

## 许可证
