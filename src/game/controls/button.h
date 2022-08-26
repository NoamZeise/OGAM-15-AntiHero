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
  Button(Sprite sprite, bool isStatic) { this->sprite = sprite; this->initialRect = sprite.rect; this->isStatic = isStatic; }
  virtual void Update(glm::vec4 camRect, Input &input, glm::vec2 mousePos)
{
    if(isStatic)
  {
      float scale = camRect.z / (float)settings::TARGET_WIDTH;
      sprite.rect = glm::vec4((int)(initialRect.x*scale + camRect.x), (int)(initialRect.y*scale + camRect.y),(int)( initialRect.z*scale), (int)(initialRect.w*scale));
      mousePos.x += camRect.x;
      mousePos.y += camRect.y;
  }
  sprite.UpdateMatrix(camRect);
  
  prevClicked = clicked;
  clicked = input.Buttons[GLFW_MOUSE_BUTTON_LEFT];
  if(gh::contains(mousePos, sprite.rect))
  {
    onSprite = true;
    sprite.spriteColour = SELECT_COLOUR;
  }
  else
  {
    onSprite = false;
    sprite.spriteColour = glm::vec4(1.0f);
  }
}

  virtual void Draw(Render *render) { sprite.Draw(render); }
  bool Clicked()  { return !prevClicked && clicked && onSprite; }
  virtual void setInitialRect(glm::vec4 rect) 
  {
    initialRect = rect;
  }

protected:
  Sprite sprite;
  bool prevClicked = true;
  bool clicked = false;
  bool onSprite = false;
  bool isStatic;
  glm::vec4 initialRect;
};

#endif
