
#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <sys/stat.h>
#endif

#define STB_IMAGE_IMPLEMENTATION // 在一个源文件中定义这个宏以实现stb_image的函数
#include <stb_image.h>

// 定义静态成员变量
std::map<std::string, Shader>       ResourceManager::Shaders;
std::map<std::string, Texture>      ResourceManager::Textures;

Shader ResourceManager::LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name) {
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader &ResourceManager::GetShader(std::string name) {
    return Shaders[name];
}

Texture ResourceManager::LoadTexture(const GLchar *file, GLboolean alpha, std::string name) {
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture &ResourceManager::GetTexture(std::string name) {
    return Textures[name];
}

void ResourceManager::Clear() {
    // (Properly) delete all shaders
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID); //删除着色器程序
        /*不是删除着色器，着色器应该在绑定到着色器程序之后马上删除*/
    // (Properly) delete all textures
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

std::string ResourceManager::GetResourcePath(const std::string &relativePath) {
#ifdef __APPLE__
    // 获取可执行文件路径
    char execPath[1024];
    uint32_t size = sizeof(execPath);
    if (_NSGetExecutablePath(execPath, &size) == 0) {
        // 手动提取目录路径（不使用 dirname）
        std::string fullPath(execPath);
        size_t lastSlash = fullPath.find_last_of('/');
        std::string basePath = fullPath.substr(0, lastSlash);

        // 移除 relativePath 开头的 "../"
        std::string cleanPath = relativePath;
        if (cleanPath.substr(0, 3) == "../") {
            cleanPath = cleanPath.substr(3);
        }

        // 尝试 App Bundle 路径 (Contents/MacOS -> Contents/Resources)
        std::string appBundlePath = basePath + "/../Resources/" + cleanPath;
        struct stat sb;
        if (stat(appBundlePath.c_str(), &sb) == 0) {
            return appBundlePath;
        }

        // 尝试相对路径（用于 build/ 目录）
        std::string buildPath = basePath + "/" + relativePath;
        if (stat(buildPath.c_str(), &sb) == 0) {
            return buildPath;
        }
    }
#endif
    // 如果以上都失败，返回原始路径
    return relativePath;
}

Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile) {
    // 1. 从文件中读取顶点/片段/几何着色器的代码
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try {
        // 获取正确的资源路径
        std::string vPath = GetResourcePath(vShaderFile);
        std::string fPath = GetResourcePath(fShaderFile);

        // 打开文件
        std::ifstream vertexShaderFile(vPath);
        std::ifstream fragmentShaderFile(fPath);
        std::stringstream vShaderStream, fShaderStream;
        // 读取文件的缓冲内容到数据流中
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // 关闭文件处理器
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // 转换数据流到字符串
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // 如果有几何着色器路径，读取几何着色器
        if (gShaderFile != nullptr) {
            std::string gPath = GetResourcePath(gShaderFile);
            std::ifstream geometryShaderFile(gPath);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    } catch (std::exception e) {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }

    // 转换为GLchar指针
    const GLchar *vShaderCode = vertexCode.c_str();
    const GLchar *fShaderCode = fragmentCode.c_str();
    const GLchar *gShaderCode = geometryCode.c_str();
    // 2. 编译着色器
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

//参数alpha： 如果为true，表示图像包含透明度通道（如PNG格式），需要使用RGBA格式加载
//如果为false，表示图像不包含透明度通道（如JPEG格式），使用RGB格式加载
Texture ResourceManager::loadTextureFromFile(const GLchar *file, GLboolean alpha) {
    // 创建纹理对象
    Texture texture;
    if (alpha) {
        //将CPU和GPU的纹理格式都设置为包含Alpha通道的RGBA格式
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // 获取正确的资源路径
    std::string texturePath = GetResourcePath(file);

    // 加载图像
    int width, height, nrChannels;
    //nrChannels 用于输出图像的通道数（如RGB有3个通道，RGBA有4个通道）
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);// 0 表示保持原始通道数
    /*如果是RGB格式
    data[0] = 第1个像素的R值
    data[1] = 第1个像素的G值  
    data[2] = 第1个像素的B值
    data[3] = 第2个像素的R值
    data[4] = 第2个像素的G值
    data[5] = 第2个像素的B值
    ...
    */
    
    if (data) {
        // 生成纹理
        texture.Generate(width, height, data);
        // 释放图像内存
        stbi_image_free(data);
    } else {
        std::cout << "Failed to load texture: " << file << std::endl;
        stbi_image_free(data);
    }
    return texture;
}