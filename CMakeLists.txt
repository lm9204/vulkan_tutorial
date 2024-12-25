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

# External 디렉토리 설정
set(EXTERNAL_DIR ${CMAKE_BINARY_DIR}/external)

# stb_image.h URL 및 다운로드 경로 설정
set(STB_IMAGE_URL https://raw.githubusercontent.com/nothings/stb/master/stb_image.h)
set(STB_IMAGE_FILE ${EXTERNAL_DIR}/stb/stb_image.h)

# 다운로드 명령어 설정
if (UNIX)
    # Linux/macOS에서 wget 또는 curl 확인
    find_program(WGET_EXECUTABLE wget)
    find_program(CURL_EXECUTABLE curl)
    if (WGET_EXECUTABLE)
        set(DOWNLOAD_COMMAND ${WGET_EXECUTABLE} -O ${STB_IMAGE_FILE} ${STB_IMAGE_URL})
    elseif (CURL_EXECUTABLE)
        set(DOWNLOAD_COMMAND ${CURL_EXECUTABLE} -L -o ${STB_IMAGE_FILE} ${STB_IMAGE_URL})
    else()
        message(FATAL_ERROR "Neither wget nor curl was found. Please install one of them.")
    endif()
elseif (WIN32)
    # Windows에서 PowerShell 명령을 사용
    find_program(POWERSHELL_EXECUTABLE powershell)
    if (NOT POWERSHELL_EXECUTABLE)
        message(FATAL_ERROR "PowerShell is required to download files, but it was not found.")
    endif()
    set(DOWNLOAD_COMMAND ${POWERSHELL_EXECUTABLE} -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri ${STB_IMAGE_URL} -OutFile ${STB_IMAGE_FILE}")
else()
    message(FATAL_ERROR "Unsupported platform for downloading files.")
endif()

# 다운로드 디렉토리 생성 및 파일 다운로드
add_custom_command(
    OUTPUT ${STB_IMAGE_FILE}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${EXTERNAL_DIR}/stb
    COMMAND ${DOWNLOAD_COMMAND}
    COMMENT "Downloading stb_image.h from ${STB_IMAGE_URL}"
    VERBATIM
)

# 다운로드한 파일을 컴파일 의존성에 추가
add_custom_target(DownloadSTB ALL DEPENDS ${STB_IMAGE_FILE})

# GLFW 및 GLM 경로 설정
add_dependencies(vulkan_tutorial glfw glm DownloadSTB CopyTextures)

target_include_directories(vulkan_tutorial PRIVATE
    ${EXTERNAL_DIR}/glfw/install/include
    ${EXTERNAL_DIR}/glm/src/glm
    ${EXTERNAL_DIR}/stb
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

# -------------------------------------------------
# glslc 실행 파일 위치 설정
# -------------------------------------------------

# OS별 glslc 실행 파일 경로
if (WIN32)
    set(GLSLC_EXECUTABLE "${VULKAN_SDK}/Bin/glslc.exe")
elseif (APPLE)
    set(GLSLC_EXECUTABLE "${VULKAN_SDK}/bin/glslc")
elseif (UNIX)
    set(GLSLC_EXECUTABLE "${VULKAN_SDK}/bin/glslc")
else()
    message(FATAL_ERROR "Unsupported platform for glslc.")
endif()

# glslc 존재 여부 확인
if (NOT EXISTS ${GLSLC_EXECUTABLE})
    message(FATAL_ERROR "glslc not found at ${GLSLC_EXECUTABLE}. Please ensure Vulkan SDK is installed and the path is correct.")
endif()

# -------------------------------------------------
# 셰이더 컴파일 설정
# -------------------------------------------------

# 1. shader 디렉토리 생성
add_custom_target(
    CreateShaderOutputDir
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/shader
    COMMENT "Creating shader output directory"
)

# 2. shader/ 디렉토리 내 모든 .vert와 .frag 파일 검색
file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/shader/*.vert" "${CMAKE_SOURCE_DIR}/shader/*.frag")

# 3. 셰이더 파일 컴파일
set(COMPILED_SHADERS "")
foreach(SHADER_FILE ${SHADER_FILES})
    get_filename_component(FILENAME_WE ${SHADER_FILE} NAME_WE)
    get_filename_component(FILE_EXTENSION ${SHADER_FILE} EXT)

    set(OUTPUT_FILE "${CMAKE_BINARY_DIR}/shader/${FILENAME_WE}${FILE_EXTENSION}.spv")

    # 디렉토리 생성 타겟 의존성 추가
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND ${GLSLC_EXECUTABLE} ${SHADER_FILE} -o ${OUTPUT_FILE}
        DEPENDS ${SHADER_FILE} CreateShaderOutputDir
        COMMENT "Compiling shader: ${SHADER_FILE} -> ${OUTPUT_FILE}"
        VERBATIM
    )

    list(APPEND COMPILED_SHADERS ${OUTPUT_FILE})
endforeach()

# 4. CompileShaders 타겟 정의
add_custom_target(
    CompileShaders ALL
    DEPENDS ${COMPILED_SHADERS}
)

# 셰이더 컴파일 전용 타겟
add_custom_target(
    compile_shaders
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target CompileShaders
    COMMENT "Re-compiling all shaders"
)

# 소스 디렉토리 및 빌드 디렉토리 설정
set(SOURCE_TEXTURE_DIR ${CMAKE_SOURCE_DIR}/texture)
set(BUILD_TEXTURE_DIR ${CMAKE_BINARY_DIR}/texture)

add_custom_command(
    OUTPUT ${BUILD_TEXTURE_DIR}/.synced
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_TEXTURE_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${BUILD_TEXTURE_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_TEXTURE_DIR} ${BUILD_TEXTURE_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${BUILD_TEXTURE_DIR}/.synced
    COMMENT "Synchronizing texture directory"
    VERBATIM
)

# Texture 복사 타겟 생성
add_custom_target(CopyTextures ALL DEPENDS ${BUILD_TEXTURE_DIR}/.synced)