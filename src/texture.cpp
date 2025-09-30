
#include "texture.h"

#include <iostream>

Texture::Texture()
    : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB),
      Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT),
      Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR) {
    glGenTextures(1, &this->ID); //生成一个纹理对象，并将其ID存储在成员变量ID中
}

void Texture::Generate(GLuint width, GLuint height, unsigned char* data) {
    this->Width = width;
    this->Height = height;
    // 创建纹理
    glBindTexture(GL_TEXTURE_2D, this->ID); //绑定纹理对象，后续的纹理操作都会作用于这个对象
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
    /*
    参数解释
    GL_TEXTURE_2D: 目标纹理类型，这里是二维纹理
    0: 纹理的mipmap级别，0表示基础级别
    this->Internal_Format: 纹理在GPU上的存储格式（如GL_RGB, GL_RGBA）
    width: 纹理的宽度
    height: 纹理的高度
    0: 边框宽度，必须为0
    this->Image_Format: 源图像的数据格式（如GL_RGB, GL_RGBA）
    GL_UNSIGNED_BYTE: 源图像的数据类型，这里是无符号字节
    data: 指向图像数据的指针
    这行代码将图像数据上传到GPU，创建了一个新的二维纹理
    纹理数据现在存储在GPU的显存中，可以高效地被着色器访问
    */
    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S); //设置S轴环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T); //设置T轴环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min); //设置缩小过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max); //设置放大过滤方式
    glGenerateMipmap(GL_TEXTURE_2D); //生成mipmap
    // 解绑纹理
    glBindTexture(GL_TEXTURE_2D, 0); //解绑纹理，防止后续操作影响当前纹理
}

void Texture::Bind() const {
    glBindTexture(GL_TEXTURE_2D, this->ID); //绑定纹理对象，使其成为当前活动的纹理
}