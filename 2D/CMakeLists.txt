cmake_minimum_required(VERSION 3.10)
project(OpenGLGameOfLife)

set(CMAKE_CXX_STANDARD 17)

# GLAD
add_library(glad src/glad.c)
target_include_directories(glad PUBLIC include)

# GLFW: expliciet op Linux
find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h)
find_library(GLFW_LIBRARY NAMES glfw3 glfw)

if (NOT GLFW_INCLUDE_DIR OR NOT GLFW_LIBRARY)
    message(FATAL_ERROR "GLFW not found! Install libglfw3-dev.")
endif()

add_executable(app src/main.cpp)
target_include_directories(app PRIVATE ${GLFW_INCLUDE_DIR})
target_link_libraries(app PRIVATE glad ${GLFW_LIBRARY})
