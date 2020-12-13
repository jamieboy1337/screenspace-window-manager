#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <file/CachedFileLoader.hpp>
#include <engine/Scene.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <critter/Camera.hpp>
#include <input/WindowEventManager.hpp>

#include <memory>

namespace monkeysworld {
namespace critter {

class Context;

}
}

namespace monkeysworld {
namespace engine {
namespace baseengine {
void UpdateCtx(double, critter::Context*);
}
}
}

namespace monkeysworld {
namespace critter {


/**
 *  The `Context` class intends to provide access to components for which a single instance will be associated
 *  with execution at a time.
 * 
 *  For instance, handlers for audio, file loaders, global event broadcasters, etc.
 *  All resources here *should* be threadsafe.
 */ 
class Context {
  friend void ::monkeysworld::engine::baseengine::UpdateCtx(double, Context*);
 public:
  /**
   *  Default constructor which attempts to initialize all fields.
   *  @param window - reference to the GLFWwindow this context occupies
   *  TODO: Reconfigure the context so that we can spin it up on each scene.
   */ 
  Context(GLFWwindow* window);

  // the following functions return some higher level component

  // add timer + timed event handler

  // file loader
  const std::shared_ptr<file::CachedFileLoader> GetCachedFileLoader();

  const std::shared_ptr<input::WindowEventManager> GetEventManager();

  /**
   *  Returns the size of the framebuffer.
   *  @param width -  output param for width.
   *  @param height - output param for height.
   */  
  void GetFramebufferSize(int* width, int* height);

  /**
   *  Notifies the context that it should swap to the next scene
   */ 
  void SwapScene(std::shared_ptr<engine::Scene> scene);

  /**
   *  If a new scene has been set, this will return it.
   */ 
  std::shared_ptr<engine::Scene> GetNextScene();
  // functions to add some object as the root
  // functions to get delta time
  double GetDeltaTime();

 private:
  std::shared_ptr<file::CachedFileLoader> file_loader_;
  std::shared_ptr<input::WindowEventManager> event_mgr_;
  GLFWwindow* window_;
  double frame_delta_;

 protected:
  // anything which a transitive relationship cant solve
  void FrameUpdate();
};

} // namespace critter
} // namespace monkeysworld

#endif