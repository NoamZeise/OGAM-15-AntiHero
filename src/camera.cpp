#include "camera.h"

namespace Camera
{

  float RoomFollow2D::getOffset(float target, float focus, float roomPos, float roomSize)
	{
		if(target > roomSize)
				return -roomSize/2 -roomPos;
		float min = roomPos + target/2;
		float max = roomPos + roomSize - target/2;
		if(focus > min && focus < max)
			return -focus;
		if(focus < min)
			return -min;
		if(focus > max)
			return -max;
		return -focus;
	}


	void RoomFollow2D::Target(glm::vec2 focus, Timer &timer)
		{
		  
		  int correctedWidth = settings::TARGET_WIDTH*scale;
			int correctedHeight = settings::TARGET_HEIGHT*scale;
			glm::vec2 transform = glm::vec2(0);

			if(mapRect.z == 0 && cameraRects.size() == 0)
			{
				transform.x = -focus.x;
				transform.y = -focus.y;
			}
			else if(cameraRects.size() == 0)
			{
				transform.x = getOffset(correctedWidth, focus.x, mapRect.x, mapRect.z);
				transform.y = getOffset(correctedHeight, focus.y, mapRect.y, mapRect.w);
			}
			else
			{
				glm::vec4 newRect = glm::vec4(0);
				currentRoom = glm::vec4(0);
				for(const auto& rect: cameraRects)
				{
					if(gh::contains(focus, rect))
					{
						newRect = rect;
						currentRoom = rect;
						break;
					}
				}
				if(newRect == glm::vec4(0))
				{
					transform.x = -focus.x;
					transform.y = -focus.y;
				}
				else
				{
					if (newRect != currentRect)
						currentRect = newRect;
					transform.x = getOffset(correctedWidth, focus.x, currentRect.x, currentRect.z);
					transform.y = getOffset(correctedHeight, focus.y, currentRect.y, currentRect.w);
				}

				if(mapRect != glm::vec4(0))
				{
					transform.x = getOffset(correctedWidth, -transform.x, mapRect.x, mapRect.z);
					transform.y = getOffset(correctedHeight, -transform.y, mapRect.y, mapRect.w);
				}
			}

			if(previousOff.x != 0 || previousOff.y != 0)
			{
			previousOff += glm::vec2(((transform.x - previousOff.x)/CAM2D_FLOAT) * timer.FrameElapsed(),
				((transform.y - previousOff.y)/CAM2D_FLOAT)* timer.FrameElapsed());
			transform = previousOff;
			}

			cameraArea = glm::vec4(-transform.x - (float)correctedWidth/2.0f, -transform.y - (float)correctedHeight/2.0f,
									(correctedWidth), (correctedHeight));
			offset = glm::translate(glm::mat4(1.0f), glm::vec3((int)(transform.x + correctedWidth/2.0f),
				 					(int)(transform.y + correctedHeight/2.0f), 0));
			previousOff = transform;
		}
  
} //namespace end
