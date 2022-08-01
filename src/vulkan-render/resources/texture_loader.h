#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <cstring>
#include <iostream>

#include "stb_image.h"
#include <config.h>
#include "../render_structs.h"
#include "../vkhelper.h"
#include "resources.h"

namespace Resource
{

const uint32_t MAX_TEXTURES_SUPPORTED = 20;//match in shader

class TextureLoader
{
public:
	TextureLoader(Base base, VkCommandPool pool);
	~TextureLoader();
	void UnloadTextures();
	Texture LoadTexture(std::string path);
	Texture LoadTexture(unsigned char* data, int width, int height, int nrChannels);
	void endLoading();
	VkSampler* getSamplerP() { return &textureSampler; }
	VkImageView* getImageViewsP() { return &imageViews[0]; }

private:
	struct TempTexture
	{
		std::string path;
		unsigned char* pixelData;
		int width;
		int height;
		int nrChannels;
		VkFormat format;
		VkDeviceSize fileSize;
	};

	struct LoadedTexture
	{
		LoadedTexture(){}
		LoadedTexture(TempTexture tex)
		{
			width = tex.width;
			height = tex.height;
			mipLevels = (int)std::floor(std::log2(width > height ? width : height)) + 1;
		}
		uint32_t width;
		uint32_t height;
		VkImage image;
		VkImageView view;
		uint32_t mipLevels;
		VkDeviceSize imageMemSize;
	};

	VkImageView _getImageView(uint32_t texID);

	Base base;
	VkCommandPool pool;

	std::vector<TempTexture> texToLoad;
	std::vector<LoadedTexture> textures;
	VkDeviceMemory memory;
	VkImageView imageViews[MAX_TEXTURES_SUPPORTED];
	VkSampler textureSampler;
};

}
#endif
