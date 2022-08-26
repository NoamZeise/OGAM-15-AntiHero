#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include "character.h"
#include "glm/geometric.hpp"

namespace
{    
    const float DISTRACTION_RANGE = 200.0f;
    const float INVESTIGATION_INITIATION_PROXIMITY = 10.0f;
    const float INVESTIGATION_DURATION = 4200.0f;
    const float PLAYER_CHASE_RADIUS = 180.0f;
    const float PLAYER_CHASE_OUT_OF_VIEW_RADIUS = 80.0f;
    const float ENEMY_PATROL_SPEED = 0.025f;
    const float ENEMY_INVESTIGATE_SPEED = 0.03f;
    const float ENEMY_CHASE_SPEED = 0.12f;
} // namespace

class Enemy : public Character
{
 public:
  Enemy() {}
    Enemy(Sprite sprite, Sprite distracted, Sprite circle, Sprite search) : Character(sprite, circle)
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
      circle.rect.z = PLAYER_CHASE_OUT_OF_VIEW_RADIUS * 2.0f;
      circle.rect.w = PLAYER_CHASE_OUT_OF_VIEW_RADIUS * 2.0f;
      this->outOfview = circle;
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
	time += timer.FrameElapsed();
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
	    float length = glm::length(toTarget);
	    if(length > INVESTIGATION_INITIATION_PROXIMITY)
		moveToTarget(toTarget, timer);
	    else
	    {
		//wobble search
	    }
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
	search.rotate = atan2(direction.y, direction.x)*180.0/3.14159265 + 90.0f
	    + sin(time*0.001f)*25.0f;
	search.UpdateMatrix(camRect);
	
	outOfview.rect.x = (sprite.rect.x + sprite.rect.z/2.0f) - outOfview.rect.z/2.0f;
	outOfview.rect.y = (sprite.rect.y + sprite.rect.w/2.0f) - outOfview.rect.w/2.0f;
	outOfview.spriteColour = glm::vec4(0.9f, 0.2f, 0.2f, 0.2f);
	outOfview.UpdateMatrix(camRect);

	float distToPlayer = glm::distance(gh::centre(sprite.rect), playerPos);
	 if(distToPlayer < PLAYER_CHASE_RADIUS)
	{
	    glm::vec2 toPlayer = glm::vec2(playerPos) - glm::vec2(sprite.rect.x, sprite.rect.y);
	    toPlayer = glm::normalize(toPlayer);
	    float angle = atan2(toPlayer.y, toPlayer.x)*180.0/3.14159265 + 90.0f;
	    if(abs(angle - search.rotate) < 22.5f || distToPlayer < PLAYER_CHASE_OUT_OF_VIEW_RADIUS)
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
	//circle.Draw(render);
	search.Draw(render);
	outOfview.Draw(render);
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
    Sprite outOfview;
    Sprite search;
    EnemyState currentState = EnemyState::Patrol;
    glm::vec2 investigationTarget = glm::vec2(0);
    float investigationTimer = 0.0f;
    float investigationDuration = INVESTIGATION_DURATION;
    glm::vec2 direction = glm::vec2(1.0f, 0.0f);
    double time = 0.0f;
};

#endif
