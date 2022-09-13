#ifndef GAME_GOD_STONE_H
#define GAME_GOD_STONE_H

#include "../sprite.h"
#include "audio.h"
#include "gamehelper.h"
#include "glm/geometric.hpp"
#include "../animation.h"
#include "glmhelper.h"
#include <cmath>

const float STONE_SPEED = 500.0f;

const float GUST_AOE = 250.0f;

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


    class Smoke
    {
    public:
	Smoke() {}
	Smoke(Resource::Texture sprite)
	{
	    this->sprite = CreateAnimationSetFromTexture(sprite, glm::vec2(1500,892), glm::vec2(0.0f, 0.0f))[0];
	    this->sprite.Reset();
	    this->sprite.setFrameDelay(250.0f);
	    
	    depth = 1.5f;
	    rect.z = this->sprite.GetCurrentFrame().size.x * 0.3f;
	    rect.w = this->sprite.GetCurrentFrame().size.y * 0.3f;
	}

	void Update(glm::vec4 camRect, Timer &timer)
	{
	    time += timer.FrameElapsed();
	    if(time < start)
	    {
		fract = (time/start);
		fract = log10(fract*10);
		rect.z = this->sprite.GetCurrentFrame().size.x * fract * 0.3f;
		rect.w = this->sprite.GetCurrentFrame().size.y * fract * 0.3f;
	    }
	    currentFrame = sprite.PlayThenSkipToFrame(timer, 2);
	}
	void Draw(Render* render)
	{
	  render->DrawQuad(currentFrame.tex,
			   glmhelper::calcMatFromRect(
	      glm::vec4(
			rect.x + ((currentFrame.size.x*0.3f)/2.0f) * (1.0f - fract),
			rect.y + ((currentFrame.size.y*0.3f)/2.0f) * (1.0f - fract),
			rect.z,
			rect.w
			),
    	      0.0f, depth),
			   glm::vec4(1.0f, 1.0f, 1.0f, 1.0f - ((time - (duration - end))/end)),
	      currentFrame.texOffset);
	}
	glm::vec4 getHitBox()
	{
	    return rect;
	}
	bool isFinished()
	{
	    return time > duration;
	}
	void setPos(glm::vec2 pos)
	{
	    rect.x = pos.x - rect.z/2;
	    rect.y = pos.y - rect.w/2;
	}
    private:

	AnimatedSprite sprite;
	Frame currentFrame;
	float time = 0.0f;
	float duration = 6000.0f;
	float start = 500.0f;
	float end = 2000.0f;
	float depth = 0.0f;
	glm::vec2 offset = glm::vec2(0.0f, 0.0f);
	glm::vec4 rect;
	float fract = 0.0f;
    };


    class Gust
    {
    public:
	Gust() {}
	Gust(Resource::Texture texture)
	{
	    this->sprite = CreateAnimationSetFromTexture(texture, glm::vec2(1125, 1091), glm::vec2(0, 0))[0];
	    sprite.Reset();
	    this->sprite.setFrameDelay(50.0f);
	    depth = 1.5f;
	    rect.z = GUST_AOE*2;
	    rect.w = GUST_AOE*2;
	}

	void Update(glm::vec4 camRect, Timer &timer)
	{
	    time += timer.FrameElapsed();
	    currentFrame = sprite.PlayOnce(timer);
	}
	void Draw(Render* render)
	{
	    render->DrawQuad(currentFrame.tex, glmhelper::calcMatFromRect(rect, 0.0f, depth), glm::vec4(1.0f), currentFrame.texOffset);
	}
	glm::vec4 getHitBox()
	{
	    return rect;
	}
	glm::vec2 getPos()
	{
	    return gh::centre(rect);
	}
	bool isFinished()
	{
	    return time > duration;
	}
	void setPos(glm::vec2 pos)
	{
	    rect.x = pos.x - rect.z/2;
	    rect.y = pos.y - rect.w/2;
	}
	float getAOE()
	{
	    return GUST_AOE;
	}
    private:

	AnimatedSprite sprite;
	Frame currentFrame;
	float time = 0.0f;
	float duration = 500.0f;
	float depth = 0.0f;
	glm::vec4 rect;
    };

}

#endif
