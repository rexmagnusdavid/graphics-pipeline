cmake_minimum_required(VERSION 3.20)
project(GraphicsPipeline)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(TIFF REQUIRED)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui)
file(GLOB IMGUI_SOURCES 
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glut.cpp
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
    ${GLUT_LIBRARIES}
    ${TIFF_LIBRARIES}
)