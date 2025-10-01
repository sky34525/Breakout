
#include "game_object.h"

GameObject::GameObject()
    : Position(0, 0), Size(1, 1), Velocity(0.0f), Color(1.0f), Rotation(0.0f), IsSolid(GL_FALSE), Destroyed(GL_FALSE), Sprite() { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture sprite, glm::vec3 color, glm::vec2 velocity)
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), IsSolid(GL_FALSE), Destroyed(GL_FALSE), Sprite(sprite) { }

void GameObject::Draw(SpriteRenderer &renderer) {
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}
