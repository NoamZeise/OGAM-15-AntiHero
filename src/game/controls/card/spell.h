#ifndef GAME_SPELL_CARD
#define GAME_SPELL_CARD

#include "../button.h"
#include "glm/geometric.hpp"
#include <timer.h>
#include <input.h>

enum class Spells
{
  None,
  Stone,
  Wait,
  Wind,
};

namespace{
const float RAISE_CARD_DIST = 30.0f;
const float CARD_FLOATINESS = 100.0f;
const float CARD_TRANSPARENCY_RANGE = 200.0f;
} // namespace

class SpellCard : public Button
{
 public:
  SpellCard() {}
 SpellCard(Sprite card, Spells spell) : Button(card, true)
  {
    this->spell = spell;
    this->sprite.depth = 2.0f;
  }

  void Update(glm::vec4 camRect, Timer &timer, Input &input, glm::vec2 mousePos)
  {
    Button::Update(camRect, input, mousePos);
    glm::vec2 toTarget = target - glm::vec2(initialRect.x, initialRect.y);
    toTarget /= CARD_FLOATINESS;
    toTarget *= timer.FrameElapsed();
    initialRect.x += toTarget.x;
    initialRect.y += toTarget.y;
    if(selected)
      {
	if(!clicked)
	  {
	  selected = false;
	  if(prevDist < 0.8f)
	    {
	      cast = true;
	    }
	  else {
	   
	  target.x = originTarget.x;
	  target.y = originTarget.y;
	  }
	  }
	else
	  {
	    sprite.spriteColour = SELECT_COLOUR;
	    float dist = glm::distance(originTarget, glm::vec2(initialRect.x, initialRect.y));
	    sprite.spriteColour.w = 1 - (dist / CARD_TRANSPARENCY_RANGE);
	    prevDist = sprite.spriteColour.w;

	    spellTarget = glm::vec2(mousePos.x + camRect.x, mousePos.y + camRect.y);
	    target.x = mousePos.x - initialRect.z/2.0f;
	    target.y = mousePos.y - initialRect.w/2.0f;
	  }
      }
    else
      {
	if(Clicked())
	  {
	  selected = true;
	  originTarget = target;
	  }
	else
	  {
	    if(onSprite && !wasOnSprite)
	      {
		wasOnSprite = true;
		target.y -= RAISE_CARD_DIST;
	      }
	    else if(!onSprite && wasOnSprite)
	      {
		wasOnSprite = false;
		target.y += RAISE_CARD_DIST;

	      }
	}
      }
  }

  bool wasCast()
  {
    return cast;
  }
  
  glm::vec2 getTarget()
    {
      return spellTarget;
    }

  Spells getSpell() { return spell; }

  void setInitialRect(glm::vec4 rect) override
  {
    initialRect = rect;
    target = glm::vec2(initialRect.x, initialRect.y);
  }

 private:
  bool selected = false;
  bool cast = false;
  bool wasOnSprite = false;
  glm::vec2 originTarget;
  Spells spell;
  float prevDist = 0.0f;
  glm::vec2 target;
  glm::vec2 spellTarget;
};

#endif
