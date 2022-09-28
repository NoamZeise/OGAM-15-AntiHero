#include "app.h"
#include "GLFW/glfw3.h"
#include "config.h"
#include "glmhelper.h"
#include <cmath>

App::App()
{
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
    throw std::runtime_error("failed to initialise glfw!");

  const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  mWindowWidth = (int)((float)videoMode->width);// / 1.2);
  mWindowHeight = (int)((float)videoMode->height);// / 1.2);
  
  Render::SetGLFWWindowHints();

  #ifdef GFX_ENV_VULKAN
  mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "Vulkan App", nullptr, nullptr);
  #endif
  #ifdef GFX_ENV_OPENGL
  mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "OpenGL App", nullptr, nullptr);
  #endif
  if (!mWindow)
  {
    glfwTerminate();
    throw std::runtime_error("failed to create glfw window!");
  }
  GLFWimage winIcon[1];
  winIcon[0].pixels = stbi_load("textures/icon.png", &winIcon[0].width, &winIcon[0].height, 0, 4); //rgba channels
  glfwSetWindowIcon(mWindow, 1, winIcon);
  glfwSetWindowUserPointer(mWindow, this);
  glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
  glfwSetCursorPosCallback(mWindow, mouse_callback);
  glfwSetScrollCallback(mWindow, scroll_callback);
  glfwSetKeyCallback(mWindow, key_callback);
  glfwSetMouseButtonCallback(mWindow, mouse_button_callback);
  glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetInputMode(mWindow, GLFW_RAW_MOUSE_MOTION, glfwRawMouseMotionSupported());
  glfwSetWindowPos(mWindow, 0.0f, 0.0f);
  int width = mWindowWidth;
  int height = mWindowHeight;
  if (settings::USE_TARGET_RESOLUTION)
  {
      width = settings::TARGET_WIDTH;
      height = settings::TARGET_HEIGHT;
  }

  mRender = new Render(mWindow, glm::vec2(width, height));
  
  if (settings::FIXED_RATIO)
      glfwSetWindowAspectRatio(mWindow, width, height);

  loadAssets();

  finishedDrawSubmit = true;
}

App::~App() {
  if (submitDraw.joinable())
    submitDraw.join();
  delete mRender;
  glfwTerminate();
}

void App::loadAssets() {
    gameLogic = GameLogic(mRender, &cam2d, &audioManager);
    endScreenFont = mRender->LoadFont("textures/MedievalSharp-Regular.ttf");
    endScreen = mRender->LoadTexture("textures/end-screen.png");
    cursor = Sprite(mRender->LoadTexture("textures/UI/cursor/default.png"));
    cursor.depth = 3.0f;
    cursor.rect.z *= 0.3f;
    cursor.rect.w *= 0.3f;
    pixel = mRender->LoadTexture("textures/pixel.png");
    audioManager.Play(game_music::Menu, true, GAME_MUSIC_VOLUME);
    audioManager.Pause(game_music::Menu);
    
    mRender->EndResourceLoad();
}

void App::run() {
  while (!glfwWindowShouldClose(mWindow)) {
    update();
    if (mWindowWidth != 0 && mWindowHeight != 0)
      draw();
  }
}

void App::resize(int windowWidth, int windowHeight) {
  if (submitDraw.joinable())
    submitDraw.join();
  this->mWindowWidth = windowWidth;
  this->mWindowHeight = windowHeight;
  if (mRender != nullptr && mWindowWidth != 0 && mWindowHeight != 0)
    mRender->FramebufferResize();
}

