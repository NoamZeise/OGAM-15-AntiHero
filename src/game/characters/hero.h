#ifndef GAME_HERO_H
#define GAME_HERO_H

#include "character.h"

class Hero : public Character
{
 public:
    Hero() {}
    Hero(Sprite sprite) : Character(sprite)
    {
    
    }
};

#endif
