cmake_minimum_required(VERSION 3.1)
project(opengl VERSION 0.1.0)


set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# set(EXTERNAL_LIB_HEADERS )
# add_compile_definitions(WITH_MINIAUDIO)
set(WITH_MINIAUDIO TRUE)
# set(BACKEND_SRCS deps/soloud/src/backend/miniaudio/soloud_miniaudio.cpp)
add_subdirectory(deps/glfw)
add_subdirectory(deps/glad)
add_subdirectory(deps/glm)
add_subdirectory(deps/soloud/contrib/)
add_executable(opengl
	src/main.cpp
	src/window.cpp
	src/renderer.cpp
	src/shader.cpp
	src/fs.cpp
	src/texture.cpp
	src/mesh.cpp
	src/utils.cpp
	src/resourceLoader.cpp
	src/camera.cpp
)

target_include_directories(opengl PUBLIC deps/stb/)
target_include_directories(opengl PUBLIC deps/soloud/include)

file(COPY resources DESTINATION .)
target_link_libraries(opengl PUBLIC glfw glad glm soloud)

get_target_property(OUT opengl LINK_LIBRARIES)
message(STATUS ${OUT})