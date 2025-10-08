/*
通过两个帧缓冲区实现后期处理
一个用于多重采样，另一个用于后期处理
多重采样帧缓冲用于抗锯齿渲染
后期处理帧缓冲用于应用各种视觉效果
最后将后期处理帧缓冲的内容渲染到屏幕（一个覆盖屏幕的4边形）上
*/

#include "post_proccessor.h"

#include <iostream>

PostProcessor::PostProcessor(Shader shader, GLuint width,GLuint height)
    : PostProcessingShader(shader), Texture(), Width(width), Height(height), Confuse(GL_FALSE), Chaos(GL_FALSE), Shake(GL_FALSE){
    // 初始化渲染缓冲区/帧缓冲区对象
    /*两个帧缓冲区对象，一个用于多重采样，另一个用于后期处理*/
    glGenFramebuffers(1, &this->MSFBO);
    glGenFramebuffers(1, &this->FBO);
    glGenRenderbuffers(1, &this->RBO);

    //初始化渲染缓冲区存储，使用多采样颜色缓冲区（不需要深度/模板缓冲区）
    glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);

    GLint maxSamples;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    GLint samples = maxSamples > 4 ? 4 : maxSamples;

    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGB8, width, height); //为多重采样渲染缓冲区分配存储空间
    /*
    参数：
    GL_RENDERBUFFER: 指定目标是渲染缓冲区对象
    samples: 采样数（动态查询硬件最大支持值，通常为4）
    GL_RGB8: 指定渲染缓冲区的内部格式（8位RGB，macOS兼容）
    width: 渲染缓冲区的宽度
    height: 渲染缓冲区的高度
    这个函数会为渲染缓冲区对象分配存储空间，以支持多重采样
    通过使用多重采样，可以显著提高渲染质量，减少锯齿和其他视觉伪影
    注意：macOS OpenGL 通常最大支持 4x MSAA，超过会导致帧缓冲初始化失败
    */
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO); //将渲染缓冲区附加到帧缓冲对象
    /*
    参数：
    GL_FRAMEBUFFER: 指定目标是帧缓冲对象
    GL_COLOR_ATTACHMENT0: 指定附加点，这里是第一个颜色附加点
    GL_RENDERBUFFER: 指定附加的是渲染缓冲区对象
    this->RBO: 要附加的渲染缓冲区对象的ID
    这个函数将渲染缓冲区对象附加到帧缓冲对象的指定附加点
    这样，渲染到这个帧缓冲对象时，颜色数据将存储在渲染缓冲区中
    */
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

    //同时初始化FBO/纹理以将多采样颜色缓冲区粘贴到；用于着色器操作（用于后期处理效果）
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    this->Texture.Generate(width, height, nullptr); //创建一个空纹理
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Texture.ID, 0); //将纹理附加到帧缓冲对象
    /*
    参数：
    GL_FRAMEBUFFER: 指定目标是帧缓冲对象
    GL_COLOR_ATTACHMENT0: 指定附加点，这里是第一个颜色附加点
    GL_TEXTURE_2D: 指定附加的是2D纹理对象
    this->Texture.ID: 要附加的纹理对象的ID
    0: 指定纹理的层级，这里是0，表示基础层级
    这个函数将纹理对象附加到帧缓冲对象的指定附加点
    这样，渲染到这个帧缓冲对象时，颜色数据将存储在纹理中
    */
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->initRenderData();
    this->PostProcessingShader.SetInteger("scene", 0, GL_TRUE); //设置纹理单元索引
    GLfloat offset = 1.0f / 300.0f;
    GLfloat offsets[9][2] = {
        {-offset,  offset},  // 左上
        { 0.0f,    offset},  // 上
        { offset,  offset},  // 右上
        {-offset,  0.0f},    // 左
        { 0.0f,    0.0f},    // 中                  
        { offset,  0.0f},    // 右
        {-offset, -offset},  // 左下
        { 0.0f,   -offset},  // 下
        { offset, -offset}   // 右下
    };
    glUniform2fv(glGetUniformLocation(this->PostProcessingShader.ID, "offset"), 9, (GLfloat*)offsets);

    //边缘检测卷积核，用于在 chaos 模式下提取图像边缘，产生扭曲的边缘轮廓效果
    //数组中的值表示卷积核的权重
    //中心值为8，周围值为-1，强调中心像素，抑制邻近像素
    //这种卷积核有助于突出图像的边缘，产生明显的边缘检测效果
    GLint edge_kernel[9] = {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };
    glUniform1iv(glGetUniformLocation(this->PostProcessingShader.ID, "edge_kernel"), 9, edge_kernel);

    //模糊卷积核，用于在 shake 模式下对图像进行模糊处理
    //数组中的值表示卷积核的权重
    //中心值为4，周围值为1，强调中心像素，抑制邻近像素
    //这种卷积核有助于产生平滑的模糊效果
    GLfloat blur_kernel[9] = {
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    };
    glUniform1fv(glGetUniformLocation(this->PostProcessingShader.ID, "blur_kernel"), 9, blur_kernel);
}

void PostProcessor::initRenderData() {
    GLuint VBO;
    GLfloat vertices[] = {
        // 位置          // 纹理
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,

        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(this->VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void PostProcessor::BeginRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
//在渲染游戏之后调用
void PostProcessor::EndRender() {
    //现在需要将多重采样的颜色缓冲区解析到普通的颜色缓冲区
    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
    glBlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    /*
    参数：
    0, 0, this->Width, this->Height: 指定源矩形区域的左下角和右上角坐标
    0, 0,   this->Width, this->Height: 指定目标矩形区域的左下角和右上角坐标
    GL_COLOR_BUFFER_BIT: 指定要复制的缓冲区，这里是颜色缓冲区
    GL_NEAREST: 指定缩放时使用的过滤方式，这里是最近点过滤
    这个函数将源帧缓冲区的指定区域复制到目标帧缓冲区的指定区域
    通过这种方式，可以将多重采样的渲染结果解析到普通的帧缓冲区，以便后续处理
    */
    glBindFramebuffer(GL_FRAMEBUFFER, 0); //解绑帧缓冲区
}

void PostProcessor::Render(GLfloat time) {
    // 使用后处理着色器
    this->PostProcessingShader.Use();
    this->PostProcessingShader.SetFloat("time", time);
    this->PostProcessingShader.SetInteger("confuse", this->Confuse);
    this->PostProcessingShader.SetInteger("chaos", this->Chaos);
    this->PostProcessingShader.SetInteger("shake", this->Shake);
    // 绘制纹理四边形
    glActiveTexture(GL_TEXTURE0);
    this->Texture.Bind();
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}