#ifndef VERTEX_DATA_H_
#define VERTEX_DATA_H_

#include <algorithm>
#include <vector>

#include <glad/glad.h>

#include "opengl/VertexDataContext.hpp"

// from https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature/10707822#10707822
namespace screenspacemanager {
namespace storage {

/**
 *  Represents the data associated with the vertices which comprise a model.
 *  @tparam Packet - the specification for the vertex data which will be passed in.
 *                   this encompasses data types, and locations.
 */ 
template <typename Packet>
class VertexData {
 private:

  /**
   *  Test struct used to verify that bind method exists
   */ 
  template <typename T> struct HasBind {
    template <typename U, typename void (*)()> struct Tester {};
    template <typename U> static char Test(Tester<U, &U::Bind>*);
    template <typename U> static int Test(...);

    // true if `Bind` method is available, false otherwise
    static constexpr bool Has = (sizeof(Test<T>(NULL)) == sizeof(char));
  };

  static_assert(HasBind<Packet>::Has, "Bind method not available for templated class");
 public:
  /**
   *  Constructs a new VertexData object.
   */ 
  VertexData() : VertexData(::screenspacemanager::opengl::VertexDataContext<Packet>()) { }

  VertexData(::screenspacemanager::opengl::VertexDataContext<Packet> context) : context_(context) { }

  /**
   *  Stores a new vertex based on the contents of the data packet.
   *  @param packet - data packet containing vertex data.
   */ 
  void AddVertex(const Packet& packet) {
    data_.push_back(packet);
  }

  /**
   *  Stores a new triangle based on the described indices.
   *  @param vertA - First vertex
   *  @param vertB - Second vertex
   *  @param vertC - third vertex
   * 
   *  @return true if the vertices are in-bounds, and false otherwise.
   */ 
  bool AddPolygon(int vertA, int vertB, int vertC) {
    std::initializer_list<int> vert_list({vertA, vertB, vertC});
    int min_value = std::min(vert_list);
    int max_value = std::max(vert_list);
    if (min_value >= 0 && max_value < getVertexCount()) {
      indices_.push_back(vertA);
      indices_.push_back(vertB);
      indices_.push_back(vertC);
      return true;
    }

    return false;
  }

  /**
   *  Provides a wrapper for Packet::Bind so that methods working with this data class
   *  need not deduce the template type to bind attributes
   */ 
  void PointToVertexAttribs() {
    context_.PopulateBuffersAndBind(data_, indices_);
  }

  /**
   *  Returns number of vertices stored here.
   */ 
  size_t GetVertexCount() const {
    return data_.size();
  }

  /**
   *  Returns number of polygons stored in indices.
   */ 
  size_t GetIndexCount() const {
    return indices_.size();
  }

  /**
   *  Returns a read-only pointer to the underlying vertex data.
   */ 
  const Packet* GetVertexData() {
    return data_.data();
  }

  /**
   *  Returns a read-only pointer to the underlying index data.
   */ 
  const int* GetIndexData() {
    return indices_.data();
  }

  /**
   *  Concatenates the passed VertexData object onto `this`.
   *  @param other - the other VertexData being merged.
   */ 
  void Concatenate(const VertexData<Packet>& other) {
    int index_offset = other.indices_.size();
    for (Packet packet : other.data_) {
      data_.push_back(packet);
    }

    for (int index : other.indices_) {
      indices_.push_back(index + index_offset);
    }
  }

 private:
  // the underlying data stored.
  std::vector<Packet> data_;

  // list of indices representing polygons formed from our vertices. must be triangles
  std::vector<int> indices_;

  // context used to make opengl calls
  ::screenspacemanager::opengl::VertexDataContext<Packet> context_;

  
};

};  // namespace storage
};  // namespace screenspacemanager

#endif  // VERTEX_DATA_H_