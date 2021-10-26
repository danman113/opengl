cmake_minimum_required(VERSION 3.0.0)
project(opengl VERSION 0.1.0)


set(CXX_STANDARD 20)
# set(EXTERNAL_LIB_HEADERS )
add_subdirectory(deps/glfw)
add_subdirectory(deps/glad)
add_executable(opengl
	src/main.cpp
)
target_link_libraries(opengl PUBLIC glfw glad)

get_target_property(OUT opengl LINK_LIBRARIES)
message(STATUS ${OUT})