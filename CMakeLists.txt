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