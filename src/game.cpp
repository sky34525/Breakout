
#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "ball_object.h"

#include <iostream>

SpriteRenderer  *Renderer;

const glm::vec2 PLAYER_SIZE(100, 20); //玩家对象的尺寸
const GLfloat PLAYER_VELOCITY(500.0f); //玩家对象的移动速度
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f); //初始球的速度
const GLfloat BALL_RADIUS = 12.5f; //球的半径

GameObject   *Player; //玩家对象

BallObject  *Ball; //球对象

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
    ResourceManager::LoadShader("../shader/sprites.vs", "../shader/sprites.fs", nullptr, "sprite");

    // Load textures
    ResourceManager::LoadTexture("../textures/awesomeface.png", GL_TRUE, "face");
    ResourceManager::LoadTexture("../textures/background.jpg", GL_FALSE, "background");
    ResourceManager::LoadTexture("../textures/block.png", GL_FALSE, "block");
    ResourceManager::LoadTexture("../textures/block_solid.png", GL_FALSE, "block_solid");
    ResourceManager::LoadTexture("../textures/paddle.png", GL_TRUE, "paddle");

    // 加载关卡
    GameLevel one; one.Load("../game_level/one.lvl", this->Width, this->Height * 0.5);
    GameLevel two; two.Load("../game_level/two.lvl", this->Width, this->Height * 0.5);
    GameLevel three; three.Load("../game_level/three.lvl", this->Width, this->Height * 0.5);
    GameLevel four; four.Load("../game_level/four.lvl", this->Width, this->Height * 0.5);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;

    // 加载玩家挡板
    glm::vec2 playerPos = glm::vec2(
        this->Width / 2 - PLAYER_SIZE.x / 2, //水平居中
        this->Height - PLAYER_SIZE.y * 2      //距离底部两个挡板高度的位置
    );
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    // 加载球
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2); //球初始位置在挡板上方
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

    // 配置渲染器
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f); //正交投影矩阵
    //此处实现了NDC映射反转
    //将y轴反转，使得y轴向下递增
    //虽然opengl的NDC坐标系中y轴是向上的
    //但是2D游戏中通常希望y轴是向下递增的
    //这样更符合大多数2D游戏的坐标系统
    //例如屏幕左上角是(0,0)，右下角是(Width, Height)
    //这样设置后，屏幕左上角的坐标是 (0, 0)，右下角是 (Width, Height)
    /*
    参数解释：
    left: 0.0f，表示视口的左边界对应的世界坐标
    right: static_cast<GLfloat>(this->Width)，表示视口的右边界对应的世界坐标
    bottom: static_cast<GLfloat>(this->Height)，表示视口的下边界对应的世界坐标
    top: 0.0f，表示视口的上边界对应的世界坐标
    near: -1.0f，表示视口的近裁剪面对应的深度值
    far: 1.0f，表示视口的远裁剪面对应的深度值
    这样设置后，屏幕左上角的坐标是 (0, 0)，右下角是 (Width, Height)
    Y轴向下递增，符合计算机图形学的习惯
    适合2D游戏开发
    */
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0); //Use()返回引用类型
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
}

void Game::Update(GLfloat dt) {
    // 更新游戏状态
    Ball->Move(dt, this->Width); //每帧都需要更新球的位置
}

void Game::ProcessInput(GLfloat dt) {
    // 处理输入
    if(this->State == GAME_ACTIVE) {
        GLfloat velocity = PLAYER_VELOCITY * dt; //根据时间增量调整速度
        // 移动玩家挡板
        if (this->Keys[GLFW_KEY_A]) {
            if (Player->Position.x >= 0) {
                Player->Position.x -= velocity;
                if(Ball->Stuck) //如果球卡在挡板上，移动挡板时也移动球
                    Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D]) {
            if (Player->Position.x <= this->Width - Player->Size.x) {
                Player->Position.x += velocity;
                if(Ball->Stuck) //如果球卡在挡板上，移动挡板时也移动球
                    Ball->Position.x += velocity;
            }
        }
        if(this->Keys[GLFW_KEY_SPACE]) {
            Ball->Stuck = false; //按空格键发射球
        }
    }
}

void Game::Render() {
    // 渲染游戏
    if (this->State == GAME_ACTIVE) {
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f);
        this->Levels[this->Level].Draw(*Renderer);
        Player->Draw(*Renderer);
        Ball->Draw(*Renderer);
    }
}