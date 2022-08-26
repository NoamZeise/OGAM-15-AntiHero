#include "logic.h"
#include "config.h"
#include "gamehelper.h"

GameLogic::GameLogic(Render *render, Camera::RoomFollow2D *cam2D, Audio::Manager* audioManager)
{
     this->audio = audioManager;
     audioManager->Play("audio/Robin Hood Medieval Music2.ogg", true, 0.5f);

     defaultCursor = Sprite(render->LoadTexture("textures/UI/cursor/default.png"));
     defaultCursor.depth = 3.0f;
     defaultCursor.rect.z *= 0.3f;
     defaultCursor.rect.w *= 0.3f;
     targetCursor = Sprite(render->LoadTexture("textures/UI/cursor/target.png"));
     targetCursor.depth = 3.0f;
     targetCursor.rect.z *= 0.3f;
     targetCursor.rect.w *= 0.3f;
     
     Resource::Font mapFont = render->LoadFont("textures/Roboto-Black.ttf");
     
     levels.push_back(
		      Level(render, "maps/level1", mapFont)
		      );
     currentLevel = levels[currentLevelIndex];
     player = Player(
		     
		     );
     hero = Hero(
		 Sprite(render->LoadTexture("textures/characters/player.png")),
		 Sprite(render->LoadTexture("textures/UI/circle.png"))
		   );
     enemy = Enemy(
		   Sprite(render->LoadTexture("textures/characters/enemy.png")),
		   Sprite(render->LoadTexture("textures/UI/distracted.png")),
		   Sprite(render->LoadTexture("textures/UI/circle.png")),
		   Sprite(render->LoadTexture("textures/UI/search.png")));
     obstacle = Obstacle(Sprite(render->LoadTexture("textures/obstacle.png")));
     stone = god::Stone(Sprite(render->LoadTexture("textures/spells/stone.png")));
     checkpoint = Sprite(render->LoadTexture("textures/checkpoint.png"));
     checkpoint.depth = 0.05f;
     spellControls = SpellControls(render);

     pickupSprite = Sprite(render->LoadTexture("textures/spells/pickup.png"));
     pickupSprite.depth = CHARACTER_DEPTH + 0.01f;

     Sprite restartBtnSprite = Sprite(glm::vec2(10.0f, 10.0f),render->LoadTexture("textures/UI/restart.png"));
     restartBtnSprite.depth = 2.0f;
     restartBtnSprite.rect.z *= 0.4f;
     restartBtnSprite.rect.w *= 0.4f;
     restartBtn = Button(restartBtnSprite, true);

     gold = Sprite(render->LoadTexture("textures/gold.png"));
     gold.depth = CHARACTER_DEPTH;
				
     LoadMap(cam2D);
}

