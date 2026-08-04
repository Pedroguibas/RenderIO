cmake_minimum_required(VERSION 3.20)

project(renderIO)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/assimp)

add_subdirectory(external/glfw)

add_subdirectory(external/glm)

add_library(glad
	external/glad/src/gl.c
)

target_include_directories(glad SYSTEM PUBLIC
	external/glad/include
)

add_executable(renderIO
	main.cpp
	src/Vertex.cpp
	src/Shader.cpp
	src/Mesh.cpp
	src/Camera.cpp
	src/MeshFactory.cpp
	src/Texture.cpp
	src/stb_image.cpp
	src/Lights.cpp
	src/Material.cpp
	src/Model.cpp
)

target_include_directories(renderIO PRIVATE
	include
	external/stb
)

target_link_libraries(renderIO PRIVATE
	glfw
	glad
	assimp
	glm::glm
)

add_custom_command(TARGET renderIO POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:renderIO>/shaders
)

add_custom_command(TARGET renderIO POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/textures
        $<TARGET_FILE_DIR:renderIO>/textures
)

target_link_options(renderIO PRIVATE
	-static
	-static-libgcc
	-static-libstdc++
)