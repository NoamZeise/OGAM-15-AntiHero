#ifndef SPRITE_ANIMATED_H
#define SPRITE_ANIMATED_H

#include "../renderer_include.h"
#include <timer.h>
#include <iostream>
#include "sprite.h"

struct Frame
{
  Frame() {}
  Frame(Resource::Texture tex, glm::vec4 texOffset, glm::vec2 size)
  {
    this->tex = tex;
    this->texOffset = texOffset;
    this->size = size;
    this->delay = 100.0f;
  }
  Resource::Texture tex;
  glm::vec4 texOffset;
  glm::vec2 size;
  float delay;
};

class AnimatedSprite
{
 public:
  AnimatedSprite() {}
  AnimatedSprite(std::vector<Frame> frames);
  Frame Play(Timer &timer);
  Frame PlayOnce(Timer &timer);
  Frame PlayThenSkipToFrame(Timer &timer, int frameSkip);
  Frame PlayOnceThenSkipToFrame(Timer &timer, int frameSkip);
  Frame PlayToFrame(Timer &timer, int frameEnd);
  Frame PlayOnceToFrame(Timer &timer, int frameEnd);
  Frame GetCurrentFrame() { return frames[current]; }
  Frame PingPong(Timer &timer);
  Frame PingPongOnce(Timer &timer);
  void Reset();
  void InvertFrameX();
  void InvertFrameY();
  void setFrameDelay(float delay);
  void RemoveFrames(int count);
  void SetFrame(int index);
  Frame GetFrame(int index);
 private:
  std::vector<Frame> frames;
  float frameTimer = 0.0f;
  int frameChange = 1;
  int current = 0;
  bool done;
  bool pingPonged = false;
};


std::vector<AnimatedSprite> CreateAnimationSetFromTexture(Resource::Texture texture, glm::vec2 dimenions, glm::vec2 offset);


#endif
