#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include "character.h"

class Enemy : public Character
{
 public:
  Enemy() {}
  Enemy(Sprite sprite) : Character(sprite)
  {
    
  }
  
};

#endif
