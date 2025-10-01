#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"

// 游戏对象类，表示游戏中的一个实体
class GameObject {
public:
    // 对象状态
    glm::vec2   Position, Size, Velocity;
    glm::vec3   Color;
    GLfloat     Rotation; // 旋转角度
    GLboolean   IsSolid; // 是否为坚固的物体（如墙壁）
    GLboolean   Destroyed; // 是否被销毁（如砖块被击中后）
    // 纹理
    Texture     Sprite;

    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    
    virtual void Draw(SpriteRenderer &renderer);
};

#endif

