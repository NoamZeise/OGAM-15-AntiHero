#include "animation.h"

std::vector<AnimatedSprite> CreateAnimationSetFromTexture(Resource::Texture texture, glm::vec2 dimenions, glm::vec2 offset)
{
    std::vector<AnimatedSprite> animations;
    for(float y = offset.y; y + dimenions.y <= texture.dim.y; y += dimenions.y)
    {
	std::vector<Frame> frames;
	for(float x = offset.x; x + dimenions.x <= texture.dim.x; x += dimenions.x)
	{
	    frames.push_back(Frame(
			texture,
			glmhelper::getTextureOffset(
			   texture.dim,
			   glm::vec4(x, y, dimenions.x, dimenions.y
				     )),
			dimenions));
	}
	animations.push_back(AnimatedSprite(frames));
    }
    return animations;
}

AnimatedSprite::AnimatedSprite(std::vector<Frame> frames)
{
    this->frames = frames;
}

void AnimatedSprite::InvertFrameX()
{
    for(auto& f: frames)
	f.texOffset.z *= -1;
}

void AnimatedSprite::InvertFrameY()
{
    for(auto& f: frames)
	f.texOffset.w *= -1;
}

void AnimatedSprite::setFrameDelay(float delay)
{
    for(auto& f: frames)
	f.delay = delay;
}

void AnimatedSprite::RemoveFrames(int count)
{
    if(frames.size() > count)
	frames.resize(frames.size() - count);
}

void AnimatedSprite::SetFrame(int index)
{
    if(frames.size() > index)
    {
	current = index;
    }
    else 
	std::cout << "supplied index out of range in animated sprite SetFrame\n";
}

Frame AnimatedSprite::GetFrame(int index)
{
    if(frames.size() > index)
    {
	return frames[index];
    }
    std::cout << "supplied index out of range in animated sprite GetFrame\n";
    return Frame();
}


Frame AnimatedSprite::Play(Timer &timer)
	{
		frameTimer += timer.FrameElapsed();
		if(frameTimer > frames[current].delay)
		{
			frameTimer = 0;
			current++;
			if(current >= frames.size())
				current = 0;
		}
		return frames[current];
	}

Frame AnimatedSprite::PlayThenSkipToFrame(Timer &timer, int frameSkip)
	{
		frameTimer += timer.FrameElapsed();
		if(frameTimer > frames[current].delay)
		{
			frameTimer = 0;
			current++;
			if(current >= frames.size())
				current = frameSkip;
		}
		return frames[current];
	}

Frame AnimatedSprite::PlayOnceThenSkipToFrame(Timer &timer, int frameSkip)
	{
		frameTimer += timer.FrameElapsed();
		if(current < frameSkip)
			current = frameSkip;
		if(frameTimer > frames[current].delay)
		{
			frameTimer = 0;
			current++;
			if(current >= frames.size())
				current = frames.size() - 1;
		}
		return frames[current];
	}

Frame AnimatedSprite::PlayOnceToFrame(Timer &timer, int frameEnd)
	{
		frameTimer += timer.FrameElapsed();
		if(current > frameEnd)
			current = frameEnd;
		if(frameTimer > frames[current].delay)
		{
			frameTimer = 0;
			current++;
			if(current > frameEnd)
				current = frameEnd;
		}
		return frames[current];
	}

Frame AnimatedSprite::PlayToFrame(Timer &timer, int frameEnd)
	{
		frameTimer += timer.FrameElapsed();
		if(current > frameEnd)
			current = 0;
		if(frameTimer > frames[current].delay)
		{
			frameTimer = 0;
			current++;
			if(current > frameEnd)
				current = 0;
		}
		return frames[current];
	}

Frame AnimatedSprite::PlayOnce(Timer &timer)
	{
		if(!done)
		{
			frameTimer += timer.FrameElapsed();
			if(frameTimer > frames[current].delay)
			{
				frameTimer = 0;
				current++;
				if(current >= frames.size())
				{
					done = true;
					current--;
				}
			}
		}
		return frames[current];
	}

Frame AnimatedSprite::PingPong(Timer &timer)
	{
		if(frames.size() == 1)
			return frames[0];
		frameTimer += timer.FrameElapsed();
		if(frameTimer > frames[current].delay)
		{
			frameTimer = 0;
			if(pingPonged)
			{
				current--;
				if(current == 0)
				{
					pingPonged = false;
				}	
			}
			else
			{
				current++;
				if(current >= frames.size())
				{
					pingPonged = true;
					current--;
				}
			}
		}
		return frames[current];
	}

Frame AnimatedSprite::PingPongOnce(Timer &timer)
	{
		if(frames.size() == 1)
			return frames[0];
		if(!done)
		{
			frameTimer += timer.FrameElapsed();
			if(frameTimer > frames[current].delay)
			{
				frameTimer = 0;
				if(pingPonged)
				{
					current--;
					if(current == 0)
					{
						done = true;
					}	
				}
				else
				{
					current++;
					if(current >= frames.size())
					{
						pingPonged = true;
						current--;
					}
				}
			}
		}
		return frames[current];
	}


void AnimatedSprite::Reset()
	{
		done = false;
		pingPonged = false;
		current = 0;
	}
