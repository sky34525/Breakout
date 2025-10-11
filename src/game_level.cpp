
#include "game_level.h"
#include "resource_manager.h"

#include <fstream>
#include <sstream>

void GameLevel::Load(const GLchar *file, GLuint levelWidth, GLuint levelHeight) {
    // 清除旧的砖块
    this->Bricks.clear();

    // 获取正确的资源路径
    std::string levelPath = ResourceManager::GetResourcePath(file);

    // 读取文件
    GLuint tileCode;
    std::string line;
    std::ifstream fstream(levelPath);
    std::vector<std::vector<GLuint>> tileData;
    if (fstream) {
        while (std::getline(fstream, line)) { //逐行读取
            std::istringstream sstream(line);
            std::vector<GLuint> row;
            while (sstream >> tileCode) //逐个读取
                row.push_back(tileCode);
            tileData.push_back(row);
        }
    }
    // 初始化关卡
    if(!tileData.empty())
        this->init(tileData, levelWidth, levelHeight);
}

void GameLevel::Draw(SpriteRenderer &remderer) {
    for (GameObject &tile : this->Bricks)
        if (!tile.Destroyed)
            tile.Draw(remderer);
}

//所有可破坏的砖块都被销毁后，关卡完成
GLboolean GameLevel::IsCompleted() {
    for (GameObject &tile : this->Bricks)
        if (!tile.IsSolid && !tile.Destroyed)
            return GL_FALSE;
    return GL_TRUE;
}

void GameLevel::init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight) {
    // 计算每个砖块的宽度和高度
    GLuint height = tileData.size();
    GLuint width = tileData[0].size();
    GLfloat unit_width = levelWidth / static_cast<GLfloat>(width); //每个砖块的宽度
    GLfloat unit_height = levelHeight / static_cast<GLfloat>(height); //每个砖块的高度

    // 遍历二维数组，创建砖块
    for (GLuint y = 0; y < height; ++y) {
        for (GLuint x = 0; x < width; ++x) {
            // 获取砖块代码
            GLuint tileCode = tileData[y][x];
            // 根据砖块代码创建不同类型的砖块
            if (tileCode == 1) { // 不可摧毁砖块
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
                obj.IsSolid = GL_TRUE;
                this->Bricks.push_back(obj);
            } else if (tileCode > 1) { // 可摧毁砖块
                glm::vec3 color = glm::vec3(1.0f); // 默认白色
                if (tileCode == 2)
                    color = glm::vec3(0.2f, 0.6f, 1.0f); // 蓝色
                else if (tileCode == 3)
                    color = glm::vec3(0.0f, 0.7f, 0.0f); // 绿色
                else if (tileCode == 4)
                    color = glm::vec3(0.8f, 0.8f, 0.4f); // 黄色
                else if (tileCode == 5)
                    color = glm::vec3(1.0f, 0.5f, 0.0f); // 橙色

                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color));
            }
            // tileCode == 0 忽略空白砖块
        }
    }
}