# Breakout Game

使用 OpenGL 3.3 开发的 Breakout（打砖块）游戏。

## 下载安装

### 从 GitHub Actions 下载

1. 进入 [GitHub Actions](../../actions) 页面
2. 选择最新的成功构建
3. 在页面底部的 **Artifacts** 区域下载：
   - `breakout-dmg.zip` - DMG 安装包
   - `breakout-app.zip` - App Bundle

### 安装方法

#### 方法一：使用 DMG（推荐）

1. 下载并解压 `breakout-dmg.zip`，得到 `Breakout.dmg`
2. 双击打开 `Breakout.dmg`
3. 将 `Breakout` 拖拽到 `Applications` 文件夹
4. 从启动台或 Applications 文件夹运行应用

#### 方法二：直接使用 App Bundle

1. 下载并解压 `breakout-app.zip`，得到 `Breakout.app`
2. 将 `Breakout.app` 移动到 `Applications` 文件夹（可选）
3. 右键点击应用，选择"打开"

### 首次打开提示"已损坏"的解决方法

由于应用未经过 Apple 公证，macOS Gatekeeper 可能会阻止运行。请按以下步骤操作：

**方法一：通过右键打开（推荐）**
1. 右键点击 `Breakout.app`
2. 选择"打开"
3. 在弹出的对话框中点击"打开"

**方法二：移除隔离属性**
打开终端，执行以下命令：
```bash
xattr -cr /Applications/Breakout.app
```

**方法三：临时禁用 Gatekeeper（不推荐）**
```bash
sudo spctl --master-disable
```
打开应用后，记得重新启用：
```bash
sudo spctl --master-enable
```

## 本地构建

### 依赖要求

- macOS 10.13+
- CMake 3.10+
- Homebrew

### 安装依赖

```bash
brew install glfw glm pkg-config
```

### 构建步骤

```bash
# 克隆仓库
git clone <repository-url>
cd Breakout

# 初始构建
mkdir -p build && cd build
cmake ..
make

# 运行游戏（必须从 build 目录运行）
./Breakout
```

### 重新构建

```bash
cd build
make
```

### 完全清理重新构建

当添加新文件或遇到链接错误时：
```bash
cd build && rm -rf * && cmake .. && make
```

## 游戏玩法

- **左/右方向键** 或 **A/D**: 移动挡板
- **空格键**: 发射球（游戏开始时）
- **ESC**: 退出游戏

## 技术栈

- **OpenGL 3.3+**: 图形渲染
- **GLFW3**: 窗口管理和输入处理
- **GLM**: 数学库
- **GLAD**: OpenGL 加载器
- **stb_image**: 图片加载

## 项目结构

```
Breakout/
├── include/          # 头文件
├── src/              # 源文件
├── shader/           # 着色器文件
├── textures/         # 纹理资源
├── game_level/       # 关卡文件
├── ThirdParty/       # 第三方库
│   ├── glad/
│   └── stb/
└── CMakeLists.txt    # CMake 配置
```

## 许可证

[根据你的项目添加许可证信息]
