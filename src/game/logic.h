#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <timer.h>
#include <input.h>
#include <gamehelper.h>

#include "player.h"
#include "../renderer_include.h"
#include "../camera.h"
#include "map/map.h"

#include <vector>
#include <iostream>

class GameLogic
{
 public:
  GameLogic() {}
  GameLogic(Render *render, Camera::RoomFollow2D *cam2D);
  void Update(glm::vec4 camRect, Timer &timer, Input &input, Camera::RoomFollow2D *cam2D);
  void Draw(Render *render);
  glm::vec2 getTarget();

 private:
  void LoadMap(Camera::RoomFollow2D *cam2D);
  
  std::vector<Level> levels;
  int currentLevelIndex = 0;
  Level currentLevel;
  Player player;
};

#endif
