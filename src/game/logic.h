#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <timer.h>
#include <input.h>
#include <gamehelper.h>
#include <audio.h>

#include "player.h"
#include "../renderer_include.h"
#include "../camera.h"
#include "map/map.h"
#include "characters/hero.h"
#include "characters/enemy.h"
#include "characters/obstacle.h"

#include "god/stone.h"

#include "controls/spell_controls.h"

#include <vector>
#include <iostream>
#include <string>

const float GAME_MUSIC_VOLUME = 0.35f;
const float GAME_SFX_VOLUME = 0.85f;
const float GAME_ROCK_BOX_VOLUME = 0.55f;
const glm::vec2 TREE_DIM = glm::vec2(1359 * 0.25f, 1755 *0.25f);

namespace game_music {
    const std::string Voiceless = "audio/Robin Hood Medieval Music2.ogg";
    const std::string Hip = "audio/Robin Hood Song Level3.wav";
    const std::string Voiced = "audio/Robin Hood SongV3.wav";
    const std::string Menu = "audio/Robin Hood Menu Music.wav";
    
    const std::string Victory = "audio/Robin Hood Win Song.wav";
    const std::string EndOfLevel = "audio/Robin Hood End of Level.wav";
    const std::string Lose = "audio/Robin Hood Lose Music.wav";
}

class GameLogic
{
 public:
    GameLogic() {}
    GameLogic(Render *render, Camera::RoomFollow2D *cam2D, Audio::Manager* audioManager);
    void Update(glm::vec4 camRect, Timer &timer, Input &input, Camera::RoomFollow2D *cam2D, glm::vec2 mousePos);
    void Draw(Render *render);
    glm::vec2 getTarget();
    bool gameComplete() { return currentLevelIndex >= levels.size(); }
    void setCurrentAudioVolume(float vol)
    {
	audio->SetVolume(currentAudio, vol);
    }
    void toggleActiveAudio(bool pause)
    {
	if(pause)
	    audio->Pause(currentAudio);
	else
	    audio->Resume(currentAudio);
    }
    void setCursorActive(bool active)
    {
	cursorActive = active;
    }
    void skipLevel() {
	hero.simulateEnd();
    }

 private:
    void LoadMap(Camera::RoomFollow2D *cam2D);
    void playerDeath(Camera::RoomFollow2D *cam2D);
    void levelComplete(Camera::RoomFollow2D *cam2D);
    void spellCast(Spells spell, glm::vec2 pos, Camera::RoomFollow2D* cam2D);
    void spellUpdate(glm::vec4 camRect, Timer &timer);

    
  std::vector<Level> levels;
  Button restartBtn;
#ifdef NDEBUG
  int currentLevelIndex = 0;
#else
  int currentLevelIndex = 0;
#endif
  Level currentLevel;
  Player player;
  Hero hero;
    std::vector<glm::vec4> staticColliders;
  Enemy enemy;
  std::vector<Enemy> enemies;
    Obstacle obstacle;
    std::vector<Obstacle> obstacles;
  god::Stone stone;
  std::vector<god::Stone> stones;
    god::Smoke smoke;
    std::vector<god::Smoke> smokes;
    god::Gust gust;
    std::vector<god::Gust> gusts;
    Sprite pickupSprite;
    std::vector<std::pair<Pickup, Sprite>> pickups;
    Sprite gold;
    Sprite tree;
    std::vector<Sprite> trees;

    Sprite checkpoint;
    std::vector<Sprite> checkpoints;
    Sprite* lastCheckpoint;
    Resource::Texture checkpointActive;
    int checkpointTargetIndex;
    glm::vec2 checkpointPos;
    std::vector<Spells> checkpointSpells;
    std::vector<Obstacle> checkpointObstacles;
    std::vector<Enemy> checkpointEnemies;
    std::vector<std::pair<Pickup, Sprite>> checkpointPickups;
    bool checkpointGotGold = false;
    
    Input prevInput;

    SpellControls spellControls;
    Audio::Manager* audio;

    float lastScale = 1.0f;

    Sprite* currentCursor;
    Sprite defaultCursor;
    Sprite targetCursor;

    bool gotGold = false;
    gh::Random rand;

    float removeAudioTimer = 0.0f;
    float removeAudioDelay = 1000.0f;

    std::string currentAudio;
    bool cursorActive = true;
};

#endif
