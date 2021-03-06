#include "model/Mesh.hpp"
#include "storage/VertexPacketTypes.hpp"
#include "model/VertexDataContext.hpp"
#include "model/VertexDataContextGL.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <iostream>
#include <memory>

#define DEBUG_EPS 0.0001


namespace monkeysworldtest {
using ::monkeysworld::model::Mesh;
using ::monkeysworld::model::VertexDataContext;
using ::monkeysworld::model::VertexDataContextType;

struct DummyPacket {
  int data;
  static void Bind() {

  }
};

struct Dummy2DPacket {
  glm::vec2 position;
  glm::vec2 normals;
  static void Bind() {

  }
};

/**
 *  Mock for vertex data context
 */ 
template <typename Packet>
class MockVertexContext : public VertexDataContext<Packet> {
 public:
  MOCK_METHOD(void, UpdateBuffersAndPoint, (const std::vector<Packet>& data, const std::vector<unsigned int>& indices), (const, override));
  MOCK_METHOD(void, Point, (), (const, override));
  MOCK_METHOD(VertexDataContextType, GetType, (), (const, override));
};


class VertexDataTests : public ::testing::Test {
 protected:
  void SetUp() override {
    std::cout << "running vertex data tests..." << std::endl;
  }
};

// gl is not initialized!



TEST_F(VertexDataTests, CreateDummyPacket) {
  // https://stackoverflow.com/questions/8114276/how-do-i-pass-a-unique-ptr-argument-to-a-constructor-or-a-function
  // inline instantiation or std move existing
  Mesh<DummyPacket> data(std::make_unique<MockVertexContext<DummyPacket>>());
  data.AddVertex({1});
  data.AddVertex({2});
  data.AddVertex({3});

  const unsigned int* contents = reinterpret_cast<const unsigned int*>(data.GetVertexData());
  ASSERT_EQ(1, contents[0]);
  ASSERT_EQ(2, contents[1]);
  ASSERT_EQ(3, contents[2]);
  ASSERT_EQ(0, data.GetIndexCount());
  ASSERT_EQ(3, data.GetVertexCount());

  data.AddPolygon(0, 1, 2);
  ASSERT_EQ(3, data.GetIndexCount());

  data.PointToVertexAttribs();
};

TEST_F(VertexDataTests, CreateMock2DPacket) {
  Mesh<Dummy2DPacket> data(std::make_unique<MockVertexContext<Dummy2DPacket>>());
  data.AddVertex({{0.0, 1.0}, {2.0, 3.0}});
  data.AddVertex({{4.0, 5.0}, {6.0, 7.0}});

  const float* contents = reinterpret_cast<const float*>(data.GetVertexData());
  const float* contents_second = reinterpret_cast<const float*>(data.GetVertexData() + 1);
  ASSERT_NEAR(0.0, contents[0], DEBUG_EPS);
  ASSERT_NEAR(1.0, contents[1], DEBUG_EPS);
  ASSERT_NEAR(2.0, contents[2], DEBUG_EPS);
  ASSERT_NEAR(3.0, contents[3], DEBUG_EPS);
  ASSERT_NEAR(4.0, contents_second[0], DEBUG_EPS);
  ASSERT_NEAR(5.0, contents_second[1], DEBUG_EPS);
  ASSERT_NEAR(6.0, contents_second[2], DEBUG_EPS);
  ASSERT_NEAR(7.0, contents_second[3], DEBUG_EPS);

  ASSERT_EQ(2, data.GetVertexCount());
  ASSERT_EQ(0, data.GetIndexCount());

  data.PointToVertexAttribs();
}


};  // namespace monkeysworldtests