#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include "character.h"
#include "glm/geometric.hpp"

namespace
{    
    const float DISTRACTION_RANGE = 150.0f;
    const float INVESTIGATION_INITIATION_PROXIMITY = 10.0f;
    const float INVESTIGATION_DURATION = 4200.0f;
    const float PLAYER_CHASE_RADIUS = 150.0f;
    const float ENEMY_PATROL_SPEED = 0.025f;
    const float ENEMY_INVESTIGATE_SPEED = 0.03f;
    const float ENEMY_CHASE_SPEED = 0.12f;
} // namespace

class Enemy : public Character
{
 public:
  Enemy() {}
    Enemy(Sprite sprite, Sprite distracted, Sprite circle, Sprite search) : Character(sprite)
  {
      speed = ENEMY_PATROL_SPEED;
      distracted.rect.z *= 0.1f;
      distracted.rect.w *= 0.1f;
      distracted.depth = 2.0f;
      circle.depth = CHARACTER_DEPTH;
      circle.rect.z = DISTRACTION_RANGE * 2.0f;
      circle.rect.w = DISTRACTION_RANGE * 2.0f;
      search.depth = CHARACTER_DEPTH+0.01f;
      search.rect.z = PLAYER_CHASE_RADIUS * 2.0f;
      search.rect.w = PLAYER_CHASE_RADIUS * 2.0f;
      this->search = search;
      this->circle = circle;
      this->distracted = distracted;
  }
    void setRectToPrev() override
    {
	this->dir *= -1;
	this->currentTargetIndex += this->dir;
	if(currentTargetIndex < 0)
	    currentTargetIndex = 0;
	if(currentTargetIndex >= this->path.size())
	    currentTargetIndex = (int)this->path.size() - 1;
	sprite.rect = prevRect;
    }

    void Update(glm::vec4 camRect, Timer &timer, glm::vec2 playerPos)
    {
	prevRect = sprite.rect;
	currentSpeed += acceleration * timer.FrameElapsed();
	if(currentSpeed > speed) { currentSpeed = speed; }
	if(currentState == EnemyState::Patrol)
	{
	    speed = ENEMY_PATROL_SPEED;
	    Character::Update(camRect, timer);
	    direction = glm::normalize(lastToTarget);
	}
	else if(currentState == EnemyState::Investigate)
	{
	    speed = ENEMY_INVESTIGATE_SPEED;
	    glm::vec2 toTarget = investigationTarget - gh::centre(sprite.rect);
	    direction = glm::normalize(toTarget);
	    //float length = glm::length(toTarget);
	    moveToTarget(toTarget, timer);
	    //if(length < INVESTIGATION_INITIATION_PROXIMITY)
		investigationTimer += timer.FrameElapsed();
	    if(investigationTimer > investigationDuration)
	    {
		currentState = EnemyState::Patrol;
	    }
	    sprite.UpdateMatrix(camRect);
	}
	else if(currentState == EnemyState::Chase)
	{
	    speed = ENEMY_CHASE_SPEED;
	    glm::vec2 toTarget = playerPos - gh::centre(sprite.rect);
	    direction = glm::normalize(toTarget);
	    moveToTarget(toTarget, timer);
	    sprite.UpdateMatrix(camRect);
	}

	distracted.rect.x = sprite.rect.x + (sprite.rect.z - distracted.rect.z)/2.0f;
	distracted.rect.y = sprite.rect.y - distracted.rect.w;
	distracted.UpdateMatrix(camRect);
	circle.rect.x = (sprite.rect.x + sprite.rect.z/2.0f) - circle.rect.z/2.0f;
	circle.rect.y = (sprite.rect.y + sprite.rect.w/2.0f) - circle.rect.w/2.0f;
	circle.spriteColour = glm::vec4(0.9f, 0.9f, 0.2f, 0.2f);
	circle.UpdateMatrix(camRect);
	search.rect.x = (sprite.rect.x + sprite.rect.z/2.0f) - search.rect.z/2.0f;
	search.rect.y = (sprite.rect.y + sprite.rect.w/2.0f) - search.rect.w/2.0f;
	search.spriteColour = glm::vec4(0.9f, 0.2f, 0.2f, 0.2f);
	search.rotate = atan2(direction.y, direction.x)*180.0/3.14159265 + 90.0f; // do math (direction)?
	search.UpdateMatrix(camRect);

	 if(glm::distance(gh::centre(sprite.rect), playerPos) < PLAYER_CHASE_RADIUS)
	{
	    glm::vec2 toPlayer = glm::vec2(playerPos) - glm::vec2(sprite.rect.x, sprite.rect.y);
	    toPlayer = glm::normalize(toPlayer);
	    float angle = atan2(toPlayer.y, toPlayer.x)*180.0/3.14159265 + 90.0f;
	    if(abs(angle - search.rotate) < 22.5f)
		currentState = EnemyState::Chase;
	}
    }

    void soundEvent(glm::vec2 sound)
    {
	if(currentState != EnemyState::Chase)
	{

	    if(glm::distance(gh::centre(sprite.rect), sound) < DISTRACTION_RANGE)
		{
		    currentState = EnemyState::Investigate;
		    investigationTarget = sound;
		    investigationTimer = 0.0f;
		}
	}

    }

    void Draw(Render *render) override
    {
	Character::Draw(render);
	if(currentState == EnemyState::Investigate)
	    distracted.Draw(render);
	circle.Draw(render);
	search.Draw(render);
    }

private:

    enum EnemyState
	{
	    Patrol,
	    Investigate,
	    Chase,
	    Dead,
	};

    Sprite distracted;
    Sprite circle;
    Sprite search;
    EnemyState currentState = EnemyState::Patrol;
    glm::vec2 investigationTarget = glm::vec2(0);
    float investigationTimer = 0.0f;
    float investigationDuration = INVESTIGATION_DURATION;
    glm::vec2 direction = glm::vec2(1.0f, 0.0f);
};

#endif
