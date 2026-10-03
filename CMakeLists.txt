cmake_minimum_required(VERSION 3.21)

project(Nova LANGUAGES C CXX)

# Metal code is Objective-C++ (.mm files)
if(APPLE)
    enable_language(OBJCXX)
    set(CMAKE_OBJCXX_STANDARD 20)
    set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Options: choose renderer backends at configure time
# ---------------------------------------------------------------------------
if(APPLE)
    set(_NOVA_METAL_DEFAULT ON)
    set(_NOVA_OPENGL_DEFAULT OFF)
else()
    set(_NOVA_METAL_DEFAULT OFF)
    set(_NOVA_OPENGL_DEFAULT ON)
endif()

option(NOVA_RENDERER_OPENGL "Build the OpenGL renderer" ${_NOVA_OPENGL_DEFAULT})
option(NOVA_RENDERER_VULKAN "Build the Vulkan renderer" OFF)
option(NOVA_RENDERER_METAL  "Build the Metal renderer (Apple only)" ${_NOVA_METAL_DEFAULT})

if(NOVA_RENDERER_METAL AND NOT APPLE)
    message(FATAL_ERROR "NOVA_RENDERER_METAL is only available on Apple platforms.")
endif()

if(NOT (NOVA_RENDERER_OPENGL OR NOVA_RENDERER_VULKAN OR NOVA_RENDERER_METAL))
    message(FATAL_ERROR "Enable at least one renderer: NOVA_RENDERER_OPENGL, NOVA_RENDERER_VULKAN or NOVA_RENDERER_METAL.")
endif()

# ---------------------------------------------------------------------------
# Detect the operating system
# (check APPLE before UNIX, because UNIX is also true on macOS)
# ---------------------------------------------------------------------------
if(WIN32)
    set(NOVA_OS_DIR   Windows)
    set(NOVA_OS_DEFINE NOVA_PLATFORM_WINDOWS)
elseif(APPLE)
    set(NOVA_OS_DIR   MacOS)
    set(NOVA_OS_DEFINE NOVA_PLATFORM_MACOS)
elseif(UNIX)
    set(NOVA_OS_DIR   Linux)
    set(NOVA_OS_DEFINE NOVA_PLATFORM_LINUX)
else()
    message(FATAL_ERROR "Nova: unsupported operating system.")
endif()

message(STATUS "Nova: platform = ${NOVA_OS_DIR}")

# ---------------------------------------------------------------------------
# Source files
#
# Expected layout:
#   src/Nova.h, src/nvpch.h
#   src/Core/...                    platform-agnostic code (always built)
#   src/Platform/<Windows|MacOS|Linux>/...   built only on that OS
#   src/Platform/<OpenGL|Vulkan|Metal>/...   built only if that renderer is enabled
# ---------------------------------------------------------------------------
set(NOVA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Objective-C++ files are only picked up on Apple platforms
set(_NOVA_EXTENSIONS cpp h hpp)
if(APPLE)
    list(APPEND _NOVA_EXTENSIONS mm)
endif()

# nova_collect_sources(<out_var> <recurse: TRUE|FALSE> <dir>)
function(nova_collect_sources out_var recurse dir)
    set(patterns "")
    foreach(ext IN LISTS _NOVA_EXTENSIONS)
        list(APPEND patterns "${dir}/*.${ext}")
    endforeach()
    if(recurse)
        file(GLOB_RECURSE found CONFIGURE_DEPENDS ${patterns})
    else()
        file(GLOB found CONFIGURE_DEPENDS ${patterns})
    endif()
    set(${out_var} ${found} PARENT_SCOPE)
endfunction()

nova_collect_sources(NOVA_SOURCES FALSE ${NOVA_SRC_DIR})                      # Nova.h, nvpch.h
nova_collect_sources(NOVA_CORE_SOURCES TRUE ${NOVA_SRC_DIR}/Core)
nova_collect_sources(NOVA_OS_SOURCES TRUE ${NOVA_SRC_DIR}/Platform/${NOVA_OS_DIR})
list(APPEND NOVA_SOURCES ${NOVA_CORE_SOURCES} ${NOVA_OS_SOURCES})

set(NOVA_RENDERER_DEFINES "")

if(NOVA_RENDERER_OPENGL)
    nova_collect_sources(_src TRUE ${NOVA_SRC_DIR}/Platform/OpenGL)
    list(APPEND NOVA_SOURCES ${_src})
    list(APPEND NOVA_RENDERER_DEFINES NOVA_RENDERER_OPENGL)
endif()

if(NOVA_RENDERER_VULKAN)
    nova_collect_sources(_src TRUE ${NOVA_SRC_DIR}/Platform/Vulkan)
    list(APPEND NOVA_SOURCES ${_src})
    list(APPEND NOVA_RENDERER_DEFINES NOVA_RENDERER_VULKAN)
endif()

if(NOVA_RENDERER_METAL)
    nova_collect_sources(_src TRUE ${NOVA_SRC_DIR}/Platform/Metal)
    list(APPEND NOVA_SOURCES ${_src})
    list(APPEND NOVA_RENDERER_DEFINES NOVA_RENDERER_METAL)
endif()

# ---------------------------------------------------------------------------
# Third-party libraries (git submodules in vendor/)
# ---------------------------------------------------------------------------
set(NOVA_VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

# GLFW: skip examples, tests and docs
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(${NOVA_VENDOR_DIR}/GLFW)

# GLM (header-only math library, ships its own CMake target)
add_subdirectory(${NOVA_VENDOR_DIR}/glm)

# Glad (OpenGL loader). Generate it at https://glad.dav1d.de/ (OpenGL 4.x core)
# and place it in vendor/glad/{include,src}. Only needed for the OpenGL renderer.
if(NOVA_RENDERER_OPENGL)
    if(NOT EXISTS ${NOVA_VENDOR_DIR}/glad/src/glad.c)
        message(FATAL_ERROR "OpenGL renderer enabled but vendor/glad/src/glad.c was not found.")
    endif()
    add_library(glad STATIC ${NOVA_VENDOR_DIR}/glad/src/glad.c)
    target_include_directories(glad PUBLIC ${NOVA_VENDOR_DIR}/glad/include)
    target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})
