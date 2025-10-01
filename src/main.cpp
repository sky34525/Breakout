
#include "game.h"
#include "resource_manager.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 600;

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

//argc 和 argv 用于处理命令行参数
//argc 表示参数的数量，argv 是一个字符串数组，包含所有参数
//例如，运行程序时可以传入参数来设置游戏的不同选项
//./Breakout --fullscreen --level 5 --debug
//argc 的值将是 4，argv 将包含 ["./Breakout", "--fullscreen", "--level", "5", "--debug"]
int main(int argc, char* argv[]) {
    glfwInit();
    //openGL 版本设置为3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //使用核心模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //前向兼容性（macOS 必需）
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
    glfwMakeContextCurrent(window);

        // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); //设置视口，告诉OpenGL渲染窗口的大小
    /*视口的作用是将窗口的坐标转换为OpenGL的坐标系统，确保渲染的内容能够正确显示在窗口中*/
    glEnable(GL_CULL_FACE); //启用面剔除
    glEnable(GL_BLEND); //启用混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //设置混合函数

    Breakout.Init();

    // deltaTime 的核心作用就是让游戏逻辑基于真实时间而不是帧率
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;
    
    Breakout.State = GAME_ACTIVE; //游戏开始时状态为活动状态

    while(!glfwWindowShouldClose(window)) {
        // 计算 deltaTime
        GLfloat currentFrame = glfwGetTime(); //获取当前时间，单位为秒
        deltaTime = currentFrame - lastFrame; //计算两帧之间的时间差
        lastFrame = currentFrame; //更新上一帧的时间

        // 处理事件
        glfwPollEvents();
        Breakout.ProcessInput(deltaTime);

        // 更新游戏状态
        Breakout.Update(deltaTime);

        // 渲染
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //设置清屏颜色为黑色
        glClear(GL_COLOR_BUFFER_BIT); //清除颜色缓冲区

        Breakout.Render();

        glfwSwapBuffers(window); //交换前后缓冲区，显示渲染结果
    }

    ResourceManager::Clear(); //清除所有资源
    glfwTerminate(); //终止GLFW，释放资源
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    // 当按下ESC键时，设置窗口关闭标志
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            Breakout.Keys[key] = GL_TRUE; //按键被按下
        else if (action == GLFW_RELEASE)
            Breakout.Keys[key] = GL_FALSE; //按键被释放
    }
}