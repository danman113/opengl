cmake_minimum_required(VERSION 3.1)
project(opengl VERSION 0.1.0)


set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# set(EXTERNAL_LIB_HEADERS )
add_subdirectory(deps/glfw)
add_subdirectory(deps/glad)
add_executable(opengl
	src/main.cpp
	src/window.cpp
	src/renderer.cpp
	src/fs.cpp
	src/texture.cpp
	src/utils.cpp
)

target_include_directories(opengl PUBLIC deps/stb/)

file(COPY resources DESTINATION .)
target_link_libraries(opengl PUBLIC glfw glad)

get_target_property(OUT opengl LINK_LIBRARIES)
message(STATUS ${OUT})