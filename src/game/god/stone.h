#ifndef GAME_GOD_STONE_H
#define GAME_GOD_STONE_H

#include "../sprite.h"
#include "gamehelper.h"
#include "glm/geometric.hpp"

const int STONE_SPEED = 500.0f;

namespace god
{

  class Stone
  {
  public:
      Stone() {}
    Stone(Sprite sprite)
    {
      this->sprite = sprite;
      this->sprite.depth = 1.0f;
      ogRect = this->sprite.rect;
    }

    void Update(glm::vec4 camRect, Timer &timer)
    {
	elapsedTime += timer.FrameElapsed();
	float heightFactor = 1.0f - pow((elapsedTime / timeLimit), 0.5f);
	this->sprite.rect.z = ogRect.z * heightFactor;
	this->sprite.rect.w = ogRect.w * heightFactor;
	
	this->sprite.rect.x = ogRect.x - (sprite.rect.z/2.0f);
	this->sprite.rect.y = ogRect.y - (sprite.rect.z/2.0f);

	glm::vec2 dirToCentre = gh::centre(camRect) - glm::vec2(ogRect.x, ogRect.y);

	this->sprite.rect.x -= dirToCentre.x * heightFactor;
	this->sprite.rect.y -= dirToCentre.y * heightFactor;
		    
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
      sprite.rect.x = pos.x;
      sprite.rect.y = pos.y;
      ogRect = sprite.rect;
    }

  private:

      glm::vec4 ogRect;
    Sprite sprite;
    float elapsedTime = 0.0f;
    float timeLimit = STONE_SPEED;
    };

}

#endif