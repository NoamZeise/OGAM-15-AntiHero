# Work in progress submission to One Game A month Jam #15 for August 2022


# building from source

## windows

If using vulkan build:
* download the [vulkan sdk](https://vulkan.lunarg.com/), run installer and put the necessary files in your lib and include directories
If using OpenGL build:
* put glad opengl include into your includes: https://glad.dav1d.de/ pick an opengl version at or above 3.3

* download [glfw3](https://www.glfw.org/), compile and put in your lib and include directories

* download [glm](https://github.com/g-truc/glm), it is header only so put the headers in your include directory

* download [freetype](https://freetype.org/download.html) compile and put in your lib and include directories

* download [assimp](https://github.com/assimp/assimp/blob/master/Build.md) compile and put in your lib and include directories, and distribute the dll with your binaries

* download [libsndfile](http://www.mega-nerd.com/libsndfile/#Download) compile and put in your lib and include directories, and put the dll with your binaries

* download [portaudio](http://files.portaudio.com/docs/v19-doxydocs/compile_windows.html) compile and put in your lib and include directories, and put the dll with your binaries


* set your lib and include paths at the start of the cmake file
```
#windows only
set(Lib "Path/to/lib")
set(Include "Path/to/include")
```

* If you have cmake and ninja installed, and a C/C++ compiler you can use the included windows build scripts under "resources/buildscripts/windows/" to build the debug or release versions of the project and automatically launch it. You must include the assimp .dll with the project.

to build from project root:
```
mkdir build
"./resource/buildscripts/windows/debugVulkan.bat"
```

## linux with apt

If using Vulkan:

vulkan tools
```
$ sudo apt-get install vulkan-tools
```
vulkan loader / validation layers / spriv compilers
```
$ sudo apt-get install libvulkan-dev vulkan-validationlayers-dev spirv-tools
```
test vulkan works
```
$ vkcube
```
If using OpenGL: put glad opengl include into your includes: https://glad.dav1d.de/ pick an opengl version at or above 3.3

additional libraries
```
$ sudo apt-get install libglfw3-dev libglm-dev libfreetype-dev libassimp-dev libsndfile1-dev libasound-dev portaudio19-dev
```

Make sure you have cmake and ninja installed otherwise do
```
sudo apt install cmake ninja-build
```

then to build, go to the project root and do
```
mkdir build
chmod +x resources/buildscripts/linux/debugVulkan.sh
./resources/buildscrips/linux/debugVulkan.sh
```
in the future just type the last line to build the project
