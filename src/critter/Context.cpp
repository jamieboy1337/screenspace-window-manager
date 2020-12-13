#include <critter/Context.hpp>

namespace monkeysworld {
namespace critter {

using file::CachedFileLoader;

Context::Context(GLFWwindow* window) {
  file_loader_ = std::make_shared<CachedFileLoader>("context_cache");
  window_ = window;
}

void Context::GetFramebufferSize(int* width, int* height) {
  glfwGetFramebufferSize(window_, width, height);
  // event_mgr_->ProcessWaitingEvents();
}

const std::shared_ptr<CachedFileLoader> Context::GetCachedFileLoader() {
  return file_loader_;
}

void Context::FrameUpdate() {
  event_mgr_->ProcessWaitingEvents();
}

} // namespace critter
} // namespace monkeysworld