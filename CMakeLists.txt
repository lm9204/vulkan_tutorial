cmake_minimum_required(VERSION 3.15)
project(vulkan_tutorial)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# IDE 빌드
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 소스 파일
set(SOURCE_FILES src/main.cpp)

# 실행 파일 생성
add_executable(vulkan_tutorial ${SOURCE_FILES})

# 외부 프로젝트 모듈
include(ExternalProject)

# 외부 라이브러리 경로 설정
set(EXTERNAL_DIR ${CMAKE_BINARY_DIR}/external)

# GLFW 클론 및 빌드 설정
ExternalProject_Add(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG latest
    PREFIX ${EXTERNAL_DIR}/glfw
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_DIR}/glfw/install
)

# GLM 클론 및 설정 (헤더 파일만 필요)
ExternalProject_Add(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG master
    PREFIX ${EXTERNAL_DIR}/glm
    CONFIGURE_COMMAND "" # GLM은 빌드 과정 필요 없음
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

# GLFW 및 GLM 경로 설정
add_dependencies(vulkan_tutorial glfw glm)

target_include_directories(vulkan_tutorial PRIVATE
    ${EXTERNAL_DIR}/glfw/install/include
    ${EXTERNAL_DIR}/glm/src/glm
)
target_link_libraries(vulkan_tutorial PRIVATE
    ${EXTERNAL_DIR}/glfw/install/lib/libglfw3.a
)

# Vulkan SDK 설정
if (NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VULKAN_SDK environment variable is not set. Please set it to the Vulkan SDK path.")
endif()

set(VULKAN_SDK $ENV{VULKAN_SDK})
target_include_directories(vulkan_tutorial PRIVATE ${VULKAN_SDK}/include)
target_link_libraries(vulkan_tutorial PRIVATE ${VULKAN_SDK}/lib/libvulkan.dylib)

# 플랫폼별 추가 설정
if (WIN32)
    target_link_libraries(vulkan_tutorial PRIVATE opengl32)
elseif (APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
    target_link_libraries(vulkan_tutorial PRIVATE ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
endif()