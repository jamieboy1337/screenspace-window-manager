#include <shader/light/SpotLight.hpp>
#include <boost/log/trivial.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace monkeysworld {
namespace shader {
namespace light {

using critter::GameObject;
using critter::Context;

SpotLight::SpotLight(Context* ctx) : GameObject(ctx) {
  angle_ = 45.0f; // simple default
  // setup the framebuffer
  glGenTextures(1, &map_);

  ChangeMapSize(1024);

  glGenFramebuffers(1, &shadow_fb_);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_fb_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, map_, 0);

  if (!glCheckFramebufferStatus(GL_FRAMEBUFFER_COMPLETE)) {
    BOOST_LOG_TRIVIAL(warning) << "Framebuffer not complete!";
  }

  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
}

void SpotLight::SetAngle(float deg) {
  angle_ = deg;
}

glm::mat4 SpotLight::GetLightMatrix() {
  // ballparking these numbers -- may need better ones :)
  glm::mat4 persp = glm::perspective(glm::radians(angle_), 1.0f, 0.01f, 100.0f);
  return persp * glm::inverse(GetTransformationMatrix());
}

void SpotLight::ChangeMapSize(int px) {
  map_size_ = px;
  glBindTexture(GL_TEXTURE_2D, map_);

  // apparently hw is only 24
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, map_size_, map_size_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

GLuint SpotLight::GetMapDesc() {
  return map_;
}

GLuint SpotLight::GetFramebuffer() {
  return shadow_fb_;
}

int SpotLight::GetMapSize() {
  return map_size_;
}

}
}
}