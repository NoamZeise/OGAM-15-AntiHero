#include "spell_controls.h"

SpellControls::SpellControls(Render *render)
{
    spellTemplates.
	insert(
	 std::pair<Spells, SpellCard>(
	   Spells::Stone,
	   SpellCard(
	       Sprite(
		     glm::vec2(100, 100),
		     render->LoadTexture("textures/UI/spells/Stone.png")
	       ),
	       Spells::Stone
      	   )
	 )
    );
}

void SpellControls::Update(glm::vec4 camRect, Timer &timer, Input &input, glm::vec2 mousePos)
{
    spell = std::pair<Spells, glm::vec2>(Spells::None, glm::vec2(0));;
    for(int i = 0; i < cards.size(); i++)
    {
	cards[i].Update(camRect, timer, input, mousePos);
	if(cards[i].wasCast())
	{
	    spell = std::pair<Spells, glm::vec2>(cards[i].getSpell(), cards[i].getTarget());
	    cards.erase(cards.begin() + i--);
	}
    }
}

void SpellControls::Draw(Render *render)
{
    for(auto& c: cards)
	c.Draw(render);
}

void SpellControls::setCards(std::vector<Spells> spells)
{
    cards.clear();
    const glm::vec2 START_CARDS = glm::vec2(50, 280);
    const glm::vec2 CARD_SIZE = glm::vec2(80, 120);
    
    for(int i = 0; i < spells.size(); i++)
    {
	    cards.push_back(spellTemplates[spells[i]]);
    cards.back().setInitialRect(glm::vec4(START_CARDS.x, START_CARDS.y, CARD_SIZE.x, CARD_SIZE.y));
    cards.push_back(spellTemplates[Spells::Stone]);
    cards.back().setInitialRect(glm::vec4(START_CARDS.x + i*CARD_SIZE.x*1.02f, START_CARDS.y, CARD_SIZE.x, CARD_SIZE.y));

    }
}

std::vector<Spells> SpellControls::getSpells()
{
    std::vector<Spells> spells(cards.size());

    for(auto& c : cards)
	spells.push_back(c.getSpell());

    return spells;
}
