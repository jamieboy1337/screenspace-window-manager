cmake_minimum_required(VERSION 3.15.0)
project(monkeys-world VERSION 0.0.1)

include(CTest)
enable_testing()

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

# separate out a bit better for testing
add_library(monkeys-world-components 
                                    ${SRC_DIR}/storage/VertexPacketTypes.cpp

                                    ${SRC_DIR}/shader/ShaderProgramBuilder.cpp
                                    ${SRC_DIR}/shader/ShaderProgram.cpp
                                    ${SRC_DIR}/shader/FilterSequence.cpp

                                    ${SRC_DIR}/shader/materials/ButtonMaterial.cpp
                                    ${SRC_DIR}/shader/materials/ImageFilterMaterial.cpp
                                    ${SRC_DIR}/shader/materials/FillMaterial.cpp
                                    ${SRC_DIR}/shader/materials/MatteMaterial.cpp
                                    ${SRC_DIR}/shader/materials/ShadowMapMaterial.cpp
                                    ${SRC_DIR}/shader/materials/TextMaterial.cpp
                                    ${SRC_DIR}/shader/materials/SkyboxMaterial.cpp
                                    ${SRC_DIR}/shader/materials/UIGroupMaterial.cpp
                                    ${SRC_DIR}/shader/materials/TextureXferMaterial.cpp

                                    ${SRC_DIR}/shader/color/Gradient.cpp

                                    ${SRC_DIR}/model/FullscreenQuad.cpp

                                    ${SRC_DIR}/file/CacheStreambuf.cpp
                                    ${SRC_DIR}/file/CachedFileLoader.cpp
                                    ${SRC_DIR}/file/LoaderThreadPool.cpp
                                    ${SRC_DIR}/file/AudioLoader.cpp
                                    ${SRC_DIR}/file/CubeMapLoader.cpp
                                    ${SRC_DIR}/file/FileLoader.cpp
                                    ${SRC_DIR}/file/FontLoader.cpp
                                    ${SRC_DIR}/file/ModelLoader.cpp
                                    ${SRC_DIR}/file/TextureLoader.cpp

                                    ${SRC_DIR}/utils/FileUtils.cpp
                                    ${SRC_DIR}/utils/IDGenerator.cpp
                                    ${SRC_DIR}/utils/ObjectGraph.cpp

                                    ${SRC_DIR}/input/WindowEventManager.cpp
                                    ${SRC_DIR}/input/ClickListener.cpp
                                    ${SRC_DIR}/input/KeyListener.cpp
                                    ${SRC_DIR}/input/Cursor.cpp

                                    ${SRC_DIR}/critter/GameObject.cpp
                                    ${SRC_DIR}/critter/Object.cpp
                                    ${SRC_DIR}/critter/Model.cpp
                                    ${SRC_DIR}/critter/GameCamera.cpp
                                    ${SRC_DIR}/critter/Skybox.cpp

                                    ${SRC_DIR}/critter/ui/UIObject.cpp
                                    ${SRC_DIR}/critter/ui/UIImage.cpp
                                    ${SRC_DIR}/critter/ui/UIGroup.cpp
                                    ${SRC_DIR}/critter/ui/UIButton.cpp
                                    ${SRC_DIR}/critter/ui/FPSCounter.cpp

                                    ${SRC_DIR}/critter/visitor/LightVisitor.cpp
                                    ${SRC_DIR}/critter/visitor/ActiveCameraFindVisitor.cpp
                                    
                                    ${SRC_DIR}/engine/EngineContext.cpp
                                    ${SRC_DIR}/engine/BaseEngine.cpp
                                    ${SRC_DIR}/engine/RenderContext.cpp
                                    ${SRC_DIR}/engine/SceneSwap.cpp
                                    ${SRC_DIR}/engine/EngineExecutor.cpp
                                    ${SRC_DIR}/engine/EngineWindow.cpp
                                    ${SRC_DIR}/engine/Scene.cpp

                                    ${SRC_DIR}/shader/light/SpotLight.cpp
                                    ${SRC_DIR}/shader/light/Light.cpp
                                    ${SRC_DIR}/shader/Texture.cpp
                                    ${SRC_DIR}/shader/CubeMap.cpp
                                    ${SRC_DIR}/shader/Framebuffer.cpp
                                    ${SRC_DIR}/shader/Canvas.cpp

                                    ${SRC_DIR}/audio/AudioBuffer.cpp
                                    ${SRC_DIR}/audio/AudioBufferOgg.cpp
                                    ${SRC_DIR}/audio/AudioManager.cpp
                                    ${SRC_DIR}/_stb_libs/stb_vorbis.c
                                    
                                    ${SRC_DIR}/font/Font.cpp
                                    ${SRC_DIR}/font/Text.cpp
                                    ${SRC_DIR}/font/TextObject.cpp
                                    ${SRC_DIR}/font/UITextObject.cpp)

