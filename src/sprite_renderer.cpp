
#include "sprite_renderer.h"

SpriteRenderer::SpriteRenderer(Shader &shader) : shader(shader), quadVAO(0) {
    this->initRenderData();
}

SpriteRenderer::~SpriteRenderer() {
    glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::initRenderData(){
    GLuint VBO;
    GLfloat vertices[] = {
        // Pos      // Tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO); //绑定顶点缓冲对象
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //将顶点数据复制到缓冲的内存中

    glBindVertexArray(this->quadVAO); //绑定VAO，开始记录 （当前绑定的VBO也会被记录）

    //配置顶点属性
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SpriteRenderer::DrawSprite(Texture &texture, glm::vec2 postion, glm::vec2 size, GLfloat rotate, glm::vec3 color){
    this->shader.Use();
    glm::mat4 model = glm::mat4(1.0f); // 必须初始化为单位矩阵，否则后续的矩阵乘法会出错，窗口中没有任何东西
    //当试图在一个场景中用旋转矩阵和缩放矩阵放置一个对象的时候，建议是首先做缩放变换，再旋转，最后才是位移变换。因为矩阵乘法是从右向左执行的，所以我们变换的矩阵顺序是相反的：移动，旋转，缩放。
    model = glm::translate(model, glm::vec3(postion, 0.0f)); // 平移
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // 移动到旋转中心
    model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // 旋转
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // 移动回去

    model = glm::scale(model, glm::vec3(size, 1.0f)); // 缩放

    this->shader.SetMatrix4("model", model);
    this->shader.SetVector3f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}