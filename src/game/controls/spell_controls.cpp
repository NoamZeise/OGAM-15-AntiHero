#include "spell_controls.h"
#include "config.h"

SpellControls::SpellControls(Render *render)
{
    insertSpellCard(Sprite(render->LoadTexture("textures/UI/spells/Stone.png")), Spells::Stone);
    insertSpellCard(Sprite(render->LoadTexture("textures/UI/spells/Wait.png")), Spells::Wait);
    insertSpellCard(Sprite(render->LoadTexture("textures/UI/spells/Wind.png")), Spells::Wind);
    insertSpellCard(Sprite(Resource::Texture()), Spells::None);
}

void SpellControls::insertSpellCard(Sprite sprite, Spells spell)
{
 spellTemplates.
	insert(
	 std::pair<Spells, SpellCard>(
				      spell,
	   SpellCard(sprite,
		     spell
      	   )
	 )
    );
}

void SpellControls::Update(glm::vec4 camRect, Timer &timer, Input &input, glm::vec2 mousePos)
{
    targeting = false;
    spell = std::pair<Spells, glm::vec2>(Spells::None, glm::vec2(0));
    for(int i = 0; i < cards.size(); i++)
    {
	cards[i].Update(camRect, timer, input, mousePos);
	if(cards[i].isSelected())
	    targeting = true;
	if(cards[i].wasCast())
	{
	    spell = std::pair<Spells, glm::vec2>(cards[i].getSpell(), cards[i].getTarget());
	    if(cards[i].getSpell() != Spells::Wait)
	    {
		cards.erase(cards.begin() + i--);
		recentreCards(true);
	    }
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
    for(int i = 0; i < spells.size(); i++)
	    cards.push_back(spellTemplates[spells[i]]);
    recentreCards(false);
}

std::vector<Spells> SpellControls::getSpells()
{
    std::vector<Spells> spells(cards.size());
    for(int i = 0 ; i < cards.size(); i++)
	spells[i] = cards[i].getSpell();

    return spells;
}


void SpellControls::recentreCards(bool smooth)
{
    const glm::vec2 CARD_SIZE = glm::vec2(160, 240);
    const glm::vec2 START_CARDS = glm::vec2(0, settings::TARGET_HEIGHT - CARD_SIZE.y*0.85f);
    const float CARD_GAP = 1.05f;

    float width = CARD_SIZE.x* CARD_GAP * cards.size();
    float xOff = (settings::TARGET_WIDTH - width) / 2.0f;
    
    for(int i = 0; i < cards.size(); i++)
    {
	auto initialRect = glm::vec4(xOff + CARD_SIZE.x*CARD_GAP*i, START_CARDS.y, CARD_SIZE.x, CARD_SIZE.y);
	if(smooth)
	    cards[i].setTarget(glm::vec2(initialRect.x, initialRect.y));
	else
	    cards[i].setInitialRect(initialRect);
    }

}
