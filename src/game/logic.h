#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <timer.h>
#include <input.h>
#include <gamehelper.h>
#include <audio.h>

#include "player.h"
#include "../renderer_include.h"
#include "../camera.h"
#include "map/map.h"
#include "characters/hero.h"
#include "characters/enemy.h"
#include "characters/obstacle.h"

#include "god/stone.h"

#include "controls/spell_controls.h"

#include <vector>
#include <iostream>

class GameLogic
{
 public:
    GameLogic() {}
    GameLogic(Render *render, Camera::RoomFollow2D *cam2D, Audio::Manager* audioManager);
    void Update(glm::vec4 camRect, Timer &timer, Input &input, Camera::RoomFollow2D *cam2D, glm::vec2 mousePos);
    void Draw(Render *render);
    glm::vec2 getTarget();
    bool gameComplete() { return currentLevelIndex >= levels.size(); }

 private:
    void LoadMap(Camera::RoomFollow2D *cam2D);
    void playerDeath(Camera::RoomFollow2D *cam2D);
    void levelComplete(Camera::RoomFollow2D *cam2D);
    void spellCast(Spells spell, glm::vec2 pos, Camera::RoomFollow2D* cam2D);
    void spellUpdate(glm::vec4 camRect, Timer &timer);

    
  std::vector<Level> levels;
    Button restartBtn;
  int currentLevelIndex = 0;
  Level currentLevel;
  Player player;
  Hero hero;
  Enemy enemy;
  std::vector<Enemy> enemies;
    Obstacle obstacle;
    std::vector<Obstacle> obstacles;
  god::Stone stone;
  std::vector<god::Stone> stones;
    Sprite pickupSprite;
    std::vector<std::pair<Pickup, Sprite>> pickups;
    Sprite gold;

    Sprite checkpoint;
    std::vector<Sprite> checkpoints;
    Sprite* lastCheckpoint;
    int checkpointTargetIndex;
    std::vector<Spells> checkpointSpells;
    std::vector<Obstacle> checkpointObstacles;
    std::vector<Enemy> checkpointEnemies;
    bool checkpointGotGold = false;
    
    Input prevInput;

    SpellControls spellControls;
    Audio::Manager* audio;

    float lastScale = 1.0f;

    Sprite* currentCursor;
    Sprite defaultCursor;
    Sprite targetCursor;

    bool gotGold = false;
};

#endif
