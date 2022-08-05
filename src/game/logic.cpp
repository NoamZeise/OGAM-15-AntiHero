#include "logic.h"

GameLogic::GameLogic(Render *render, Camera::RoomFollow2D *cam2D)
{
  Resource::Font mapFont = render->LoadFont("textures/Roboto-Black.ttf"); 
  levels.push_back(
		   Level(render, "maps/test.tmx", mapFont)
		   );
  currentLevel = levels[0];
  player = Player(
		  
		  );
  hero = Hero(
		   Sprite(glm::vec2(0, 0), render->LoadTexture("textures/player.png"))
		   );
  enemy = Enemy(Sprite(glm::vec2(0, 0), render->LoadTexture("textures/enemy.png")));
  stone = god::Stone(Sprite(glm::vec2(0, 0), render->LoadTexture("textures/stone.png")));
  
  LoadMap(cam2D);
}

void GameLogic::Update(glm::vec4 camRect, Timer &timer, Input &input, Camera::RoomFollow2D *cam2D, glm::vec2 mousePos)
{
  std::vector<glm::vec4> frameColliders;
  currentLevel.Update(camRect, timer, &frameColliders);
  hero.Update(camRect, timer);
  for(auto& e: enemies)
    e.Update(camRect, timer);
  for(int stoneI = 0; stoneI < stones.size(); stoneI++)
  {
      stones[stoneI].Update(camRect, timer);
      auto hit = stones[stoneI].hit();
      if(hit != glm::vec4(0))
      {
	  for(int i = 0; i < enemies.size(); i++)
	      if(gh::colliding(hit, enemies[i].getHitBox()))
		  enemies.erase(enemies.begin() + i--);
	  stones.erase(stones.begin() + stoneI--);
      }
  }
  if(input.Keys[GLFW_KEY_H] && !prevInput.Keys[GLFW_KEY_H])
  {
      god::Stone s = stone;
      s.setPos(glm::vec2(mousePos.x + cam2D->getCameraOffset().x,
			 mousePos.y + cam2D->getCameraOffset().y));
      stones.push_back(s);
  }

  prevInput = input;
}

void GameLogic::Draw(Render *render)
{
  hero.Draw(render);
  currentLevel.Draw(render);
  for(auto& e: enemies)
    e.Draw(render);
  for(auto& s: stones)
    s.Draw(render);
}

glm::vec2 GameLogic::getTarget()
{
  return this->hero.getPos();
}

void GameLogic::LoadMap(Camera::RoomFollow2D *cam2D)
{
  cam2D->setCameraMapRect(currentLevel.getMapRect());
  std::vector<glm::vec4> roomRects;
  std::vector<glm::vec2> heroPath;
  std::vector<std::vector<glm::vec2>> enemypaths;
  currentLevel.getObjLists(&roomRects, &heroPath, &enemypaths);
  cam2D->setCameraRects(roomRects);
  hero.setPath(heroPath);
  enemies.clear();
  for(auto& p: enemypaths)
  {
    Enemy e = enemy;
    e.setPath(p);
    enemies.push_back(e);
  }
}
