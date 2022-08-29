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
	this->sprite.rect.z *= 1.2f;
	this->sprite.rect.w *= 1.2f;
    }

    bool isFinished() { return finishedLevel; }

  void setPath(std::vector<glm::vec2> path) override
  {
      waiting = false;
      currentSpeed = 0.0f;
      finishedLevel = false;
      displayPath = true;
      pathOutline.clear();
      circle.spriteColour = glm::vec4(0.2f, 0.54f, 0.3f, 0.2f);
      Character::setPath(path);
  }

    void Update(glm::vec4 camRect, Timer &timer) override
    {
	//waitTimer += timer.FrameElapsed();
	prevRect = sprite.rect;
	if(!waiting)
	    Character::Update(camRect, timer);
	else
	    sprite.UpdateMatrix(camRect);
    }

    void setCheckpoint(glm::vec2 pos, int targetIndex)
    {
	this->sprite.rect.x = pos.x - sprite.rect.z/2.0f;
	this->sprite.rect.y = pos.y - sprite.rect.w/2.0f;
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

    glm::vec4 getHitBox() override
    {
	const float HITBOX_SCALE = 0.6f;
	float xOff = sprite.rect.z * HITBOX_SCALE;
	float yOff = sprite.rect.w * HITBOX_SCALE;
	auto rect = glm::vec4(sprite.rect.x + xOff/2.0f, sprite.rect.y + yOff/1.5f, sprite.rect.z - xOff, sprite.rect.w - yOff);
	return rect;
    }

    void simulateEnd()
    {
	currentTargetIndex = path.size() - 1;
	nextPoint();
    }

protected:
   void nextPoint() override
  {
    dir =
      currentTargetIndex <= 0 ?
              1 :
              currentTargetIndex >= path.size() - 1 ?
                 -1 : dir;
    if(dir == -1)
    {
	dir = 1;
	finishedLevel = true;
    }
    else {
	    currentTargetIndex += dir;
    }
  }
private:
    bool finishedLevel = false;
    bool waiting = false;
};

#endif
