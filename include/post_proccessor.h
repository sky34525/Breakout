#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "shader.h"
#include "shader.h"

//无论我们需要什么，我们只需要将需要的PostProcessor类中的特效属性设置为true，其效果就可以立即可见。

// 后处理器承载了Breakout的所有后处理效果

// 游戏在纹理四边形上渲染游戏后，可以通过启用混淆、混沌或

// 摇晃布尔值来启用特定效果。

// 在渲染游戏之前必须调用BeginRender()，在渲染游戏之后调用EndRender()，以便类正常工作。

class PostProcessor
{
public:
    // 状态
    Shader PostProcessingShader; //后处理着色器
    Texture Texture; //渲染纹理
    GLuint Width, Height; //屏幕宽高
    // 效果
    GLboolean Confuse, Chaos, Shake; //混淆、混沌、摇晃
    // 构造器
    PostProcessor(Shader shader, GLuint width, GLuint height);
    // 预处理所有后处理效果
    void BeginRender();
    // 后处理所有后处理效果
    void EndRender();
    // 渲染纹理四边形
    void Render(GLfloat time);
private:
    // 初始化渲染数据并配置纹理和帧缓冲
    void initRenderData();
    GLuint MSFBO, FBO; //多重采样帧缓冲和帧缓冲对象
    GLuint RBO; //作为帧缓冲附件,存储渲染结果(深度、模板等) 
    GLuint VAO; //顶点数组对象
};

#endif