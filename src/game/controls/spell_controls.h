#ifndef GAME_SPELL_CONTROLS_H
#define GAME_SPELL_CONTROLS_H


#include "../../renderer_include.h"
#include "card/spell.h"
#include <timer.h>
#include <input.h>

#include <vector>
#include <map>

class SpellControls
{
 public:
  SpellControls() {}
  SpellControls(Render *render);
    void Update(glm::vec4 camRect, Timer &timer, Input &input, glm::vec2 mousePos);
  void Draw(Render *render);
    void setCards(std::vector<Spells> spells);
  std::vector<Spells> getSpells();
  std::pair<Spells, glm::vec2> spellCast() { return spell; }
    bool isTargeting() { return targeting; }
 private:
  std::vector<SpellCard> cards;
  std::map<Spells, SpellCard> spellTemplates;
  std::pair<Spells, glm::vec2> spell = std::pair<Spells, glm::vec2>(Spells::None, glm::vec2(0));
  void recentreCards(bool smooth);
    void insertSpellCard(Sprite sprite, Spells spell);
    bool targeting = false;
};

#endif