void GameLogic::Update(glm::vec4 camRect, Timer &timer, Input &input, Camera::RoomFollow2D *cam2D, glm::vec2 mousePos)
{
    lastScale = camRect.z / settings::TARGET_WIDTH;
    std::vector<glm::vec4> frameColliders;
    spellControls.Update(camRect, timer, input, mousePos);
    spellCast(spellControls.spellCast().first, spellControls.spellCast().second, cam2D);
    currentLevel.Update(camRect, timer, &frameColliders);
    hero.Update(camRect, timer);
    if(hero.isFinished())
	levelComplete(cam2D);
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
    for(int i = 0; i < pickups.size(); i++)
    {
	pickups[i].second.UpdateMatrix(camRect);        ;
	if(gh::colliding(pickups[i].first.rect, hero.getHitBox()))
	{
	    std::vector<Spells> toAdd;

	    for(int s = 0; s < pickups[i].first.spells.size(); s++)
	    {
		for(int k = 0; k < pickups[i].first.spells[s].second; k++)
		    toAdd.push_back(pickups[i].first.spells[s].first);
	    }
	    spellControls.addCards(toAdd);
	    pickups.erase(pickups.begin() + i--);
	}
    }
    if(!gotGold)
    {
        gold.UpdateMatrix(camRect);
	if(gh::colliding(hero.getHitBox(), gold.rect))
	    gotGold = true;
    }
    for(int i = 0; i < checkpoints.size(); i++)
    {
	checkpoints[i].UpdateMatrix(camRect);
	if(lastCheckpoint != &checkpoints[i])
	    if (gh::colliding(checkpoints[i].rect, hero.getHitBox()))
	    {
		lastCheckpoint = &checkpoints[i];
		checkpointTargetIndex = hero.getTargetIndex();
		checkpointSpells = spellControls.getSpells();
		checkpointObstacles.clear();
		for(int o = 0; o < obstacles.size(); o++)
		    checkpointObstacles.push_back(obstacles[o]);
		checkpointEnemies.clear();
		for(int c = 0; c < enemies.size(); c++)
		    checkpointEnemies.push_back(enemies[c]);
		checkpointGotGold = gotGold;
	    }
    }
    spellUpdate(camRect, timer);
    
    if(spellControls.isTargeting())
    {
	targetCursor.rect.x = mousePos.x + camRect.x - targetCursor.rect.z / 2.0f;
	targetCursor.rect.y = mousePos.y + camRect.y - targetCursor.rect.w / 2.0f;
	targetCursor.UpdateMatrix(camRect);
	currentCursor = &targetCursor;
  }
    else
  {
      defaultCursor.rect.x = mousePos.x + camRect.x;
      defaultCursor.rect.y = mousePos.y + camRect.y;
      defaultCursor.UpdateMatrix(camRect);
      currentCursor = &defaultCursor;
  }
    restartBtn.Update(camRect, input, mousePos);
    if(restartBtn.Clicked())
	LoadMap(cam2D);
    prevInput = input;
}

void GameLogic::Draw(Render *render)
{
  currentLevel.Draw(render);
  hero.Draw(render);
  for(auto& s: stones)
    s.Draw(render);
  for(auto& o: obstacles)
      o.Draw(render);
  for(auto& pu: pickups)
      pu.second.Draw(render);
  if(!gotGold)
      gold.Draw(render);
  for(auto& c: checkpoints)
      c.Draw(render);
  for(auto& e: enemies)
      e.Draw(render);
  spellControls.Draw(render);
  currentCursor->Draw(render);
  restartBtn.Draw(render);
}

glm::vec2 GameLogic::getTarget()
{
    auto target = hero.getPos();
    const float CARD_OFFSET = 200.0f;
    target.y += CARD_OFFSET * lastScale;
    return target;
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
  
  spellControls.setCards({Spells::Restart, Spells::Wait});
  pickups.clear();
  for(auto &pu: mapObjs.pickups)
  {
      Sprite puS = pickupSprite;
      puS.rect = pu.rect;
      pickups.push_back(std::pair<Pickup, Sprite>(pu, puS));
  }
  gold.rect = mapObjs.gold;
}

void GameLogic::playerDeath(Camera::RoomFollow2D *cam2D)
{
    glm::vec4 cp = lastCheckpoint == nullptr ? glm::vec4(0) : lastCheckpoint->rect;
    LoadMap(cam2D);
    if(cp != glm::vec4(0))
    {
	hero.setCheckpoint(glm::vec2(cp.x, cp.y), checkpointTargetIndex);
	spellControls.setCards(checkpointSpells);
	obstacles.clear();
	for(int o = 0; o < checkpointObstacles.size(); o++)
	    obstacles.push_back(checkpointObstacles[o]);
	enemies.clear();
	for(int c = 0; c < checkpointEnemies.size(); c++)
	    enemies.push_back(checkpointEnemies[c]);
	gotGold = checkpointGotGold;
    }
}

void GameLogic::levelComplete(Camera::RoomFollow2D *cam2D)
{
    currentLevelIndex++;
    if(currentLevelIndex<levels.size())
    {
	currentLevel = levels[currentLevelIndex];
	LoadMap(cam2D);
    }
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

void GameLogic::spellCast(Spells spell, glm::vec2 pos, Camera::RoomFollow2D* cam2D)
{
    god::Stone s = stone;
    switch (spell)
    {
    case Spells::Stone:
	s.setPos(glm::vec2(pos));
	stones.push_back(s);
	break;
    case Spells::Wait:
	hero.Wait();
	break;
    case Spells::Go:
	hero.Go();
	break;
    case Spells::Restart:
	playerDeath(cam2D);
	break;
  }

}
