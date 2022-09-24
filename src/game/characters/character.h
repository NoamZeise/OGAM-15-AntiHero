#ifndef GAME_CHARACTER_H
#define GAME_CHARACTER_H

#include "../sprite.h"
#include "gamehelper.h"
#include "glm/geometric.hpp"
#include <glmhelper.h>
#include <audio.h>

#include <iostream>
#include <vector>

const float CHARACTER_DEPTH = 0.15f;

class Character
{
 public:
  Character() {}

  Character(Sprite sprite, Sprite circle, Audio::Manager* audio)
  {
    this->audio = audio;
    this->sprite = sprite;
    this->sprite.rect.z *= 0.2f;
    this->sprite.rect.w *= 0.2f;
    this->sprite.depth = CHARACTER_DEPTH;
    currentTargetIndex = 0;
    this->circle = circle;
    this->circle.rect.z = 10.0f;
    this->circle.rect.w = 10.0f;
    this->circle.depth = CHARACTER_DEPTH - 0.01f;
    this->circle.spriteColour = glm::vec4(0.5f, 0.1f, 0.14f, 0.4f);
  }
   
  virtual void Update(glm::vec4 camRect, Timer &timer)
  {
    //movement
    if(path.size() > currentTargetIndex)
    {
	prevRect = sprite.rect;
	glm::vec2 pos = glm::vec2(sprite.rect.x + sprite.rect.z/2, sprite.rect.y + sprite.rect.w/2);
	
	glm::vec2 toTarget = path[currentTargetIndex] - pos;
	lastToTarget = toTarget;
	previousRotation = atan2(toTarget.y, toTarget.x);
	
	float length = glm::length(toTarget);
	currentSpeed += acceleration * timer.FrameElapsed();
	if(currentSpeed > speed) { currentSpeed = speed; }
	moveToTarget(toTarget, timer);
	if(length < 10.0f)
	    nextPoint();
    }
    this->sprite.UpdateMatrix(camRect);
    for(auto& c: pathOutline)
	c.UpdateMatrix(camRect);
  }

  virtual void Draw(Render *render)
  {
    sprite.Draw(render);
    //render->DrawQuad(Resource::Texture(), glmhelper::getModelMatrix(getHitBox(), 0.0f, 4.0f));
    for(auto& c: pathOutline)
	c.Draw(render);
  }

  virtual void setPath(std::vector<glm::vec2> path)
  {
    this->path = path;
    dir = 1;
    currentTargetIndex = 0;
    if(path.size() != 0)
    {
	this->sprite.rect.x = this->path[0].x - this->sprite.rect.z/2.0f;
	this->sprite.rect.y = this->path[0].y - this->sprite.rect.w/2.0f - 10.0f;
      if(displayPath)
      {
	  float takeaway = 0.0f;
	  for(int p = 1; p < path.size(); p++)
	  {
	      float dist = glm::distance(path[p-1], path[p]);
	      glm::vec2 dir = glm::normalize(path[p] - path[p-1]);
	      float nowDist = 0.0f + takeaway;
	      while(nowDist < dist)
	      {
		  glm::vec2 pos = path[p-1] + dir*nowDist;
		  circle.rect = glm::vec4(
					  pos.x - circle.rect.z/2.0f,
					  pos.y + sprite.rect.w/2.0f - circle.rect.w/2.0f,
					  circle.rect.z, circle.rect.w);
		  pathOutline.push_back(circle);
		  nowDist += 100.0f;
	      }
	      takeaway = nowDist - dist;
	  }
      }
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
	const float HITBOX_SCALE = 0.6f;
	float xOff = sprite.rect.z * HITBOX_SCALE;
	float yOff = sprite.rect.w * HITBOX_SCALE;
	auto rect = glm::vec4(sprite.rect.x + xOff/2.0f, sprite.rect.y + yOff/2.0f, sprite.rect.z - xOff, sprite.rect.w - yOff);
	return rect;
    }

    virtual void setRectToPrev()
    {
	currentSpeed = 0.0f;
	sprite.rect = prevRect;
	collided = true;
    }
    
    virtual void push(glm::vec2 vec, Timer &timer)
    {
	sprite.rect.x += vec.x * timer.FrameElapsed();
	sprite.rect.y += vec.y * timer.FrameElapsed();
	currentSpeed = 0.0f;
    }

 protected:

  virtual void nextPoint()
  {
      int lastDir = dir;
      dir =
	  currentTargetIndex <= 0 ?
	  1 :
	  currentTargetIndex >= path.size() - 1 ?
	  -1 : dir;
      if(lastDir != dir)
	  currentSpeed = 0.0f;
      currentTargetIndex += dir;
  }

    void moveToTarget(glm::vec2 vecToTarget, Timer &timer)
    {
      float movement = currentSpeed * timer.FrameElapsed();
      vecToTarget = glm::normalize(vecToTarget);
      vecToTarget.x *= movement;
      vecToTarget.y *= movement;
      gh::addVec2ToRect(vecToTarget , &sprite.rect);
    }

    Audio::Manager* audio;
    glm::vec2 lastToTarget;
    glm::vec4 prevRect;
    Sprite sprite;
    std::vector<glm::vec2> path;
    int currentTargetIndex;
    float speed = 0.1f;
    float currentSpeed = 0.0f;
    float acceleration = 0.0001f;
    int dir = 1;
    Sprite circle;
    std::vector<Sprite> pathOutline;
    float previousRotation;

    bool  displayPath = false;
    bool collided = false;
    gh::Random rand;
};

#endif
