#ifndef GAME_GOD_STONE_H
#define GAME_GOD_STONE_H

#include "../sprite.h"

namespace god
{

  class Stone
  {
  public:
      Stone() {}
    Stone(Sprite sprite)
    {
      this->sprite = sprite;
      this->sprite.rect.z *= 0.1f;
      this->sprite.rect.w *= 0.1f;
      this->sprite.depth = 1.0f;
    }

    void Update(glm::vec4 camRect, Timer &timer)
    {
      elapsedTime += timer.FrameElapsed();
      sprite.UpdateMatrix(camRect);
    }

    void Draw(Render *render)
    {
      sprite.Draw(render);
    }

    glm::vec4 hit()
    {
      return elapsedTime > timeLimit ?
	sprite.rect : glm::vec4(0);
    }

    void setPos(glm::vec2 pos)
    {
      sprite.rect.x = pos.x - sprite.rect.z/2;
      sprite.rect.y = pos.y - sprite.rect.w/2;
    }

  private:


    Sprite sprite;
    float elapsedTime = 0.0f;
    float timeLimit = 100.0f;
  };

}

#endif
