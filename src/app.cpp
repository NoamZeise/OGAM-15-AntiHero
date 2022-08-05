#include "app.h"
#include "GLFW/glfw3.h"
#include <cmath>

App::App()
{
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
    throw std::runtime_error("failed to initialise glfw!");

  const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  mWindowWidth = videoMode->width*0.95;
  mWindowHeight = videoMode->height*0.95;
  
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
  glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetInputMode(mWindow, GLFW_RAW_MOUSE_MOTION, glfwRawMouseMotionSupported());

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
  gameLogic = GameLogic(mRender, &cam2d);
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
  if (input.Keys[GLFW_KEY_ESCAPE] && !previousInput.Keys[GLFW_KEY_ESCAPE]) {
    glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
  }

    float camspeed = 1.0f * camScale;
  if(input.Keys[GLFW_KEY_UP])
    camTarget.y -= camspeed * timer.FrameElapsed();
  if(input.Keys[GLFW_KEY_DOWN])
    camTarget.y += camspeed * timer.FrameElapsed();
  if(input.Keys[GLFW_KEY_LEFT])
    camTarget.x -= camspeed * timer.FrameElapsed();
  if(input.Keys[GLFW_KEY_RIGHT])
    camTarget.x += camspeed * timer.FrameElapsed();
  if(input.Keys[GLFW_KEY_EQUAL])
    camScale -=  0.001f * timer.FrameElapsed();
  if(input.Keys[GLFW_KEY_MINUS])
    camScale +=  0.001f * timer.FrameElapsed();

  cam2d.setScale(camScale);
  cam2d.Target(gameLogic.getTarget(), timer);

  gameLogic.Update(cam2d.getCameraArea(), timer, input, &cam2d, correctedMouse());
  
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

    gameLogic.Draw(mRender);
        
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
