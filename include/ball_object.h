#ifndef BALL_OBJECT_H
#define BALL_OBJECT_H

#include "game_object.h"


class BallObject : public GameObject {
public:
    GLfloat Radius; // 球的半径
    GLboolean Stuck; // 球是否卡在挡板上

    BallObject();
    BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture sprite, glm::vec3 color = glm::vec3(1.0f));

    glm::vec2 Move(GLfloat dt, GLuint window_width);
    void Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif // BALL_OBJECT_H