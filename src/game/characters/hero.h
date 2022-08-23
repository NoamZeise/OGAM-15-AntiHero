#ifndef GAME_HERO_H
#define GAME_HERO_H

#include "character.h"

class Hero : public Character
{
 public:
    Hero() {}
    Hero(Sprite sprite) : Character(sprite)
    {
	this->sprite.rect.z *= 2.0f;
	this->sprite.rect.w *= 2.0f;
    }

    bool isFinished() { return finishedLevel; }

  void setPath(std::vector<glm::vec2> path) override
  {
      finishedLevel = false;
      Character::setPath(path);
  }

    void setCheckpoint(glm::vec2 pos, int targetIndex)
    {
	this->sprite.rect.x = pos.x;
	this->sprite.rect.y = pos.y;
	currentTargetIndex = targetIndex;
	dir = 1;
    }

    int getTargetIndex() { return currentTargetIndex; }

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
};

#endif
