#ifndef GAME_SPRITE_H
#define GAME_SPRITE_H

#include <glmhelper.h>
#include "../renderer_include.h"
#include "gamehelper.h"
#include "glm/fwd.hpp"


struct Sprite
{
  Resource::Texture texture;
  glm::vec4 rect;
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  glm::vec4 spriteColour = glm::vec4(1.0f);
  float rotate = 0.0f;
  float depth = 0.0f;
  bool toDraw = true;
  
  Sprite()
  {
    rect = glm::vec4(0);
  }

  Sprite(glm::vec4 rect, Resource::Texture texture)
  {
    this->rect = rect;
    this->texture = texture;
  }

  Sprite(glm::vec2 pos, Resource::Texture texture)
  {
    rect = glm::vec4(pos.x, pos.y, texture.dim.x, texture.dim.y);
    this->texture = texture;
  }

  Sprite(Resource::Texture texture)
  {
    rect = glm::vec4(0, 0, texture.dim.x, texture.dim.y);
    this->texture = texture;
  }

  void UpdateMatrix(glm::vec4 cameraRect)
  {
    toDraw = gh::colliding(cameraRect, rect);
    if(toDraw)
      modelMatrix = glmhelper::calcMatFromRect(rect, rotate, depth);
  }

  void Draw(Render* render)
  {
    if(toDraw)
      render->DrawQuad(texture, modelMatrix, spriteColour);
  }
  
};

#endif
