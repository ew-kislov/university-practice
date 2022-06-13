cmake_minimum_required (VERSION 3.0)

set (CMAKE_CXX_STANDARD 11)

project (engine)

find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)

add_subdirectory (external)

include(FindFreetype)

include_directories(
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/freetype-2.10.3
	${FREETYPE_INCLUDE_DIRS}
	external/glew-1.13.0/include/
	external/stb_image
	src
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW
	${FREETYPE_LIBRARIES}
)

file(GLOB src
     "src/main.cpp"
     "src/*.glsl"
     "src/*.glsl"
)

add_executable(engine
	${src}
)
target_link_libraries(engine
	${ALL_LIBS}
)


