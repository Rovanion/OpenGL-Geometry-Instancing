cmake_minimum_required (VERSION 2.8)
project (LeapMusic)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -pedantic-errors")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=c99 -Wall")

find_package(OpenGL)
find_package(GLUT)

add_subdirectory (libraries)

add_executable ( Projekt
	main.c
	instancing.c
	shaders/instancing.vert
	shaders/instancing.frag
	shaders/skybox.vert
	shaders/skybox.frag
)
target_link_libraries ( Projekt
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
	GLUtilities
	VectorUtils3
	LoadObject
	LoadTGA
	m
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
