#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "texture.h"
#include "shader.h"

#include <map>
#include <string>
#include <glad/glad.h>

class ResourceManager {
public:
    // Resource storage
    //声明静态变量，定义在cpp文件中
    static std::map<std::string, Shader> Shaders;
    static std::map<std::string, Texture> Textures;
    // Loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code.
    static Shader LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name);
    // Retrieves a stored shader
    static Shader &GetShader(std::string name);
    // Loads (and generates) a texture from file
    static Texture LoadTexture(const GLchar *file, GLboolean alpha, std::string name);
    // Retrieves a stored texture
    static Texture &GetTexture(std::string name);
    // Properly de-allocates all loaded resources
    static void Clear();
    // Get the correct resource path (supports both build/ and App Bundle)
    static std::string GetResourcePath(const std::string &relativePath);
private:
    ResourceManager() {
        // Constructor is private to prevent instantiation
    }
    // Loads and generates a shader from file
    static Shader loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
    // Loads a single texture from file
    static Texture loadTextureFromFile(const GLchar *file, GLboolean alpha);
};  


#endif