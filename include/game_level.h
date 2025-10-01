
#include <glad/glad.h>
#include <vector>

#include "game_object.h"
#include "sprite_renderer.h"
#include "resource_manager.h"

class GameLevel {
public:
    /*Bricks 是一个存储 GameObject 对象的动态数组（vector）
    每个 GameObject 对象表示关卡中的一个砖块
    使用 vector 的好处是可以动态调整大小，方便添加和删除砖块
    */
    std::vector<GameObject> Bricks;

    GameLevel() { }
    // 加载关卡
    void Load(const char *file, GLuint levelWidth, GLuint levelHeight);
    // 渲染关卡
    void Draw(SpriteRenderer &renderer);
    // 检查关卡是否完成
    GLboolean IsCompleted();
private:
    // 初始化关卡
    void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};