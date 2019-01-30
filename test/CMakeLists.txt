include_directories("${PROJECT_SOURCE_DIR}")

set(PROJECT_TEST_NAME ${PROJECT_NAME}-test)

file( GLOB TEST_CPP RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp" )

message("TEST_CPP " ${TEST_CPP} )

add_executable(${PROJECT_TEST_NAME} ${TEST_CPP} )

include_directories(${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_TEST_NAME} gtest gmock_main ${PROJECT_NAME})

add_test(NAME ${PROJECT_TEST_NAME}-runner COMMAND ${PROJECT_TEST_NAME})

