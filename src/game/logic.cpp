#include "logic.h"
#include "gamehelper.h"

GameLogic::GameLogic(Render *render, Camera::RoomFollow2D *cam2D)
{
  Resource::Font mapFont = render->LoadFont("textures/Roboto-Black.ttf"); 
  levels.push_back(
		   Level(render, "maps/testNewTiles.tmx", mapFont)
		   );
  currentLevel = levels[0];
  player = Player(
		  
		  );
  hero = Hero(
		   Sprite(glm::vec2(0, 0), render->LoadTexture("textures/characters/player.png"))
		   );
  enemy = Enemy(Sprite(glm::vec2(0, 0), render->LoadTexture("textures/characters/enemy.png")));
  obstacle = Obstacle(Sprite(glm::vec2(0, 0), render->LoadTexture("textures/obstacle.png")));
  stone = god::Stone(Sprite(glm::vec2(0, 0), render->LoadTexture("textures/spells/stone.png")));
  checkpoint = Sprite(glm::vec2(0, 0), render->LoadTexture("textures/checkpoint.png"));
  checkpoint.depth = 0.05f;
  LoadMap(cam2D);

  spellControls = SpellControls(render);

  spellControls.setCards(
			 {
			     Spells::Stone, Spells::Stone, Spells::Stone, Spells::Stone
			     , Spells::Stone, Spells::Stone
			 }
			 );
}

void GameLogic::Update(glm::vec4 camRect, Timer &timer, Input &input, Camera::RoomFollow2D *cam2D, glm::vec2 mousePos)
{
  std::vector<glm::vec4> frameColliders;
  spellControls.Update(camRect, timer, input, mousePos);
  spellCast(spellControls.spellCast().first, spellControls.spellCast().second);
  currentLevel.Update(camRect, timer, &frameColliders);
  hero.Update(camRect, timer);
  if(hero.isFinished())
      LoadMap(cam2D);
  for(auto& e: enemies)
  {
      e.Update(camRect, timer, gh::centre(hero.getHitBox()));
      if(gh::colliding(e.getHitBox(), hero.getHitBox()))
	  playerDeath(cam2D);
  }
  for(auto& o: obstacles)
  {
      o.Update(camRect, timer);
      if(gh::colliding(o.getHitBox(), hero.getHitBox()))
	  hero.setRectToPrev();
      for(auto& e: enemies)
	  if(gh::colliding(o.getHitBox(), e.getHitBox()))
	      e.setRectToPrev();
  }
  for(int i = 0; i < checkpoints.size(); i++)
  {
      checkpoints[i].UpdateMatrix(camRect);
      if (gh::colliding(checkpoints[i].rect, hero.getHitBox()))
      {
        lastCheckpoint = &checkpoints[i];
	checkpointTargetIndex = hero.getTargetIndex();
	checkpointSpells = spellControls.getSpells();
      }
  }
  spellUpdate(camRect, timer);

  prevInput = input;
}

void GameLogic::Draw(Render *render)
{
  currentLevel.Draw(render);
  hero.Draw(render);
  for(auto& e: enemies)
    e.Draw(render);
  for(auto& s: stones)
    s.Draw(render);
  for(auto& o: obstacles)
      o.Draw(render);
  for(auto& c: checkpoints)
      c.Draw(render);
  spellControls.Draw(render);
}

glm::vec2 GameLogic::getTarget()
{
  return this->hero.getPos();
}

void GameLogic::LoadMap(Camera::RoomFollow2D *cam2D)
{
  cam2D->setCameraMapRect(currentLevel.getMapRect());
  
  Level::MapGameplayObjects mapObjs = currentLevel.getObjLists();
  cam2D->setCameraRects(mapObjs.roomRects);
  hero.setPath(mapObjs.heroPath);
  enemies.clear();
  for(auto& p: mapObjs.enemyPaths)
  {
    Enemy e = enemy;
    e.setPath(p);
    enemies.push_back(e);
  }
  obstacles.clear();
  for(auto& o: mapObjs.obstacles)
  {
      Obstacle obs = obstacle;
      obs.setRect(o);
      obstacles.push_back(obs);
  }
  checkpoints.clear();
  lastCheckpoint = nullptr;
  for(auto &c: mapObjs.checkpoints)
  {
      auto cp = checkpoint;
      cp.rect = c;
      checkpoints.push_back(cp);
  }
}

void GameLogic::playerDeath(Camera::RoomFollow2D *cam2D)
{
    glm::vec4 cp = lastCheckpoint == nullptr ? glm::vec4(0) : lastCheckpoint->rect;
    LoadMap(cam2D);
    if(cp != glm::vec4(0))
    {
	hero.setCheckpoint(glm::vec2(cp.x, cp.y), checkpointTargetIndex);
    }
    spellControls.setCards(checkpointSpells);
}


void GameLogic::spellUpdate(glm::vec4 camRect, Timer &timer)
{
    for(int stoneI = 0; stoneI < stones.size(); stoneI++)
  {
      stones[stoneI].Update(camRect, timer);
      auto hit = stones[stoneI].hit();
      if(hit != glm::vec4(0))
      {
	  //stone makes sound for enemies
	  for(int i = 0; i < enemies.size(); i++)
	      enemies[i].soundEvent(gh::centre(hit));

	  //stone destroys box
	  for(int i = 0; i < obstacles.size(); i++)
	      if(gh::colliding(hit, obstacles[i].getHitBox()))
		  obstacles.erase(obstacles.begin() + i--);
	  stones.erase(stones.begin() + stoneI--);
      }
  }
}

void GameLogic::spellCast(Spells spell, glm::vec2 pos)
{
    switch (spell)
  {
      case Spells::Stone:
	  god::Stone s = stone;
	  s.setPos(glm::vec2(pos));
	  stones.push_back(s);
	  break;

  }

}
