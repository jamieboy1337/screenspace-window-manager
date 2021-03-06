#include <critter/GameObject.hpp>
#include <critter/Visitor.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <memory>

#include <boost/log/trivial.hpp>

#include <critter/visitor/ActiveCameraFindVisitor.hpp>

namespace monkeysworld {
namespace critter {

using utils::IDGenerator;
using critter::visitor::ActiveCameraFindVisitor;
using engine::Context;

GameObject::GameObject() : GameObject(nullptr) { }

GameObject::GameObject(Context* ctx) : Object(ctx) {
  this->parent_ = std::weak_ptr<GameObject>();
  this->dirty_ = true;
  this->position = glm::vec3(0);
  this->rotation = glm::vec3(0);
  this->scale = glm::vec3(1);
}

void GameObject::Accept(Visitor& v) {
  v.Visit(std::dynamic_pointer_cast<GameObject>(this->shared_from_this()));
}

void GameObject::AddChild(std::shared_ptr<GameObject> child) {
  // if the child is a parent (direct or indirect) this will fail.
  if (child->GetChild(this->GetId()) != NULL) {
    return;
  }

  auto test = child->parent_.lock();
  BOOST_LOG_TRIVIAL(trace) << "Adding child with ID " << child->GetId();

  if (auto parent = child->parent_.lock()) {
    parent->RemoveChild(child->GetId());
    BOOST_LOG_TRIVIAL(trace) << "Removed old parent";
  }

  child->parent_ = std::weak_ptr<GameObject>(this->shared_from_this());
  // child is moved here -- don't want it in multiple locations
  children_.push_back(child);
}

std::shared_ptr<Object> GameObject::GetChild(uint64_t id) {
  // direct nesting will probably be common
  for (auto child : children_) {
    if (child->GetId() == id) {
      return child;
    }
  }

  // check for multiple nesting levels
  std::shared_ptr<Object> result;
  for (auto child : children_) {
    result = child->GetChild(id);
    if (result != nullptr) {
      return result;
    }
  }

  return nullptr;
}

std::vector<std::shared_ptr<Object>> GameObject::GetChildren() {
  std::vector<std::shared_ptr<Object>> res;
  for (auto child : children_) {
    res.push_back(std::static_pointer_cast<Object>(child));
  }

  return res;
}

std::shared_ptr<Object> GameObject::GetParent() {
  auto temp = parent_.lock();
  return temp;
}

void GameObject::SetPosition(const glm::vec3& new_pos) {
  dirty_.store(true, std::memory_order_release);
  position = new_pos;
}

void GameObject::SetRotation(const glm::vec3& new_rot) {
  dirty_.store(true, std::memory_order_release);
  rotation = new_rot;
}

void GameObject::SetScale(const glm::vec3& new_scale) {
  dirty_.store(true, std::memory_order_release);
  scale = new_scale;
}

glm::mat4 GameObject::GetTransformationMatrix() const {
  if (dirty_) {
    // const cast this cache var
    // the external state does not change, the cache only exists to save some matrix calcs
    glm::mat4& matrix_cache = const_cast<glm::mat4&>(tf_matrix_cache_);
    matrix_cache = glm::mat4(1.0);
    // scales, then rotates, then translates
    matrix_cache = glm::translate(tf_matrix_cache_, position);
    matrix_cache *= glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);
    matrix_cache = glm::scale(tf_matrix_cache_, scale);
  }

  if (auto parent = parent_.lock()) {
    return parent->GetTransformationMatrix() * tf_matrix_cache_;
  }

  return tf_matrix_cache_;
}

void GameObject::RemoveChild(uint64_t id) {
  for (auto ptr = children_.begin(); ptr != children_.end(); ptr++) {
    if ((*ptr)->GetId() == id) {
      children_.erase(ptr);
      return;
    }
  }

  // check descendants
  for (auto child : children_) {
    child->RemoveChild(id);
  }
}

const glm::vec3& GameObject::GetRotation() const {
  return rotation;
}

const glm::vec3& GameObject::GetPosition() const {
  return position;
}
const glm::vec3& GameObject::GetScale() const {
  return scale;
}

// superctor for gameobject :)
GameObject::GameObject(const GameObject& other) : Object(other) {
  position = other.position;
  rotation = other.rotation;
  scale = other.scale;

  parent_ = std::weak_ptr<GameObject>();
  dirty_ = true;

  // deep copy the children
  for (auto child : other.children_) {
    // this is ok
    AddChild(child);
  }
}

GameObject::GameObject(GameObject&& other) : Object(other) {
  position = std::move(other.position);
  rotation = std::move(other.rotation);
  scale = std::move(other.scale);

  if (auto other_parent = other.parent_.lock()) {
    other_parent->RemoveChild(other.GetId());
    other_parent->AddChild(shared_from_this());
  }

  dirty_ = true;

  // cannot copy over parent/child relationship
  // if for some reason this occurs: must rebind the parent

  children_ = std::move(other.children_);
}

GameObject& GameObject::operator=(const GameObject& other) {
  Object::operator=(other);
  position = other.position;
  rotation = other.rotation;
  scale = other.scale;

  parent_ = std::weak_ptr<GameObject>();
  dirty_ = true;

  for (auto child : other.children_) {
    AddChild(child);
  }

  return *this;
}

GameObject& GameObject::operator=(GameObject&& other) {
  Object::operator=(other);
  position = std::move(other.position);
  rotation = std::move(other.rotation);
  scale = std::move(other.scale);

  if (auto other_parent = other.parent_.lock()) {
    other_parent->RemoveChild(other.GetId());
    other_parent->AddChild(shared_from_this());
  }

  dirty_ = true;

  children_ = std::move(other.children_);

  return *this;
}

std::shared_ptr<Camera> GameObject::GetActiveCamera() {
  std::shared_ptr<GameObject> parent;
  if ((parent = std::dynamic_pointer_cast<GameObject>(GetParent())) != nullptr) {
    return parent->GetActiveCamera();
  }

  // at the root -- read full tree
  ActiveCameraFindVisitor v;
  Accept(v);
  return v.GetActiveCamera();
}

} // namespace critter
} // namespace monkeysworld