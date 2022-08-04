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
    this->sprite.depth = 0.15f;
    currentTargetIndex = 0;
  }
   
  void Update(glm::vec4 camRect, Timer &timer)
  {
    //movement
    if(path.size() > currentTargetIndex)
    {
      glm::vec2 pos = glm::vec2(sprite.rect.x + sprite.rect.z/2, sprite.rect.y + sprite.rect.w/2);
      
      glm::vec2 toTarget = path[currentTargetIndex] - pos;
      float length = glm::length(toTarget);
      float movement = speed * timer.FrameElapsed();
      toTarget = glm::normalize(toTarget);
      toTarget.x *= movement;
      toTarget.y *= movement;
      gh::addVec2ToRect(toTarget , &sprite.rect);
      if(length < 10.0f)
	nextPoint();
      }
    this->sprite.UpdateMatrix(camRect);
  }

  void Draw(Render *render)
  {
    sprite.Draw(render);
  }

  void setPath(std::vector<glm::vec2> path)
  {
    this->path = path;
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


    glm::vec4 getHitBox()
    {
	return sprite.rect;
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
  
  Sprite sprite;
  std::vector<glm::vec2> path;
  int currentTargetIndex;
  float speed = 0.1f;
  int dir = 1;
};

#endif
