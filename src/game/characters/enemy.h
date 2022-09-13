#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include "audio.h"
#include "character.h"
#include "gamehelper.h"
#include "glm/geometric.hpp"

namespace
{    
    const float DISTRACTION_RANGE = 250.0f;
    const float INVESTIGATION_INITIATION_PROXIMITY = 10.0f;
    const float INVESTIGATION_DURATION = 6200.0f;
    const float PLAYER_CHASE_RADIUS = 250.0f;
    const float PLAYER_CHASE_OUT_OF_VIEW_RADIUS = 110.0f;
    const float PLAYER_SIZE_OFFSET = 0.0f;
    const float ENEMY_PATROL_SPEED = 0.025f;
    const float ENEMY_INVESTIGATE_SPEED = 0.03f;
    const float ENEMY_CHASE_SPEED = 0.12f;
    const float ENEMY_POV_ANGLE = 40.0f;
    const float ENEMY_FOOTSTEP_MAX = 1.8f;
    const float ENEMY_AUDIBLE_DISTANCE = 2.0f;
    const float STANDING_SEARCH_RANGE = 100.0f;
    const float CONFUSED_SEARCH_RANGE = 180.0f;
} // namespace

class Enemy : public Character
{
 public:
  Enemy() {}
    Enemy(Sprite sprite, Sprite distracted, Sprite circle, Sprite search, Audio::Manager* audio) : Character(sprite, circle, audio)
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

