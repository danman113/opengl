cmake_minimum_required(VERSION 3.0.0)
project(opengl VERSION 0.1.0)


set(CXX_STANDARD 20)
# set(EXTERNAL_LIB_HEADERS )
add_subdirectory(deps/glfw deps/glad)
add_executable(opengl
	src/main.cpp
)
target_link_libraries(opengl PRIVATE glfw glad)
# target_link_libraries(opengl )

get_target_property(OUT opengl LINK_LIBRARIES)
message(STATUS ${OUT})