void App::update() {
#ifdef TIME_APP_DRAW_UPDATE
  auto start = std::chrono::high_resolution_clock::now();
#endif
  glfwPollEvents();

  if(timer.FrameElapsed() > 500.0f)
      timer.Update();

  timeSinceStart += timer.FrameElapsed();

  if (input.Keys[GLFW_KEY_F] && !previousInput.Keys[GLFW_KEY_F]) {
    if (glfwGetWindowMonitor(mWindow) == nullptr) {
      const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
      glfwSetWindowMonitor(mWindow, glfwGetPrimaryMonitor(), 0, 0, mode->width,
                           mode->height, mode->refreshRate);
    } else {
      const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
      glfwSetWindowMonitor(mWindow, NULL, 0, 0, mWindowWidth, mWindowHeight,
                           mode->refreshRate);
    }
  }

  if(input.Keys[GLFW_KEY_EQUAL])
    camScale -=  0.001f * timer.FrameElapsed();
  if(input.Keys[GLFW_KEY_MINUS])
    camScale +=  0.001f * timer.FrameElapsed();

  if(gameLogic.gameComplete())
  {
      cam2d.SetCameraOrigin();
      cam2d.setScale(1.0f);
      if(!playingEndMusic)
      {
	  timeSinceStart = 0.0f;
	  playingEndMusic = true;
	  audioManager.StopAll();
	  audioManager.Play(game_music::Victory, false, GAME_MUSIC_VOLUME);
      }
      if(input.Keys[GLFW_KEY_ESCAPE])
	  glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
  }
  else
  {
      if (input.Keys[GLFW_KEY_ESCAPE] && !previousInput.Keys[GLFW_KEY_ESCAPE] && gameLogic.getLevel() != 0) {
	  paused = !paused;
	  if(paused)
	  {
	      gameLogic.toggleActiveAudio(true);
	      audioManager.Resume(game_music::Menu);
	      if(!audioManager.Playing(game_music::Menu))
		  audioManager.Play(game_music::Menu, true, GAME_MUSIC_VOLUME);
	  }
	  else
	  {
	      timeSincePause = 0.0f;
	      audioManager.Pause(game_music::Menu);
	      gameLogic.toggleActiveAudio(false);
	  }
	  gameLogic.setCursorActive(!paused);
	  //reset timer as takes a while to change music, which lags fade transition
	  timer.Update();
	  timer.Update();
      }
      if (input.Keys[GLFW_KEY_BACKSPACE] && !previousInput.Keys[GLFW_KEY_BACKSPACE]) {
	  if(paused)
	  {
	      glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
	  }
      }
      if (input.Keys[GLFW_KEY_F1] && !previousInput.Keys[GLFW_KEY_F1]) {
	  if(paused)
	  {
	      paused = false;
	      timeSincePause = 0.0f;
	      audioManager.Pause(game_music::Menu);
	      gameLogic.toggleActiveAudio(false);
	      gameLogic.setCursorActive(!paused);
	  }
	  gameLogic.skipLevel();
      }
      
      cam2d.setScale(camScale);
      auto mousePos =  correctedMouse();
      auto camRect = cam2d.getCameraArea();
      if(!paused)
      {
	  cam2d.Target(gameLogic.getTarget(), timer);
	  camRect = cam2d.getCameraArea();
	  gameLogic.Update(camRect, timer, input, &cam2d, mousePos);
      }
      else
      {
	  timeSincePause += timer.FrameElapsed();
	  cursor.rect.x = mousePos.x + camRect.x;
	  cursor.rect.y = mousePos.y + camRect.y;
	  cursor.UpdateMatrix(camRect);
      }
  }
  
  postUpdate();
#ifdef TIME_APP_DRAW_UPDATE
  auto stop = std::chrono::high_resolution_clock::now();
  std::cout << "update: "
            << std::chrono::duration_cast<std::chrono::microseconds>(stop -
                                                                     start)
                   .count()
            << " microseconds" << std::endl;
#endif
}

void App::postUpdate() {
  mRender->set2DViewMatrixAndScale(cam2d.getViewMat(), cam2d.getScale());
  previousInput = input;
  input.offset = 0;
  timer.Update();
}

void App::draw() {
#ifdef TIME_APP_DRAW_UPDATE
  auto start = std::chrono::high_resolution_clock::now();
#endif

#ifdef MULTI_UPDATE_ON_SLOW_DRAW
  if (!finishedDrawSubmit)
    return;
  finishedDrawSubmit = false;
#endif

    if(submitDraw.joinable())
      submitDraw.join();

    mRender->Begin2DDraw();

    if(gameLogic.gameComplete())
    {
	drawEndScreen();
    }
    else
    {
	gameLogic.Draw(mRender);
	if(paused)
	{
	    cursor.Draw(mRender);
	    float fade = (timeSincePause / FADE_TIME);
	    auto cam = cam2d.getCameraArea();
	    mRender->DrawString(endScreenFont, "Press Esc to unpause",
				glm::vec2(cam.x + 20.0f, cam.y + 170.0f), 30, 8.1f, glm::vec4(1.0f, 1.0f, 1.0f, fade));
	    mRender->DrawString(endScreenFont, "Press F1 to skip this level",
				glm::vec2(cam.x + 20.0f, cam.y + 270.0f), 30, 8.1f, glm::vec4(1.0f, 1.0f, 1.0f, fade));
	    mRender->DrawString(endScreenFont, "Press Backspace to quit",
				glm::vec2(cam.x + 20.0f, cam.y + 370.0f), 30, 8.1f, glm::vec4(1.0f, 1.0f, 1.0f, fade));
	    mRender->DrawQuad(pixel, glmhelper::calcMatFromRect(cam, 0.0f, 8.0f),
			      glm::vec4(0.2f, 0.1f, 0.0f, fade > FADE_MAX ? FADE_MAX : fade));
	}
	if(gameLogic.getLevel() == 0)
	{
	    auto cam = cam2d.getCameraArea();
	    float fade = (timeSinceStart / FADE_START);
	    mRender->DrawQuad(pixel, glmhelper::calcMatFromRect(cam, 0.0f, 8.0f),
			      glm::vec4(0.0f, 0.0f, 0.0f, 1.2f - fade));
	}
    
    }
    
#ifdef GFX_ENV_VULKAN
  submitDraw =
      std::thread(&Render::EndDraw, mRender, std::ref(finishedDrawSubmit));
#endif

#ifdef GFX_ENV_OPENGL
 mRender->EndDraw(finishedDrawSubmit);
#endif

#ifdef TIME_APP_DRAW_UPDATE
  auto stop = std::chrono::high_resolution_clock::now();
  std::cout << "draw: "
            << std::chrono::duration_cast<std::chrono::microseconds>(stop -
                                                                     start)
                   .count()
            << " microseconds" << std::endl;
#endif
}

