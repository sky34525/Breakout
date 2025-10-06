
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

ParticleGenerator   *Particles; //粒子生成器

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
    ResourceManager::LoadShader("../shader/particle.vs", "../shader/particle.fs", nullptr, "particle");

    // Load textures
    ResourceManager::LoadTexture("../textures/awesomeface.png", GL_TRUE, "face");
    ResourceManager::LoadTexture("../textures/glass_ball_RGBA.png", GL_TRUE, "glass_ball");
    ResourceManager::LoadTexture("../textures/background.jpg", GL_FALSE, "background");
    ResourceManager::LoadTexture("../textures/block.png", GL_FALSE, "block");
    ResourceManager::LoadTexture("../textures/block_solid.png", GL_FALSE, "block_solid");
    ResourceManager::LoadTexture("../textures/paddle.png", GL_TRUE, "paddle");
    ResourceManager::LoadTexture("../textures/particle.png", GL_TRUE, "particle");

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
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("glass_ball"));

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

    // 配置粒子着色器
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

    // 初始化粒子系统
    Particles = new ParticleGenerator(
        ResourceManager::GetShader("particle"),
        ResourceManager::GetTexture("particle"),
        500
    );
}

void Game::Update(GLfloat dt) {
    // 更新游戏状态
    Ball->Move(dt, this->Width); //每帧都需要更新球的位置
    this->DoCollisions(); //每帧都需要检查碰撞
    this->Dead(); //检查玩家是否死亡
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2)); // 每帧更新粒子生成器
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
        Particles->Draw(); // 在其他物体之后，球体之前渲染粒子，粒子就会在所有其他物体面前，但在球体之后
        Ball->Draw(*Renderer);
    }
}

void Game::DoCollisions() {
   for (GameObject &box : this->Levels[this->Level].Bricks) {

        // 砖块碰撞处理
        if(!box.Destroyed) {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) { // 如果发生碰撞
                // 如果砖块不是坚固的，标记为已销毁
                if (!box.IsSolid)
                    box.Destroyed = GL_TRUE;
                // 碰撞后反转球的速度
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT) { // 水平碰撞
                    Ball->Velocity.x = -Ball->Velocity.x; // 反转水平速度
                    // 重定位
                    GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        Ball->Position.x += penetration; // 防止球卡在砖块内
                    else
                        Ball->Position.x -= penetration; // 防止球卡在砖块内
                } else { // 垂直碰撞
                    Ball->Velocity.y = -Ball->Velocity.y; // 反转垂直速度
                    // 重定位
                    GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        Ball->Position.y -= penetration; // 防止球卡在砖块内
                    else
                        Ball->Position.y += penetration; // 防止球卡在砖块内
                }
            }
        }
    }


    // 挡板碰撞处理
    Collision result = CheckCollision(*Ball, *Player);
    if (std::get<0>(result) && !Ball->Stuck) { // 如果发生碰撞
            // 反转垂直速度
            Ball->Velocity.y = -1 * abs(Ball->Velocity.y);  // 确保球向上反弹

            // 根据球与挡板的碰撞位置调整水平速度
            GLfloat centerBoard = Player->Position.x + Player->Size.x / 2; // 挡板中心x坐标
            GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard; // 球心与挡板中心的水平距离
            GLfloat percentage = distance / (Player->Size.x / 2); // 归一化到 [-1, 1]
            // 根据碰撞位置调整球的水平速度
            GLfloat strength = 2.0f; // 调整这个值以改变反弹的灵敏度
            glm::vec2 oldVelocity = Ball->Velocity;
            Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength; // 调整水平速度
            Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); // 保持速度不变，只改变方向
    }
    
};
/*将球视为方形来计算碰撞
Collision Game::CheckCollision(BallObject &one, GameObject &two) {
    // AABB - AABB collision
    // 检查两个AABB（轴对齐边界框）是否碰撞
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
                      two.Position.x + two.Size.x >= one.Position.x;
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
                      two.Position.y + two.Size.y >= one.Position.y;
    return collisionX && collisionY;
}
*/

// 更精确的圆形和AABB碰撞检测
Collision Game::CheckCollision(BallObject &one, GameObject &two) {
    // 获取球的中心点
    glm::vec2 center(one.Position + one.Radius);
    // 计算AABB的半尺寸和中心点
    glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
    glm::vec2 aabb_center(two.Position + aabb_half_extents);
    // 计算两者之间的差值
    glm::vec2 difference = center - aabb_center;
    /*clamped 是将 difference 的 x 和 y 分量分别限制在 [-半宽, +半宽] 和 [-半高, +半高] 范围内，得到的结果表示从矩形中心到矩形边界上最近点的向量。*/
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

    // 获取最近点
    glm::vec2 closest = aabb_center + clamped;

    // 计算最近点与球心的距离
    glm::vec2 closest_distance = closest - center;
    // 如果最近点与球心的距离小于球的半径，则发生碰撞
    if (glm::length(closest_distance) < one.Radius) {
        // 碰撞发生，计算碰撞方向
        Direction dir = VectorDirection(closest_distance);
        return std::make_tuple(GL_TRUE, dir, closest_distance);
    } else {
        return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
    }
}

Direction Game::VectorDirection(glm::vec2 target) {
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),  // up
        glm::vec2(1.0f, 0.0f),  // right
        glm::vec2(0.0f, -1.0f), // down
        glm::vec2(-1.0f, 0.0f)  // left
    };
    GLfloat max = 0.0f;
    GLuint best_match = -1;
    // 遍历方向向量,找到与目标向量夹角最小的方向即是碰撞方向
    for (GLuint i = 0; i < 4; i++) {
        GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max) {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

void Game::Dead() {
    if(Ball->Position.y >= Height) {
        State = GAME_MENU;
    }
}