endif()

# ---------------------------------------------------------------------------
# The Nova library
# ---------------------------------------------------------------------------
add_library(Nova STATIC ${NOVA_SOURCES})
add_library(Nova::Nova ALIAS Nova)

# "src" is public so applications can write #include "Nova.h"
target_include_directories(Nova PUBLIC ${NOVA_SRC_DIR})

# stb (header-only): vendor/stb/stb_image.h etc. Used internally by Nova.
if(EXISTS ${NOVA_VENDOR_DIR}/stb)
    target_include_directories(Nova PRIVATE ${NOVA_VENDOR_DIR}/stb)
endif()

# Platform / renderer macros are public so apps can use them too
target_compile_definitions(Nova PUBLIC ${NOVA_OS_DEFINE} ${NOVA_RENDERER_DEFINES})

# Precompiled header (std includes + anything used almost everywhere).
# Only Nova's own sources use it; the generator expression keeps it away from
# any non-C++ files.
target_precompile_headers(Nova PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX,OBJCXX>:${NOVA_SRC_DIR}/nvpch.h>"
)

# Dependencies
target_link_libraries(Nova
        PRIVATE glfw
        PUBLIC  glm::glm
)

if(NOVA_RENDERER_OPENGL)
    target_link_libraries(Nova PRIVATE glad)
endif()

if(NOVA_RENDERER_VULKAN)
    # Needs the Vulkan SDK. On macOS the SDK also provides MoltenVK.
    find_package(Vulkan REQUIRED)
    target_link_libraries(Nova PRIVATE Vulkan::Vulkan)
endif()

if(NOVA_RENDERER_METAL)
    find_library(METAL_FRAMEWORK     Metal      REQUIRED)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    find_library(COCOA_FRAMEWORK     Cocoa      REQUIRED)
    target_link_libraries(Nova PRIVATE
            ${METAL_FRAMEWORK} ${QUARTZCORE_FRAMEWORK}
            ${FOUNDATION_FRAMEWORK} ${COCOA_FRAMEWORK})
endif()

# ---------------------------------------------------------------------------
# Compiler settings
# ---------------------------------------------------------------------------
if(MSVC)
    target_compile_options(Nova PRIVATE /W4 /MP /utf-8)
    target_compile_definitions(Nova PUBLIC
            NOMINMAX WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(Nova PRIVATE -Wall -Wextra)
endif()

# Mirror the folder layout in Visual Studio / Xcode
source_group(TREE ${NOVA_SRC_DIR} PREFIX "src" FILES ${NOVA_SOURCES})
