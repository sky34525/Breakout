#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

class Texture {
public:
    GLuint ID; // 纹理ID
    GLuint Width, Height; // 纹理的宽度和高度
    GLuint Internal_Format; // 纹理的内部格式（如 GL_RGB, GL_RGBA） GPU存储格式
    GLuint Image_Format; // 纹理的图像格式（如 GL_RGB, GL_RGBA） CPU存储格式
    /*
    T轴是曲直方向
    S轴是水平方向
    纹理坐标的范围一般是 [0.0, 1.0]
    超出这个范围的坐标会根据环绕方式进行处理
    主要的环绕方式有以下几种：
    GL_REPEAT: 重复纹理
    GL_MIRRORED_REPEAT: 镜像重复纹理
    GL_CLAMP_TO_EDGE: 拉伸边缘像素
    GL_CLAMP_TO_BORDER: 使用边界颜色
    选择合适的环绕方式可以避免纹理接缝问题
    */
    GLuint Wrap_S; // S轴的环绕方式（如 GL_REPEAT, GL_CLAMP_TO_EDGE）
    GLuint Wrap_T; // T轴的环绕方式（如 GL_REPEAT, GL_CLAMP_TO_EDGE）
    /*
    纹理过滤：
    纹理过滤决定了当纹理被缩小或放大时，如何计算像素的颜色值
    主要的过滤方式有以下几种：
    GL_NEAREST: 最近点过滤，选择最接近的纹理像素，可能导致锯齿状效果
    GL_LINEAR: 线性过滤，使用邻近的多个纹理像素进行插值，效果更平滑
    结合mipmap使用的过滤方式：
    GL_NEAREST_MIPMAP_NEAREST: 使用最接近的mipmap级别的最近点过滤
    GL_LINEAR_MIPMAP_NEAREST: 使用最接近的mipmap级别的线性过滤
    GL_NEAREST_MIPMAP_LINEAR: 使用两个最接近的mipmap级别的最近点过滤进行插值
    Mipmap是预先计算并存储的一系列纹理图像，分辨率逐级降低
    */
    GLuint Filter_Min; // 最小化过滤方式（如 GL_LINEAR, GL_NEAREST）
    GLuint Filter_Max; // 最大化过滤方式（如 GL_LINEAR, GL_NEAREST）
    Texture();
    // 生成纹理
    void Generate(GLuint width, GLuint height, unsigned char* data);
    // 绑定纹理
    void Bind() const;
};

#endif