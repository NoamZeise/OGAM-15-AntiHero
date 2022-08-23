#ifndef GAME_CHARACTER_H
#define GAME_CHARACTER_H

#include "../sprite.h"
#include "gamehelper.h"
#include "glm/geometric.hpp"
#include <glmhelper.h>

#include <iostream>
#include <vector>

class Character
{
 public:
  Character() {}

  Character(Sprite sprite)
  {
    this->sprite = sprite;
    this->sprite.rect.z *= 0.2f;
    this->sprite.rect.w *= 0.2f;
    this->sprite.depth = 0.05f;
    currentTargetIndex = 0;
  }
   
  virtual void Update(glm::vec4 camRect, Timer &timer)
  {
    //movement
    if(path.size() > currentTargetIndex)
    {
	prevRect = sprite.rect;
      glm::vec2 pos = glm::vec2(sprite.rect.x + sprite.rect.z/2, sprite.rect.y + sprite.rect.w/2);
      
      glm::vec2 toTarget = path[currentTargetIndex] - pos;
      float length = glm::length(toTarget);
      moveToTarget(toTarget, timer);
      if(length < 10.0f)
	nextPoint();
      }
    this->sprite.UpdateMatrix(camRect);
  }

  void Draw(Render *render)
  {
    sprite.Draw(render);
  }

  virtual void setPath(std::vector<glm::vec2> path)
  {
    this->path = path;
    dir = 1;
    if(path.size() != 0)
    {
      currentTargetIndex = 0;
      this->sprite.rect.x = this->path[0].x;
      this->sprite.rect.y = this->path[0].y;
    }
  }

  glm::vec2 getPos()
  {
    return glm::vec2(
		     sprite.rect.x + sprite.rect.z/2,
		     sprite.rect.y + sprite.rect.w/2
		     );
  }


    virtual glm::vec4 getHitBox()
    {
	const float HITBOX_SCALE = 0.4f;
	float xOff = sprite.rect.z * HITBOX_SCALE;
	float yOff = sprite.rect.w * HITBOX_SCALE;
	auto rect = glm::vec4(sprite.rect.x + xOff/2.0f, sprite.rect.y + yOff/2.0f, sprite.rect.z - xOff, sprite.rect.w - yOff);
	return rect;
    }

    virtual void setRectToPrev()
    {
	sprite.rect = prevRect;
    }

 protected:

  virtual void nextPoint()
  {
    dir =
      currentTargetIndex <= 0 ?
              1 :
              currentTargetIndex >= path.size() - 1 ?
                 -1 : dir;
    currentTargetIndex += dir;
  }

    void moveToTarget(glm::vec2 vecToTarget, Timer &timer)
    {
      float movement = speed * timer.FrameElapsed();
      vecToTarget = glm::normalize(vecToTarget);
      vecToTarget.x *= movement;
      vecToTarget.y *= movement;
      gh::addVec2ToRect(vecToTarget , &sprite.rect);
    }

    glm::vec4 prevRect;
  Sprite sprite;
  std::vector<glm::vec2> path;
  int currentTargetIndex;
  float speed = 0.1f;
  int dir = 1;
};

#endif
