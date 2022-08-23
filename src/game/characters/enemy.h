#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include "character.h"
#include "glm/geometric.hpp"

namespace
{    
    const float DISTRACTION_RANGE = 140.0f;
    const float INVESTIGATION_INITIATION_PROXIMITY = 10.0f;
    const float INVESTIGATION_DURATION = 2200.0f;
    const float PLAYER_CHASE_RADIUS = 100.0f;
} // namespace

class Enemy : public Character
{
 public:
  Enemy() {}
  Enemy(Sprite sprite) : Character(sprite)
  {
    
  }
    void setRectToPrev() override
    {
	this->dir *= -1.0f;
	this->currentTargetIndex += this->dir;
	if(currentTargetIndex < 0)
	    currentTargetIndex = 0;
	if(currentTargetIndex >= this->path.size())
	    currentTargetIndex = this->path.size() - 1;
	sprite.rect = prevRect;
    }

    void Update(glm::vec4 camRect, Timer &timer, glm::vec2 playerPos)
    {
	prevRect = sprite.rect;
	if(currentState == EnemyState::Patrol)
	{
	    Character::Update(camRect, timer);
	}
	else if(currentState == EnemyState::Investigate)
	{	   
	    glm::vec2 toTarget = investigationTarget - gh::centre(sprite.rect);
	    float length = glm::length(toTarget);
	    moveToTarget(toTarget, timer);
	    if(length < INVESTIGATION_INITIATION_PROXIMITY)
		investigationTimer += timer.FrameElapsed();
	    if(investigationTimer > investigationDuration)
	    {
		currentState = EnemyState::Patrol;
	    }
	    sprite.UpdateMatrix(camRect);
	}
	else if(currentState == EnemyState::Chase)
	{
	    glm::vec2 toTarget = playerPos - gh::centre(sprite.rect);
	    moveToTarget(toTarget, timer);
	    sprite.UpdateMatrix(camRect);
	}

	if(glm::distance(gh::centre(sprite.rect), playerPos) < PLAYER_CHASE_RADIUS)
	{
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

private:

    enum EnemyState
	{
	    Patrol,
	    Investigate,
	    Chase,
	};

    EnemyState currentState = EnemyState::Patrol;
    glm::vec2 investigationTarget = glm::vec2(0);
    float investigationTimer = 0.0f;
    float investigationDuration = INVESTIGATION_DURATION;
};

#endif
