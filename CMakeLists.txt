cmake_minimum_required(VERSION 3.19)
project(MASK LANGUAGES CXX)

set(APP MASK) #executable name, can be changed
option(VENDORED "use vendored library for SDL2" OFF)

if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
    #set(MACOSX_BUNDLE TRUE) this builds the project as a .app in macosx for release
endif()
if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "C:/libs/SDL2" "C:/libs/SDL2_image")
endif()

if(VENDORED)
    add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)
else()
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
endif()

include_directories(src/ECS)

add_executable(${APP})
#if macosx bundle: add_executable(${APP} MACOSX_BUNDLE)
target_sources(${APP} PRIVATE 
    src/main.cpp
)

target_link_libraries(${APP} PRIVATE SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image)

if(APPLE)
    target_link_libraries(${APP} PRIVATE "-framework Cocoa")
endif()