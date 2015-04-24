cmake_minimum_required (VERSION 2.8)
project (LeapMusic)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic -Wextra -lcurses -pedantic-errors")

find_package(OpenGL)
find_package(GLUT)

add_subdirectory (libraries)

add_executable ( Projekt
	Main.cpp
	shaders/main.vert
	shaders/main.frag
)
target_link_libraries ( Projekt
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
	${GLEW_LIBRARIES}
	GLUtilities
	VectorUtils3
	LoadObject
	LoadTGA
)

add_custom_command(TARGET Projekt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:Projekt>/shaders)

add_custom_command(TARGET Projekt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/models
        $<TARGET_FILE_DIR:Projekt>/models)

add_custom_command(TARGET Projekt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/textures
        $<TARGET_FILE_DIR:Projekt>/textures)
