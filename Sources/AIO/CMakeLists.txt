set(target AIO)

file(GLOB_RECURSE sources
	${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

add_library(${target} ${sources})
