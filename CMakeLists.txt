cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("Starlight")

## Vulkan
find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)

## GLSL C
find_program(glslc_EXECUTABLE NAMES glslc HINTS Vulkan::glslc)

if (NOT glslc_EXECUTABLE)
	message(FATAL_ERROR "glslc executable not found. Please ensure that the Vulkan SDK is properly installed and that glslc is in your PATH.")
endif()

## Third party
include(FetchContent)

# Tracy 
FetchContent_Declare(
    tracy
    GIT_REPOSITORY https://github.com/wolfpld/tracy.git 
    GIT_TAG v0.13.1
    GIT_SHALLOW TRUE
)
set(TRACY_ENABLE ON)
set(TRACY_ON_DEMAND ON)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/icaven/glm
	GIT_SHALLOW TRUE
)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
	GIT_SHALLOW TRUE
)

# VMA 
FetchContent_Declare(
    vma
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    GIT_TAG v3.3.0
    GIT_SHALLOW TRUE
)
# Disable VMA sample builds by default
set(VMA_BUILD_SAMPLES OFF)

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
	GIT_TAG v1.92.7
	GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(tracy glm glfw vma imgui)

## ImGui as static lib
add_library(imgui 
    "${imgui_SOURCE_DIR}/imgui.cpp" 
    "${imgui_SOURCE_DIR}/imgui_demo.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp" 
    "${imgui_SOURCE_DIR}/imgui_tables.cpp" 
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp" 
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp"
)

target_link_libraries(imgui PRIVATE 
    glfw Vulkan::Vulkan
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})


if (WIN32)
    set(SRC ${SRC} "${CMAKE_CURRENT_SOURCE_DIR}/Assets/Icon/Resources.rc")
endif()

## Exe
add_executable(${PROJECT_NAME}
"Source/main.cpp"
"Include/Errors.h"
 "Include/Logger.h" "Source/Logger.cpp" "Include/Gpu.h" "Source/Gpu.cpp" "Include/Utility.h"
${SRC} "Include/Profiler.h" "Include/Memory.h")

target_link_libraries(${PROJECT_NAME} 
    Vulkan::Vulkan
    Threads::Threads
    VulkanMemoryAllocator
    glm
    glfw
    imgui
    TracyClient
)

add_custom_command(TARGET ${PROJECT_NAME} 
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory 
	"${CMAKE_CURRENT_SOURCE_DIR}/Shaders/"
	"$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders/"
)

function(CompileShader SRC_PATH DST_PATH PROFILE)
	
	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${glslc_EXECUTABLE} ${SOURCE_PATH} -o "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders/${DST_PATH}" --target-env=vulkan1.2
		COMMENT "Compiling Shader ${SRC_PATH} -> ${DST_PATH}"
	)

endfunction()

foreach(SHADER ${SHADERS})

	string(REGEX REPLACE "(\\.[^.]+)$" ".spv" OUT_SHADER ${SHADER})
	string(REGEX REPLACE "(.*\/)" "" OUT_SHADER ${OUT_SHADER})

	CompileShader(${SHADER} ${OUT_SHADER})

endforeach()