#include <critter/GameObject.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <memory>

#include <boost/log/trivial.hpp>

namespace monkeysworld {
namespace critter {

using utils::IDGenerator;

GameObject::GameObject() : GameObject(nullptr) { }

GameObject::GameObject(Context* ctx) : Object(ctx) {
  this->parent_ = std::weak_ptr<GameObject>();
  this->dirty_ = true;
  this->position = glm::vec3(0);
  this->rotation = glm::vec3(0);
  this->scale = glm::vec3(1);
}

void GameObject::AddChild(std::shared_ptr<GameObject> child) {
  // if the child is a parent (direct or indirect) this will fail.
  if (child->GetChild(this->GetId()) != NULL) {
    return;
  }

  BOOST_LOG_TRIVIAL(trace) << child->parent_.expired();
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

GameObject* GameObject::GetChild(uint64_t id) {
  // direct nesting will probably be common
  for (auto child : children_) {
    if (child->GetId() == id) {
      return child.get();
    }
  }

  // check for multiple nesting levels
  GameObject* result;
  for (auto child : children_) {
    result = child->GetChild(id);
    if (result != nullptr) {
      return result;
    }
  }

  return nullptr;
}

GameObject* GameObject::GetParent() {
  return parent_.lock().get();
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

glm::mat4 GameObject::GetTransformationMatrix() {
  if (dirty_) {
    tf_matrix_cache_ = glm::mat4(1.0);
    // scales, then rotates, then translates
    tf_matrix_cache_ = glm::translate(tf_matrix_cache_, position);
    tf_matrix_cache_ *= glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
    tf_matrix_cache_ = glm::scale(tf_matrix_cache_, scale);
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

} // namespace critter
} // namespace monkeysworld