#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// 游戏状态
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

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
};

#endif