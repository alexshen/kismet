file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(math ${SOURCES})

find_package(Boost REQUIRED)
target_include_directories(math PUBLIC ${Boost_INCLUDE_DIRS})