add_library(MonkeysWorld::Engine ALIAS monkeys-world-components)

set_property(TARGET monkeys-world-components PROPERTY CXX_STANDARD 14)

target_include_directories(monkeys-world-components PUBLIC ${INC_DIR})
target_include_directories(monkeys-world-components PRIVATE ${STB_INCLUDE_DIRS})

### GLFW ###

find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(monkeys-world-components glfw)

### GLAD ###
set(GLAD_DIR deps/glad)
add_library(glad ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)
add_library(glad::glad ALIAS glad)

target_link_libraries(monkeys-world-components glad::glad)

### FREETYPE ###
find_package(freetype CONFIG REQUIRED)

### GTEST ###
find_package(GTest CONFIG REQUIRED)

### GLM ###
find_package(glm CONFIG REQUIRED)

### STB LIBS ###
find_path(STB_INCLUDE_DIRS "stb.h")

### PORTAUDIO ###
find_package(portaudio CONFIG REQUIRED)

### BOOST ###
# side note: this doesn't work because vcpkg.cmake overrides USE_STATIC_LIBS.
# i'd have to load the static ver, which i might do later, but i cant right now
set(BOOST_USE_MULTITHREADED ON)
# side note: this doesn't work because vcpkg.cmake overrides USE_STATIC_LIBS.
set(BOOST_USE_STATIC_LIBS   ON)
# i'd have to load the static ver, which i might do later, but i cant right now
set(Boost_DEBUG             ON)
find_package(Boost 1.72.0 COMPONENTS log regex REQUIRED)

# this should be all i need for now
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG ${CMAKE_CXX_FLAGS_DEBUG}")

target_link_libraries(monkeys-world-components glm glad::glad freetype)
target_include_directories(monkeys-world-components PUBLIC ${Boost_INCLUDE_DIRS} ${STB_INCLUDE_DIRS})
target_link_libraries(monkeys-world-components ${Boost_LIBRARIES} portaudio)
target_include_directories(monkeys-world-components PUBLIC ${Boost_INCLUDE_DIRS})




