#ifndef RENDERER_CONFIG_H
#define RENDERER_CONFIG_H


//#define NDEBUG
#define NO_ASSIMP
#define ONLY_2D

#ifndef NDEBUG
   #define TIME_TEXTURE_LOAD
#endif

namespace settings
{
const bool SRGB = false;
const bool MIP_MAPPING = false;
const bool PIXELATED = true;
const bool VSYNC = true;
const bool MULTISAMPLING = true;
const bool SAMPLE_SHADING = true;

const bool FIXED_RATIO = true;
const bool USE_TARGET_RESOLUTION = true;
const int TARGET_WIDTH = 1920;
const int TARGET_HEIGHT = 1080;

const int INITIAL_WINDOW_WIDTH = 800;
const int INITIAL_WINDOW_HEIGHT = 450;

#ifndef NDEBUG
const bool ERROR_ONLY = false;
#endif
}


#endif
