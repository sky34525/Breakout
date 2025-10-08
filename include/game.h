#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_level.h"
#include "particle_generator.h"
#include "post_proccessor.h"


// 游戏状态
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// 方向枚举
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision; // 碰撞结果

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game {
public:
    GameState State;
    /*GLboolean 是opengl API中定义的数据类型，用于表示布尔值
    Keys 数组用于存储每个按键的状态
    每个元素对应了一个按键
    GL_TRUE表示按键被按下，GL_FALSE表示按键未被按下
    */
    GLboolean Keys[1024];
    /*GLuint 是opengl API中定义的数据类型，用于表示无符号整数
    Width 和 Height 分别表示游戏窗口的宽度和高度
    */
    GLuint Width, Height;
    Game(GLuint width, GLuint height);
    ~Game();
    void Init();
    void ProcessInput(GLfloat dt);
    void Update(GLfloat dt);
    void Render();
    void DoCollisions(); //处理碰撞

private:
    std::vector<GameLevel> Levels;
    GLuint Level;

    Collision CheckCollision(BallObject &one, GameObject &two); // 检查两个游戏对象是否碰撞

    Direction VectorDirection(glm::vec2 target);

    void Dead(); // 处理玩家死亡
};

#endif