cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("Starlight")

## Vulkan
find_package(Vulkan REQUIRED)

## GLSL C
find_program(glslc_EXECUTABLE NAMES glslc HINTS Vulkan::glslc)

if (NOT glslc_EXECUTABLE)
	message(FATAL_ERROR "glslc executable not found. Please ensure that the Vulkan SDK is properly installed and that glslc is in your PATH.")
endif()

add_executable(${PROJECT_NAME}
	main.cpp
)

target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)