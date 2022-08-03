#include "logic.h"

GameLogic::GameLogic(Render *render, Camera::RoomFollow2D *cam2D)
{
  Resource::Font mapFont = render->LoadFont("textures/Roboto-Black.ttf"); 
  levels.push_back(
		   Level(render, "maps/test.tmx", mapFont)
		   );
  currentLevel = levels[0];
  player = Player(
		  Sprite(glm::vec2(0, 0), render->LoadTexture("textures/player.png"))
		  );
  LoadMap(cam2D);
}

void GameLogic::Update(glm::vec4 camRect, Timer &timer, Input &input, Camera::RoomFollow2D *cam2D)
{
  std::vector<glm::vec4> frameColliders;
  currentLevel.Update(camRect, timer, &frameColliders);
  player.Update(camRect, timer, input);
}

void GameLogic::Draw(Render *render)
{
  player.Draw(render);
  currentLevel.Draw(render);
}

glm::vec2 GameLogic::getTarget()
{
  return this->player.getPos();
}

void GameLogic::LoadMap(Camera::RoomFollow2D *cam2D)
{
  cam2D->setCameraMapRect(currentLevel.getMapRect());
  std::vector<glm::vec4> roomRects;
  std::vector<glm::vec2> heroPath;
  currentLevel.getObjLists(&roomRects, &heroPath);
  cam2D->setCameraRects(roomRects);
}
