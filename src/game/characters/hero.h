#ifndef GAME_HERO_H
#define GAME_HERO_H

#include "character.h"

const float WAIT_TIME = 1200.0f;

class Hero : public Character
{
 public:
    Hero() {}
    Hero(Sprite sprite, Sprite circle) : Character(sprite, circle)
    {
	this->sprite.rect.z *= 2.0f;
	this->sprite.rect.w *= 2.0f;
    }

    bool isFinished() { return finishedLevel; }

  void setPath(std::vector<glm::vec2> path) override
  {
      waiting = false;
      currentSpeed = 0.0f;
      finishedLevel = false;
      circle.spriteColour = glm::vec4(0.2f, 0.54f, 0.3f, 0.2f);
      Character::setPath(path);
  }

    void Update(glm::vec4 camRect, Timer &timer) override
    {
	//waitTimer += timer.FrameElapsed();
	if(!waiting)
	    Character::Update(camRect, timer);
    }

    void setCheckpoint(glm::vec2 pos, int targetIndex)
    {
	this->sprite.rect.x = pos.x;
	this->sprite.rect.y = pos.y;
	currentTargetIndex = targetIndex;
	dir = 1;
    }

    int getTargetIndex() { return currentTargetIndex; }

    void Wait()
    {
	waiting = true;
	currentSpeed = 0.0f;
    }

    void Go()
    {
	waiting = false;
    }

protected:
   void nextPoint() override
  {
    dir =
      currentTargetIndex <= 0 ?
              1 :
              currentTargetIndex >= path.size() - 1 ?
                 -1 : dir;
    currentTargetIndex += dir;
    if(dir == -1)
	finishedLevel = true;
  }
private:
    bool finishedLevel = false;
    bool waiting = false;
};

#endif