glm::vec2 App::correctedPos(glm::vec2 pos)
{
  if (settings::USE_TARGET_RESOLUTION)
    return glm::vec2(
        pos.x * ((float)settings::TARGET_WIDTH*camScale / (float)mWindowWidth),
        pos.y * ((float)settings::TARGET_HEIGHT*camScale / (float)mWindowHeight));

  return glm::vec2(pos.x, pos.y);
}

void App::drawEndScreen()
{
    auto cam = glm::vec4(0, 0, settings::TARGET_WIDTH, settings::TARGET_HEIGHT);
    float fade = (timeSinceStart / FADE_START);

    mRender->DrawQuad(endScreen, glmhelper::calcMatFromRect(cam, 0.0f, 0.0f),
			      glm::vec4(0.7f, 0.6f, 0.6f, 1.0f));
    
    mRender->DrawString(endScreenFont, "Thanks for playing!",
			glm::vec2(250.0f, 100.0f), 70, 1.0f, glm::vec4(1.0f));
    
    mRender->DrawString(endScreenFont, "Mick Cooke(Make Fire Music)",
			glm::vec2(100.0f, 300.0f), 60, 1.0f, glm::vec4(1.0f));
    mRender->DrawString(endScreenFont, "-    Music",
			glm::vec2(1000.0f, 300.0f), 60, 1.0f, glm::vec4(1.0f));
    mRender->DrawString(endScreenFont, "Thanos Gramosis",
			glm::vec2(100.0f, 450.0f), 60, 1.0f, glm::vec4(1.0f));
    mRender->DrawString(endScreenFont, "-    Art",
			glm::vec2(1000.0f, 450.0f), 60, 1.0f, glm::vec4(1.0f));
    // mRender->DrawString(endScreenFont, "Paulina Ramirez(Lady Yami)",
    //			glm::vec2(100.0f, 600.0f), 60, 1.0f, glm::vec4(1.0f));
    // mRender->DrawString(endScreenFont, "-    Voice Acting",
    //			glm::vec2(1000.0f, 600.0f), 60, 1.0f, glm::vec4(1.0f));
    mRender->DrawString(endScreenFont, "Noam Zeise",
			glm::vec2(100.0f, 600.0f), 60, 1.0f, glm::vec4(1.0f));
    mRender->DrawString(endScreenFont, "-    Programming",
			glm::vec2(1000.0f, 600.0f), 60, 1.0f, glm::vec4(1.0f));
    
    mRender->DrawString(endScreenFont, "Press Esc To Exit",
			glm::vec2(1500.0f, 1000.0f), 50, 1.0f, glm::vec4(1.0f));

    mRender->DrawQuad(pixel, glmhelper::calcMatFromRect(cam, 0.0f, 8.0f),
		      glm::vec4(0.0f, 0.0f, 0.0f, 1.2f - fade));
}

glm::vec2 App::appToScreen(glm::vec2 pos)
{
  #ifdef OGL_RENDER_H
  //std::cout << (float)mWindowWidth / (float)mWindowHeight << std::endl;
  //wRatio = wRatio > 1.0f ? 1.0f : wRatio;
  float ratio = (float)mWindowWidth  / (float)mWindowHeight;
  float proper = (float)settings::TARGET_WIDTH / (float)settings::TARGET_HEIGHT;
  float diff = ratio/proper;
	return glm::vec2((pos.x / camScale) * ((float)mWindowWidth / (float)settings::TARGET_WIDTH) / diff,
    mWindowHeight  - ((pos.y / camScale) * ((float)mWindowHeight / (float)settings::TARGET_HEIGHT)));
  #else
	return glm::vec2(pos.x / camScale, pos.y / camScale );
  #endif
}

glm::vec2 App::correctedMouse()
{
  return correctedPos(glm::vec2(input.X, input.Y));
}

/*
 *       GLFW CALLBACKS
 */

void App::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  auto app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
  app->resize(width, height);
}

void App::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  App *app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
  app->input.X = xpos;
  app->input.Y = ypos;
}
void App::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  App *app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
  app->input.offset = yoffset;
}

void App::key_callback(GLFWwindow *window, int key, int scancode, int action,
                       int mode) {
  App *app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS) {
      app->input.Keys[key] = true;
    } else if (action == GLFW_RELEASE) {
      app->input.Keys[key] = false;
    }
  }
}

void App::mouse_button_callback(GLFWwindow *window, int button, int action,
                                int mods) {
  App *app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

  if (button >= 0 && button < 8) {
    if (action == GLFW_PRESS) {
      app->input.Buttons[button] = true;
    } else if (action == GLFW_RELEASE) {
      app->input.Buttons[button] = false;
    }
  }
}

void App::error_callback(int error, const char *description) {
  throw std::runtime_error(description);
}
