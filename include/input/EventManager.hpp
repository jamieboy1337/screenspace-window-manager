#ifndef EVENT_MANAGER_H_
#define EVENT_MANAGER_H_

#include <functional>
#include <memory>

#include <input/Cursor.hpp>
#include <input/MouseEvent.hpp>

#include <input/ClickListener.hpp>
#include <input/KeyListener.hpp>

namespace monkeysworld {
namespace input {

class EventManager {
 public:
  /**
   *  @returns a ptr to the active cursor object.
   */ 
  virtual std::shared_ptr<Cursor> GetCursor() = 0;

  /**
   * Register a listener which will be called when an event is generated for the desired key.
   * @param key - The GLFW integer associated with the desired key.
   * @param callback - The callback which will be invoked when the respective event is invoked.
   *                   Callbacks must return void, and must accept three parameters:
   *                     - int: the GLFW identifier for the key which generated the event (see below, and https://www.glfw.org/docs/3.3.2/group__keys.html)
   *                     - int: the GLFW action associated with the event (see https://www.glfw.org/docs/3.3.2/group__input.html)
   *                     - int: Any key modifiers (see https://www.glfw.org/docs/3.3.2/group__mods.html)
   * @returns a uniquely identifying "event descriptor" which can be used to modify the event later.
   * (deprecated)
   */  
  virtual uint64_t RegisterKeyListener(int key, std::function<void(int, int, int)> callback) = 0;
  virtual KeyListener CreateKeyListener(int key, std::function<void(int, int, int)> callback) = 0;

  /**
   *  Register a listener which will be called when an event is generated by the mouse.
   *  @param callback - The callback which will be invoked when a mouse event occurs.
   *                    Callbacks must not return anything, and must accept a single parameter:
   *                      - MouseEvent: a struct representing the incoming mouse event.
   *  @returns a uniquely identifying "event descriptor" which can be used to modify the event later.
   *  (deprecated)
   */ 
  virtual uint64_t RegisterClickListener(std::function<void(MouseEvent)> callback) = 0;
  virtual ClickListener CreateClickListener(std::function<void(MouseEvent)> callback) = 0;

  /**
   * Removes the key listener associated with the provided event descriptor.
   * @param event_id - the ID of the event we wish to remove.
   * @returns true if the event was removed successfully -- false otherwise.
   */ 
  virtual bool RemoveKeyListener(uint64_t event_id) = 0;
  virtual void RemoveKeyListener(KeyListener& k) = 0;

  /**
   *  Removes a click listener associated with the provided descriptor.
   *  @param event_id - the ID of the callback we wish to remove.
   *  @returns true if the callback was removed -- false otherwise.
   */ 
  virtual bool RemoveClickListener(uint64_t event_id) = 0;
  virtual void RemoveClickListener(ClickListener& c) = 0;
};

}
}

#endif