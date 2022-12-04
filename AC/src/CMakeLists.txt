# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources	
			${CMAKE_SOURCE_DIR}/include/*.h
			${CMAKE_SOURCE_DIR}/interfaces/*.*
			${CMAKE_SOURCE_DIR}/src/*.*
)

idf_component_register(SRCS ${app_sources})

set(app_sources
	"main.cpp"
)
