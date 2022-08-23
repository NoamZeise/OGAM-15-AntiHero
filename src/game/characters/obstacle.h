#ifndef GAME_OBSTACLE_H
#define GAME_OBSTACLE_H

#include "character.h"
#include <memory>

class Obstacle : public Character
{
public:
 Obstacle() {}
 Obstacle(Sprite sprite) : Character(sprite) {}

 void Update(glm::vec4 camRect, Timer &timer) override
 {
   this->sprite.UpdateMatrix(camRect); 
 }
 void setRect(glm::vec4 rect)
 {
   sprite.rect = rect;
 }
 
    glm::vec4 getHitBox() override
    {
	return sprite.rect;
    }
};

#endif
