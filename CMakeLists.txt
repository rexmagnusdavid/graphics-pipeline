cmake_minimum_required(VERSION 3.20)
project(GraphicsPipeline)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build the tests" OFF)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(TIFF REQUIRED)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui)
file(GLOB IMGUI_SOURCES
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
)

file(GLOB APP_SOURCES "src/*.cpp")

add_executable(renderer ${APP_SOURCES} ${IMGUI_SOURCES})

target_include_directories(renderer PRIVATE
    include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(renderer PRIVATE
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    glfw
    ${TIFF_LIBRARIES}
)

if(BUILD_TESTS)
  find_package(GTest REQUIRED)
  enable_testing()

  add_executable(run_tests
    tests/test_vector_3.cpp
    tests/test_matrix_3x3.cpp
    tests/test_planar_pinhole_camera.cpp
    src/vector_3.cpp
    src/matrix_3x3.cpp
    src/planar_pinhole_camera.cpp
  )

  target_include_directories(run_tests PRIVATE include)

  target_link_libraries(run_tests PRIVATE GTest::GTest GTest::Main)

  add_test(NAME run_tests COMMAND run_tests)
endif()