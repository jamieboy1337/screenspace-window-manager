#ifndef MOUSE_EVENT_H_
#define MOUSE_EVENT_H_

#include <glm/glm.hpp>


// quick to copy -- one for each nested component

namespace monkeysworld {
namespace input {

/**
 *  An event generated by mouse input.
 */ 
struct MouseEvent {
  // position, relative to an object's container, of the click event.
  glm::vec2 local_pos;

  // position, relative to the screen, of the click event.
  glm::vec2 absolute_pos;

  int button;
  int action;
  int mods;
};

}
}

#endif