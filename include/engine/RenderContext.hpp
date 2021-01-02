#ifndef RENDER_CONTEXT_H_
#define RENDER_CONTEXT_H_

#include <shader/light/LightTypes.hpp>
#include <critter/GameCamera.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

namespace monkeysworld {
namespace engine {

/**
 *  A context which contains information regarding the scene which is used
 *  by components while being rendered.
 */ 
class RenderContext {
  // lights
  // current active camera
  // i think thats all we need for now

 public:
  /**
   *  Returns a reference to the game camera.
   */ 
  critter::camera_info GetActiveCamera() const;

  /**
   *  Returns a list of infos for all spotlights. Client can handle however it wants to.
   */ 
  const std::vector<shader::light::spotlight_info>& GetSpotlights() const;

  // setters
  void SetActiveCamera(std::shared_ptr<critter::GameCamera> cam);
  void SetSpotlights(const std::vector<shader::light::spotlight_info>& spotlights);
 private:
  std::vector<shader::light::spotlight_info> spotlights_;
  critter::camera_info cam_info_;

};

}
}

#endif