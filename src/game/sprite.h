#ifndef GAME_SPRITE_H
#define GAME_SPRITE_H

#include <glmhelper.h>
#include "../renderer_include.h"


struct Sprite
{
  Resource::Texture texture;
  glm::vec4 spriteRect;
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  glm::vec4 spriteColour = glm::vec4(1.0f);
  float rotate = 0.0f;
  float depth = 0.0f;
  
  Sprite()
  {
    spriteRect = glm::vec4(0);
  }

  Sprite(glm::vec4 rect, Resource::Texture texture)
  {
    spriteRect = rect;
    this->texture = texture;
    UpdateMatrix();
  }

  void UpdateMatrix()
  {
    modelMatrix = glmhelper::calcMatFromRect(spriteRect, rotate, depth);
  }

  void Draw(Render* render)
  {
    render->DrawQuad(texture, modelMatrix, spriteColour);
  }
  
};

#endif
