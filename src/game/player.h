#ifndef PLAYER_H
#define PLAYER_H

#include "sprite.h"
#include <timer.h>
#include <input.h>

class Player
{
 public:
  Player() {}
  Player(Sprite sprite);
  void Update(glm::vec4 cameraRect, Timer &timer, Input &input);
  void Draw(Render *render);
  glm::vec2 getPos()
  {
    return glm::vec2(
		     sprite.rect.x + sprite.rect.z/2,
		     sprite.rect.y + sprite.rect.w/2
		     );
  }

 private:
  glm::vec2 movementUpdate(Input &input);
  glm::vec4 hitbox;
  Sprite sprite;
  float speed = 0.5f;
};



#endif
