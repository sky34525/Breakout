/*
粒子生成器
粒子生成器维护一个vector存储所有粒子
需要注意的是粒子的初始生命命是0.0f
也就是说如果在Update之前调用Draw函数，粒子是不会被渲染的
Update函数会找到第一个life <= 0.0f的粒子，并重新生成它
*/

#include "particle_generator.h"

ParticleGenerator::ParticleGenerator(Shader &shader, Texture &texture, GLuint amount)
    : shader(shader), texture(texture), amount(amount) {
    this->init();
}

void ParticleGenerator::init() {
    // 配置 VAO
    GLuint VBO;
    GLfloat particle_quad[] = {
        // Pos      // Tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(this->VAO);
    // 填充顶点缓冲
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // 设置顶点属性指针
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    // 创建指定数量的粒子
    for (GLuint i = 0; i < this->amount; ++i)
        this->particles.push_back(Particle());
}

void ParticleGenerator::Draw() {
    // 使用加法混合来产生“发光”效果
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    this->shader.Use();
    for (Particle particle : this->particles) {
        if (particle.Life > 0.0f) {
            this->shader.SetVector2f("offset", particle.Position);
            this->shader.SetVector4f("color", particle.Color);
            //glActiveTexture(GL_TEXTURE0);
            this->texture.Bind();
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // 恢复默认混合模式
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::Update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset) {
    // 添加新粒子
    for (GLuint i = 0; i < newParticles; ++i) {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], object, offset);
    }
    // 更新所有粒子
    for (GLuint i = 0; i < this->amount; ++i) {
        Particle &p = this->particles[i];
        p.Life -= dt; // 减少生命值
        if (p.Life > 0.0f) { // 如果粒子还活着
            p.Position -= p.Velocity * dt; // 移动粒子位置
            p.Color.a -= dt * 2.5; // 使粒子逐渐变透明
        }
    }
}

GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle() {
    // 从上次找到的粒子索引开始搜索
    for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
        if (this->particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }       
    }
    // 如果没有找到未使用的粒子，则从头开始搜索
    for (GLuint i = 0; i < lastUsedParticle; ++i) {
        if (this->particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    return 0; // 如果没有找到，返回0        
}

void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset) {
    GLfloat random = ((rand() % 100) - 50) / 10.0f; // 生成一个-5.0到5.0之间的随机偏移
    GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f); // 生成一个0.5到1.5之间的随机颜色值
    particle.Position = object.Position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.0f;
    particle.Velocity = object.Velocity * 0.1f; // 粒子速度是物体速度的10%
}