cmake_minimum_required(VERSION 3.10)


#customize
if(GFX_ENV_VULKAN)
  set(ProjectName "Robyn-Hood")
  add_definitions(-DGFX_ENV_VULKAN)
elseif(GFX_ENV_OPENGL)
  set(ProjectName "Robyn-Hood-ogl-ver")
  add_definitions(-DGFX_ENV_OPENGL)
endif()
#windows only
set(Lib "D:/noam4/Libraries/VS/lib/x64/")
set(Include "D:/noam4/Libraries/VS/include")

project(${ProjectName} VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)

#packages
if (GFX_ENV_VULKAN)
  find_package(Vulkan REQUIRED)
endif()

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    if (GFX_ENV_VULKAN)
      add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)
    endif()
    find_package(glfw3 3.3 REQUIRED)
    find_package(Freetype REQUIRED)
endif()

#add executables
file(GLOB SOURCES src/*
	  	  src/game/*
		  src/game/map/*
		  src/game/controls/*
		  src/game/controls/card/*
		  )
if(GFX_ENV_VULKAN)
  file(GLOB GRAPHICS-RENDER src/vulkan-render/*
                            src/vulkan-render/resources/*
                            src/vulkan-render/resources/model/*
                            src/vulkan-render/parts/*
			    )
                    
elseif(GFX_ENV_OPENGL)
  file(GLOB GRAPHICS-RENDER src/opengl-render/*
  	    		    src/opengl-render/resources/*
			    )
endif()

list(FILTER GRAPHICS-RENDER EXCLUDE REGEX ".*#+")
list(FILTER SOURCES EXCLUDE REGEX ".*#+")
list(FILTER PROJ_INCLUDE EXCLUDE REGEX ".*#+")
add_executable(${ProjectName}   ${SOURCES}
                                ${GRAPHICS-RENDER}
                                ${PROJ_INCLUDE}
				"resources/resource/resource.o")
				
#link
target_include_directories(${ProjectName} PUBLIC include/)
if(UNIX)
    target_include_directories(${ProjectName} PUBLIC ${FREETYPE_INCLUDE_DIRS})
    if(GFX_ENV_VULKAN)
    target_link_libraries(${ProjectName} glfw
                                         Vulkan::Vulkan
                                         ${FREETYPE_LIBRARIES}
                                         sndfile
                                         portaudio)
    elseif(GFX_ENV_OPENGL)
      target_link_libraries(${ProjectName} glfw
                                     ${FREETYPE_LIBRARIES}
                                     sndfile
                                     portaudio
                                     ${CMAKE_DL_LIBS})
    endif()
else()
    target_include_directories(${ProjectName} PUBLIC ${Include})
    if(GFX_ENV_VULKAN)
    target_link_libraries(${ProjectName} ${Lib}glfw3.lib
                                         ${Lib}freetype.lib
                                         ${Lib}vulkan-1.lib
                                         ${Lib}portaudio_x64.lib
                                         ${Lib}sndfile.lib
                                        bufferoverflowU.lib)
    elseif(GFX_ENV_OPENGL)
      target_link_libraries(${ProjectName} ${Lib}glfw3.lib
                                         ${Lib}freetype.lib
                                         ${Lib}portaudio_x64.lib
                                         ${Lib}sndfile.lib)
    endif()
endif()
