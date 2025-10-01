
#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"

#include <iostream>

SpriteRenderer  *Renderer;

//Keys() 将数组中的所有元素初始化为 0（即 GL_FALSE），如果没有初始化数组中的元素会包含随机值
//如果是vector则可以不初始化，因为vector会自动初始化所有成员为0
Game::Game(GLuint width, GLuint height) 
    : Width(width), Height(height), State(GAME_ACTIVE), Keys() {
    // 初始化其他成员
}

Game::~Game() {
    // 释放资源
}

void Game::Init() {
    // 初始化游戏资源
    std::cout << "Loading shader..." << std::endl;
    ResourceManager::LoadShader("../shader/sprites.vs", "../shader/sprites.fs", nullptr, "sprite");
    std::cout << "Shader loaded, ID: " << ResourceManager::GetShader("sprite").ID << std::endl;

    std::cout << "Loading texture..." << std::endl;
    ResourceManager::LoadTexture("../textures/awesomeface.png", GL_TRUE, "face");
    std::cout << "Texture loaded, ID: " << ResourceManager::GetTexture("face").ID << std::endl;

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0); //Use()返回引用类型
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    std::cout << "Game initialized successfully!" << std::endl;
}

void Game::Update(GLfloat dt) {
    // 更新游戏状态
}

void Game::ProcessInput(GLfloat dt) {
    // 处理输入
}

void Game::Render() {
    // 渲染游戏
    if (this->State == GAME_ACTIVE) {
        Renderer->DrawSprite(ResourceManager::GetTexture("face"), glm::vec2(200, 200), glm::vec2(100, 60), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    }
}