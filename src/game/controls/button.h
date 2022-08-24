#ifndef BUTTON_H
#define BUTTON_H

#include "../sprite.h"
#include "GLFW/glfw3.h"

#include <input.h>

const glm::vec4 SELECT_COLOUR = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);

class Button
{
public:
  Button() {}
  Button(Sprite sprite, bool isStatic) { this->sprite = sprite; this->initialRect = sprite.rect; this->isStatic = isStatic; }
  virtual void Update(glm::vec4 camRect, Input &input, glm::vec2 mousePos)
{
  prevClicked = clicked;
  clicked = input.Buttons[GLFW_MOUSE_BUTTON_LEFT];
  if(gh::contains(mousePos, initialRect))
  {
    onSprite = true;
    sprite.spriteColour = SELECT_COLOUR;
  }
  else
  {
    onSprite = false;
    sprite.spriteColour = glm::vec4(1.0f);
  }

  if(isStatic)
    sprite.rect = glm::vec4(initialRect.x + camRect.x, initialRect.y + camRect.y, initialRect.z, initialRect.w);

  sprite.UpdateMatrix(camRect);
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
