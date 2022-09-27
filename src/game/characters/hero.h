#ifndef GAME_HERO_H
#define GAME_HERO_H

#include "audio.h"
#include "character.h"
#include "gamehelper.h"
#include "../animation.h"
#include "glm/ext/scalar_constants.hpp"
#include <map>

const float WAIT_TIME = 1200.0f;
const float FOOTSTEP_SFX_DELAY = 600.0f;
const float PLAYER_FOOTSTEP_VOLUME = 0.2f;
const glm::vec2 PLAYER_SIZE = glm::vec2(526, 469);

class Hero : public Character
{
 public:
    Hero() {}
    Hero(Sprite sprite, Sprite circle, Audio::Manager* audio) : Character(sprite, circle, audio)
    {
	this->sprite.rect.z = PLAYER_SIZE.x * 0.25f;
	this->sprite.rect.w = PLAYER_SIZE.y * 0.25f;

	auto animations = CreateAnimationSetFromTexture(sprite.texture, PLAYER_SIZE, glm::vec2(0));
	for(auto &a: animations)
	    a.setFrameDelay(280.0f);
	this->animations[Direction::Right] = animations[0];
	animations[0].InvertFrameX();
	this->animations[Direction::Left] = animations[0];
	this->animations[Direction::Up] = animations[1];
	this->animations[Direction::Down] = animations[2];

	for( int i = 1; i < 6; i++)
	    audio->LoadAudioFile("audio/SFX/Footsteps/Footstep Light" + std::to_string(i) + ".wav");
	for( int i = 1; i < 6; i++)
	    audio->LoadAudioFile("audio/SFX/Footsteps/Footstep Mud" + std::to_string(i) + ".wav");
    }

    bool isFinished() { return finishedLevel; }

  void setPath(std::vector<glm::vec2> path) override
  {
      waiting = false;
      currentSpeed = 0.0f;
      finishedLevel = false;
      displayPath = true;
      pathOutline.clear();
      circle.spriteColour = DOT_ACTIVE;
      Character::setPath(path);
      
  }

    void Update(glm::vec4 camRect, Timer &timer) override
    {
	//waitTimer += timer.FrameElapsed();
	footstepTimer += timer.FrameElapsed();
	prevRect = sprite.rect;
	if(!waiting)
	{
	    if(collided)
		footstepTimer = 0.0f;

	    
	    Character::Update(camRect, timer);

	    if(displayPath)
	    {
		auto pos = getPos();
		for(auto &pathSpot: pathOutline) {
		    if(
		       abs(pathSpot.rect.x - pos.x) < 10 &&
		       abs(pathSpot.rect.y - (pos.y + sprite.rect.w/2.2)) < 17)
			pathSpot.spriteColour = DOT_DONE;
		}
	    }

	    if(!collided)
	    {
		if(abs(previousRotation)> 3.1415*(3.0/4.0))
		    currentDirection = Direction::Left;
		else if(previousRotation > 3.1415*(1.0/4.0))
		    currentDirection = Direction::Up;
		else if(previousRotation > -3.1415*(1.0/4.0))
		    currentDirection = Direction::Right;
		else
		    currentDirection = Direction::Down;

		currentFrame = animations[currentDirection].Play(timer);
	    }
	    sprite.texture = currentFrame.tex;
	    sprite.texOffset = currentFrame.texOffset;
	    
	    if(footstepTimer > FOOTSTEP_SFX_DELAY)
	    {
		footstepTimer = 0.0f;
		if(rand.PositiveReal() > 0.95)
		     audio->Play("audio/SFX/Footsteps/Footstep Light" + rand.stringNum(5) + ".wav", false, PLAYER_FOOTSTEP_VOLUME);
		//	else if(rand.PositiveReal() > 0.7)
		//   audio->Play("audio/SFX/Footsteps/Footstep Water" + rand.stringNum(5) + ".wav", false, 0.4);
		else
		    audio->Play("audio/SFX/Footsteps/Footstep Mud" + rand.stringNum(5) + ".wav", false, PLAYER_FOOTSTEP_VOLUME);
	    }
	}
	else
	{
	    sprite.UpdateMatrix(camRect);
	    for(auto& c: pathOutline)
		c.UpdateMatrix(camRect);
	}
	collided = false;
    }

    void setCheckpoint(glm::vec2 pos, int targetIndex)
    {
	this->sprite.rect.x = pos.x - sprite.rect.z/2.0f;
	this->sprite.rect.y = pos.y - sprite.rect.w/2.0f;
	currentTargetIndex = targetIndex;
	for(auto& mark: pathOutline)
	{
	    if(abs(mark.rect.x - path[currentTargetIndex].x) < 100 &&
	       abs(mark.rect.y - path[currentTargetIndex].y) < 100)
		break;
	    else
		mark.spriteColour = DOT_DONE;
	}
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

    float footstepTimer = 0.0f;

    enum Direction  {
	Left,
	Right,
	Up,
	Down
    };

    Direction currentDirection;
    std::map<Direction, AnimatedSprite> animations;
    Frame currentFrame;
};

#endif