if(compile_tests)

  add_executable(scene-test test/demos/SceneTest.cpp)
  target_link_libraries(scene-test monkeys-world-components )

  # tba: fix this command
  add_custom_command(TARGET scene-test POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:monkeys-world-components>/resources)

  add_executable(mesh-test test/MeshTest.cpp)
  target_include_directories(mesh-test PRIVATE ${INC_DIR})
  target_link_libraries(mesh-test GTest::gtest_main GTest::gmock_main monkeys-world-components)
  target_include_directories(mesh-test PRIVATE ${Boost_INCLUDE_DIRS})
  add_test(NAME mesh-test COMMAND mesh-test WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  add_executable(shader-compile-test test/ShaderCompilationTest.cpp)
  target_include_directories(shader-compile-test PRIVATE ${INC_DIR})
  target_link_libraries(shader-compile-test GTest::gtest_main GTest::gmock_main monkeys-world-components)
  add_test(NAME shader-compile-test COMMAND shader-compile-test
              WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  add_executable(font-test test/demos/FontDoubleFeature.cpp)
  target_link_libraries(font-test monkeys-world-components )

  add_executable(streambuf-test test/CacheStreambufTest.cpp)
  target_include_directories(streambuf-test PRIVATE ${INC_DIR})
  target_link_libraries(streambuf-test GTest::gtest_main monkeys-world-components)
  add_test(NAME streambuf-test COMMAND streambuf-test
              WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  add_executable(loader-test test/CacheFileLoaderTest.cpp)
  target_include_directories(loader-test PRIVATE ${INC_DIR})
  target_link_libraries(loader-test GTest::gtest_main monkeys-world-components)
  add_test(NAME loader-test COMMAND loader-test
              WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  add_executable(generator-test test/IDGeneratorTest.cpp)
  target_include_directories(generator-test PRIVATE ${INC_DIR})
  target_link_libraries(generator-test GTest::gtest_main monkeys-world-components)
  add_test(NAME generator-test COMMAND generator-test
              WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  add_executable(object-test test/GameObjectTest.cpp)
  target_include_directories(object-test PRIVATE ${INC_DIR})
  target_link_libraries(object-test GTest::gtest_main monkeys-world-components)
  add_test(NAME object-test COMMAND object-test
              WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  # add_executable(events-test test/EventManagerTest.cpp)
  # target_include_directories(events-test PRIVATE ${INC_DIR})
  # target_link_libraries(events-test GTest::gtest_main monkeys-world-components)
  # add_test(NAME events-test COMMAND events-test
  #             WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  add_executable(audio-buffer-test test/AudioBufferTest.cpp)
  target_include_directories(audio-buffer-test PRIVATE ${INC_DIR})
  target_link_libraries(audio-buffer-test GTest::gtest_main monkeys-world-components)
  add_test(NAME audio-buffer-test COMMAND audio-buffer-test
              WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  add_executable(audio-buffer-test-ogg test/AudioBufferOggTest.cpp)
  target_include_directories(audio-buffer-test-ogg PRIVATE ${INC_DIR})
  target_link_libraries(audio-buffer-test-ogg GTest::gtest_main monkeys-world-components)
  add_test(NAME audio-buffer-test-ogg COMMAND audio-buffer-test-ogg
              WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  add_executable(light-visitor-test test/LightVisitorTest.cpp)
  target_include_directories(light-visitor-test PRIVATE ${INC_DIR})
  target_link_libraries(light-visitor-test GTest::gtest_main monkeys-world-components)
  add_test(NAME light-visitor-test COMMAND audio-buffer-test
              WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  add_executable(camera-find-visitor-test test/ActiveCameraFindVisitorTest.cpp)
  target_include_directories(camera-find-visitor-test PRIVATE ${INC_DIR})
  target_link_libraries(camera-find-visitor-test GTest::gtest_main monkeys-world-components)
  add_test(NAME camera-find-visitor-test COMMAND camera-find-visitor-test
               WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)
  
  add_executable(thread-pool-test test/LoaderThreadPoolTest.cpp)
  target_link_libraries(thread-pool-test GTest::gtest_main monkeys-world-components)
  add_test(NAME thread-pool-test COMMAND thread-pool-test
               WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  add_executable(model-loader-test test/ModelLoaderTest.cpp)
  target_link_libraries(model-loader-test GTest::gtest_main monkeys-world-components)
  add_test(NAME model-loader-test COMMAND model-loader-test
              WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)
  
  add_executable(font-loader-test test/FontLoaderTest.cpp)
  target_link_libraries(font-loader-test GTest::gtest_main monkeys-world-components)
  add_test(NAME font-loader-test COMMAND font-loader-test
              WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  add_executable(texture-loader-test test/TextureLoaderTest.cpp)
  target_link_libraries(texture-loader-test GTest::gtest_main monkeys-world-components)
  add_test(NAME texture-loader-test COMMAND texture-loader-test
              WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  add_executable(executor-test test/EngineExecutorTest.cpp)
  target_link_libraries(executor-test GTest::gtest_main monkeys-world-components)
  add_test(NAME executor-test COMMAND executor-test
              WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  add_executable(graph-test test/ObjectGraphTest.cpp)
  target_link_libraries(graph-test GTest::gtest_main monkeys-world-components)
  add_test(NAME graph-test COMMAND graph-test
              WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

  add_executable(gradient-test test/GradientTest.cpp)
  target_link_libraries(gradient-test GTest::gtest_main monkeys-world-components)
  add_test(NAME gradient-test COMMAND gradient-test
              WORKING_DIRECTORY $<TARGET_FILE_DIR:monkeys-world-components>)

endif()

if(MSVC)
  target_compile_options(monkeys-world-components PRIVATE /W3)
else()
  target_compile_options(monkeys-world-components PRIVATE -Wall)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
 
### ONCE COMPLETE: provide a single import which will pull in all of these components under a unified library, or something