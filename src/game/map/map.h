#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "visual.h"
#include "tiled.h"

#include <string>
#include <fstream>
#include "../controls/card/spell.h"


class Level
{
 public:
  Level() {}
  Level(Render *render, std::string file, Resource::Font mapFont)
  {
    this->tiledMap = tiled::Map(file + ".tmx");
    this->visualMap = Map::Visual(tiledMap, render, mapFont);
    std::ifstream in(file + ".cards");
    for(std::string line; std::getline(in, line);)
    {
	std::string card;
	std::string count;
	bool getCount = false;
	for(auto& c :line)
	{
	    if(c == ' ')
		getCount = true;
	    else if(getCount)
		count.push_back(c);
	    else
		card.push_back(c);
	}
	int cardCount = std::stoi(count);
	std::cout << cardCount << std::endl;
	for(int i = 0; i < cardCount; i++)
	{
	    if(card == "stone")
		spells.push_back(Spells::Stone);
	    else if(card == "wait")
		spells.push_back(Spells::Wait);
	    else if(card == "wind")
		spells.push_back(Spells::Wind);
	    else if(card == "restart")
		spells.push_back(Spells::Restart);
	    else
		std::cout << "warning : card name not recognized!\n";
	}
    }
  }

  void Update(glm::vec4 camRect, Timer &timer, std::vector<glm::vec4> *activeColliders)
  {
    visualMap.Update(camRect, timer, activeColliders);
  }

  void Draw(Render* render)
  {
    visualMap.Draw(render);
  }

  glm::vec4 getMapRect()
  {
    return glm::vec4(0, 0, tiledMap.width * tiledMap.tileWidth, tiledMap.height * tiledMap.tileHeight);
  }

  struct MapGameplayObjects
  {
      std::vector<glm::vec4> roomRects;
      std::vector<glm::vec2> heroPath;
      std::vector<std::vector<glm::vec2>> enemyPaths;
      std::vector<glm::vec4> obstacles;
      std::vector<glm::vec4> checkpoints;
  };

  MapGameplayObjects getObjLists()
  {
      MapGameplayObjects mapObjs;
    for(auto& objGroup : tiledMap.objectGroups)
    {
      for(auto& obj: objGroup.objs)
      {
	if(obj.props.room || objGroup.props.room)
	{
	    mapObjs.roomRects.push_back(glm::vec4(obj.x, obj.y, obj.w, obj.h));
        }
	if(obj.props.obstacle || objGroup.props.obstacle)
	{
	    mapObjs.obstacles.push_back(glm::vec4(obj.x, obj.y, obj.w, obj.h));
	}
	if(obj.props.checkpoint || objGroup.props.checkpoint)
	{
	    mapObjs.checkpoints.push_back(glm::vec4(obj.x, obj.y, obj.w, obj.h));
	}
      }
      for(auto& obj: objGroup.polys)
      {
	std::vector<glm::vec2> *pointsList = nullptr;
	if(obj.obj.props.hero || objGroup.props.hero)
	{
	    pointsList = &mapObjs.heroPath;
	}
	if(obj.obj.props.enemy || objGroup.props.enemy)
	{
	  mapObjs.enemyPaths.push_back({});
	  pointsList = &mapObjs.enemyPaths.at(mapObjs.enemyPaths.size() - 1);
	}
	  
	if(pointsList != nullptr)
	{
	  for(auto &p: obj.points)
	  {
	    pointsList->push_back(glm::vec2(p.x + obj.obj.x, p.y + obj.obj.y));
	  }
	}
      }
    }
    return mapObjs;
  }

    std::vector<Spells> getSpells() { return spells; }
  
 private:
  Map::Visual visualMap;
  tiled::Map tiledMap;
    std::vector<Spells> spells;
};


#endif