      for( int i = 1; i < 6; i++)
	  audio->LoadAudioFile("audio/SFX/Footsteps/Footstep Heavy" + std::to_string(i) + ".wav");
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
	search.rect.z = PLAYER_CHASE_RADIUS * 2.0f;
	search.rect.w = PLAYER_CHASE_RADIUS * 2.0f;
	outOfview.rect.z = PLAYER_CHASE_OUT_OF_VIEW_RADIUS * 2.0f;
	outOfview.rect.w = PLAYER_CHASE_OUT_OF_VIEW_RADIUS * 2.0f;
	if(currentSpeed > speed) { currentSpeed = speed; }
	if(currentState == EnemyState::Patrol)
	{
	    searchRange = 30.0f;
	    speed = ENEMY_PATROL_SPEED;
	    Character::Update(camRect, timer);
	    direction = glm::normalize(lastToTarget);
	}
	else if(currentState == EnemyState::Investigate)
	{
	    searchRange = 25.0f;
	    speed = ENEMY_INVESTIGATE_SPEED;
	    glm::vec2 toTarget = investigationTarget - gh::centre(sprite.rect);
	    direction = glm::normalize(toTarget);
	    float length = glm::length(toTarget);
	    if(length > INVESTIGATION_INITIATION_PROXIMITY)
		moveToTarget(toTarget, timer);
	    else
	    {
		//wobble search
		searchRange = STANDING_SEARCH_RANGE;
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
	    searchRange = 5.0f;
	    speed = ENEMY_CHASE_SPEED;
	    glm::vec2 toTarget = playerPos - gh::centre(sprite.rect);
	    direction = glm::normalize(toTarget);
	    moveToTarget(toTarget, timer);
	    sprite.UpdateMatrix(camRect);
	    currentState = EnemyState::Investigate;
	    investigationTarget = playerPos;
	    investigationTimer = 0.0f;
	}
	else if(currentState == EnemyState::Confused)
	{
	    currentState = EnemyState::Patrol;
	    searchRange = CONFUSED_SEARCH_RANGE;
	    search.rect.z = PLAYER_CHASE_RADIUS * 0.5f;
	    search.rect.w = PLAYER_CHASE_RADIUS * 0.5f;
	    outOfview.rect.z = PLAYER_CHASE_OUT_OF_VIEW_RADIUS * 0.1f;
	    outOfview.rect.w = PLAYER_CHASE_OUT_OF_VIEW_RADIUS * 0.1f;
	}

	distracted.rect.x = sprite.rect.x + (sprite.rect.z - distracted.rect.z)/2.0f;
	distracted.rect.y = sprite.rect.y - distracted.rect.w;
	distracted.UpdateMatrix(camRect);
	
	//circle.rect.x = (sprite.rect.x + sprite.rect.z/2.0f) - circle.rect.z/2.0f;
	//circle.rect.y = (sprite.rect.y + sprite.rect.w/2.0f) - circle.rect.w/2.0f;
	//circle.spriteColour = glm::vec4(0.9f, 0.9f, 0.2f, 0.2f);
	//circle.UpdateMatrix(camRect);
	
	search.rect.x = (sprite.rect.x + sprite.rect.z/2.0f) - search.rect.z/2.0f;
	search.rect.y = (sprite.rect.y + sprite.rect.w/2.0f) - search.rect.w/2.0f;
	search.spriteColour = glm::vec4(0.9f, 0.2f, 0.2f, 0.2f);
	float rot = atan2(direction.y, direction.x);
	
	if(rot < 3.1415/2.0 && rot > -3.1415/2.0)
	    sprite.texOffset = glm::vec4(0, 0, 1, 1);
	else
	    sprite.texOffset = glm::vec4(0, 0, -1, 1);
	
	float trueAngle = rot*180.0/3.14159265 + 90.0f
	    + sin(time*0.001f)*searchRange;

	float diff = fmod(trueAngle - currentAngle, 360.0f);
	if(diff > 180.0f)
	    diff = -(360.0f - diff);
	if(diff < -180.0f)
	    diff = -(-360.0f - diff);
	currentAngle += diff*turnSpeed*timer.FrameElapsed();
	currentAngle = fmod(currentAngle, 360.0f);
	currentAngle = currentAngle < 0.0f ? currentAngle + 360.0f : currentAngle;
	
	if(firstAngleUpdate)
	{
	    firstAngleUpdate = false;
	    currentAngle = trueAngle;
	}
	search.rotate = currentAngle;

	search.UpdateMatrix(camRect);
	
	outOfview.rect.x = (sprite.rect.x + sprite.rect.z/2.0f) - outOfview.rect.z/2.0f;
	outOfview.rect.y = (sprite.rect.y + sprite.rect.w/2.0f) - outOfview.rect.w/2.0f;
	outOfview.spriteColour = glm::vec4(0.9f, 0.2f, 0.2f, 0.2f);
	outOfview.UpdateMatrix(camRect);

	float distToPlayer = glm::distance(gh::centre(sprite.rect), playerPos);
	if(distToPlayer < PLAYER_CHASE_RADIUS + PLAYER_SIZE_OFFSET)
	{
	    glm::vec2 toPlayer = glm::vec2(playerPos) - gh::centre(sprite.rect);
	    toPlayer = glm::normalize(toPlayer);
	    float angle = atan2(toPlayer.y, toPlayer.x)*180.0/3.14159265 + 90.0f;
	    angle = fmod(angle, 360.0f);
	    if(angle < 0.0f)
		angle += 360.0f;
     
	    //if(abs(angle - search.rotate) > 360.0f)
	    //{
	    //angle -= 360.0f;
	    //}
	    //std::cout << angle << std::endl;
	    //std::cout << search.rotate << std::endl;
	    float angle_diff = abs(angle - search.rotate);
	    if(angle_diff < ENEMY_POV_ANGLE || angle_diff > 360.0f - ENEMY_POV_ANGLE || distToPlayer < PLAYER_CHASE_OUT_OF_VIEW_RADIUS || currentState == EnemyState::Chase)
		currentState = EnemyState::Chase;
	}
	 //else
	 // {
	 //    currentState = EnemyState::Patrol;
	 //}
	if(distToPlayer < PLAYER_CHASE_RADIUS*ENEMY_AUDIBLE_DISTANCE &&
	   searchRange != STANDING_SEARCH_RANGE &&
	   searchRange != CONFUSED_SEARCH_RANGE)
	{
	    footstepTimer += timer.FrameElapsed();
	    if(footstepTimer > 800.0f - speed*5000.0f)
	    {
		footstepTimer = 0.0f;
		float sfxVolume = 1.0f - (distToPlayer / (PLAYER_CHASE_RADIUS*ENEMY_AUDIBLE_DISTANCE));
		//	std::cout << sfxVolume * sfxVolume << std::endl;
		audio->Play("audio/SFX/Footsteps/Footstep Heavy" + rand.stringNum(5) + ".wav", false, ENEMY_FOOTSTEP_MAX * sfxVolume * sfxVolume);
	    }
	}

	 collided = false;
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

    void smokeEvent(glm::vec4 smoke)
    {
	if(gh::colliding(getHitBox(), smoke))
	{
	    currentState = EnemyState::Confused;
	}
    }

    void DrawTransparent(Render *render)
    {
	if(currentState == EnemyState::Investigate)
	    distracted.Draw(render);
	//circle.Draw(render);
	outOfview.Draw(render);
	search.Draw(render);
    }

    bool isChasable(glm::vec2 playerPos)
    {
	float distToPlayer = glm::distance(gh::centre(sprite.rect), playerPos);
	if(distToPlayer < PLAYER_CHASE_RADIUS)
	{
	    glm::vec2 toPlayer = glm::vec2(playerPos) - glm::vec2(sprite.rect.x, sprite.rect.y);
	    toPlayer = glm::normalize(toPlayer);
	    float angle = atan2(toPlayer.y, toPlayer.x)*180.0/3.14159265 + 90.0f;
	    angle = fmod(angle, 360.0f);
	    if(angle < 0.0f)
		angle += 360.0f;
     
	    //if(abs(angle - search.rotate) > 360.0f)
	    //{
	    //angle -= 360.0f;
	    //}
	    //std::cout << angle << std::endl;
	    //std::cout << search.rotate << std::endl;
	    if(abs(angle - search.rotate) < 40.0f || distToPlayer < PLAYER_CHASE_OUT_OF_VIEW_RADIUS || currentState == EnemyState::Chase)
		return true;
	}
	return false;

    }

private:

    enum EnemyState
	{
	    Patrol,
	    Investigate,
	    Chase,
	    Confused,
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
    float currentAngle = 90.0f;
    bool firstAngleUpdate = true;
    float turnSpeed = 0.005f;
    float searchRange = 25.0f;
    double time = 0.0f;
    float footstepTimer = 0.0f;
};

#endif
