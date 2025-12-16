#cmake_minimum_required(VERSION 3.22)
#project(breakout)
#
#set(CMAKE_CXX_STANDARD 20)
#
#find_package(raylib CONFIG REQUIRED)
#find_package(glfw3  CONFIG REQUIRED)
#if(APPLE)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
#elseif(UNIX)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fsanitize=address -fsanitize=undefined")
#endif()
#
#add_executable(
#    breakout
#        breakout.cpp
#        game.h
#        sprite.h
#        sprite.cpp
#        assets.h
#        assets.cpp
#        graphics.h
#        graphics.cpp
#        level.h
#        level.cpp
#        ball.h
#        ball.cpp
#        paddle.h
#        paddle.cpp
#)
#target_link_libraries(breakout PRIVATE raylib glfw)
#
#add_executable(test test.cpp)
#target_link_libraries(test PRIVATE raylib glfw)


cmake_minimum_required(VERSION 3.22)
project(breakout CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Downloading packages
include(FetchContent)

# raylib
set(RAYLIB_VERSION 5.5)
FetchContent_Declare(
        raylib
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
)
FetchContent_MakeAvailable(raylib)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples

# box2d
set(BOX2D_VERSION 3.1.1)
FetchContent_Declare(
        box2d
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/erincatto/box2d/archive/refs/tags/v${BOX2D_VERSION}.tar.gz
)
FetchContent_MakeAvailable(box2d)

# Building project

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp") # Define PROJECT_SOURCES as a list of all source files
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/src/") # Define PROJECT_INCLUDE to be the path to the include directory of the project

if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
elseif(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fsanitize=address -fsanitize=undefined")
endif()

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} box2d)

#add_executable(test test.cpp src/level.cpp src/assets.cpp src/graphics.cpp)
#target_link_libraries(test raylib box2d glfw3)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

# Bundling assets (developer mode)
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data/")