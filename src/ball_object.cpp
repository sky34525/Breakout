
#include "ball_object.h"

BallObject::BallObject()
    : GameObject(), Radius(12.5f), Stuck(true) { }

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture sprite)
    : GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.0f), velocity), Radius(radius), Stuck(true) { }


//投影矩阵是渲染管线的一部分，游戏逻辑永远在世界坐标系下进行。
glm::vec2 BallObject::Move(GLfloat dt, GLuint window_width) {
    // 如果球卡在挡板上，直接返回当前位置
    if (!this->Stuck) {
        // 移动球的位置
        this->Position += this->Velocity * dt;
        // 检查球是否碰到窗口边界
        if (this->Position.x <= 0.0f) {
            this->Velocity.x = -this->Velocity.x; // 反转水平速度
            this->Position.x = 0.0f; // 防止球出界
        } else if (this->Position.x + this->Size.x >= window_width) {
            this->Velocity.x = -this->Velocity.x; // 反转水平速度
            this->Position.x = window_width - this->Size.x; // 防止球出界
        }
        if (this->Position.y <= 0.0f) {
            this->Velocity.y = -this->Velocity.y; // 反转垂直速度
            this->Position.y = 0.0f; // 防止球出界
        }
    }
    return this->Position;
}