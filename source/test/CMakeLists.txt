enable_testing()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(Boost_USE_STATIC_LIBS ON)
endif()

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(unit_test ${SOURCES})

if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	target_compile_definitions(unit_test PRIVATE BOOST_TEST_DYN_LINK)
endif()

target_link_libraries(
    unit_test
    ${Boost_LIBRARIES}
    math)

add_test(NAME unit_test COMMAND unit_test)
add_custom_command(TARGET unit_test
    POST_BUILD
    COMMAND unit_test)
