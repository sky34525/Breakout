#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "shader.h"
#include "game_object.h"

struct Particle {
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    GLfloat Life; // 生命周期

    Particle()
        : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

class ParticleGenerator {
public:
    ParticleGenerator(Shader &shader, Texture &texture, GLuint amount);
    void Update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    void Draw();
private:
    std::vector<Particle> particles; // 粒子容器
    GLuint amount; // 粒子数量

    Shader shader; // 着色器
    Texture texture; // 纹理
    GLuint VAO; // 顶点数组对象

    // 粒子初始化
    void init();

    //返回当前未使用的第一个粒子索引，例如如果当前没有粒子处于非活动状态，则寿命<= 0.0f或0.0f
    GLuint firstUnusedParticle();

    // 重新生成粒子
    void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif