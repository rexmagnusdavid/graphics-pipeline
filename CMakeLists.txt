cmake_minimum_required(VERSION 3.20)
project(GraphicsPipeline)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(TIFF REQUIRED)
find_package(FLTK REQUIRED)

file(GLOB APP_SOURCES "src/*.cpp" "src/*.cxx")

add_executable(renderer ${APP_SOURCES})

target_include_directories(renderer PRIVATE include)

target_link_libraries(renderer PRIVATE
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${TIFF_LIBRARIES}
    ${FLTK_LIBRARIES}
)