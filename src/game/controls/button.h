#ifndef BUTTON_H
#define BUTTON_H

#include "../sprite.h"
#include "GLFW/glfw3.h"
#include "config.h"

#include <input.h>

const glm::vec4 SELECT_COLOUR = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);

class Button
{
public:
  Button() {}
    Button(Sprite sprite, bool isStatic) {
	this->sprite = sprite;
	this->activeSprite = sprite;
	this->activeSprite.spriteColour = SELECT_COLOUR;
	this->initialRect = sprite.rect;
	this->isStatic = isStatic;
    }
    Button(Sprite sprite, Sprite activeSprite, bool isStatic)
    {
	this->sprite = sprite;
	this->activeSprite = activeSprite;
	this->isStatic = isStatic;
    }
  virtual void Update(glm::vec4 camRect, Input &input, glm::vec2 mousePos)
{
    if(isStatic)
  {
      float scale = camRect.z / (float)settings::TARGET_WIDTH;
      sprite.rect = initialRect * scale;
      sprite.rect.x += camRect.x;
      sprite.rect.y += camRect.y;
      //std::cout << sprite.rect.x << std::endl;
      activeSprite.rect = sprite.rect;
      mousePos.x += camRect.x;
      mousePos.y += camRect.y;
  }
  sprite.UpdateMatrix(camRect);
  activeSprite.UpdateMatrix(camRect);
  
  prevClicked = clicked;
  clicked = input.Buttons[GLFW_MOUSE_BUTTON_LEFT];
  if(gh::contains(mousePos, glm::vec4(sprite.rect.x + horizontalHitboxOffset/2.0f, sprite.rect.y, sprite.rect.z - horizontalHitboxOffset, sprite.rect.w)))
  {
      onSprite = true;
  }
  else
  {
      onSprite = false;
  }
}

    virtual void Draw(Render *render) { if(!onSprite) sprite.Draw(render); else activeSprite.Draw(render); }
  bool Clicked()  { return !prevClicked && clicked && onSprite; }
  virtual void setInitialRect(glm::vec4 rect) 
  {
    initialRect = rect;
  }

protected:
  Sprite sprite;
  Sprite activeSprite;
  bool prevClicked = true;
  bool clicked = false;
  bool onSprite = false;
  bool isStatic;
  glm::vec4 initialRect;
  float horizontalHitboxOffset = 0.0f;
};

#endif
