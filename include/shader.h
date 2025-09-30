#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    GLuint ID;

    Shader(){}

    Shader &Use(); //&表示返回引用类型

    void Compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr);// 编译着色器

    //功能函数
    //第三个参数 useShader 指示是否在设置uniform前激活着色器
    //默认为 false，表示不自动激活
    //如果传入 true，则会先调用 this->Use() 激活着色器
    void SetFloat(const GLchar *name, GLfloat value, GLboolean useShader = false);
    void SetInteger(const GLchar *name, GLint value, GLboolean useShader = false);
    void SetVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false);
    void SetVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader = false);
    void SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
    void SetVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader = false);
    void SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
    void SetVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader = false);
    void SetMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader = false);

private:
    // Checks if compilation or linking failed and if so, print the error logs
    void checkCompileErrors(GLuint object, std::string type);
};

#endif
