include_directories("${PROJECT_SOURCE_DIR}")

set(PROJECT_TEST_NAME ${LIB_NAME}-test)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

add_executable(${PROJECT_TEST_NAME} 
		nfrc_102_2011_SA1_same_solar_and_visible.unit.cpp
		calc_from_disk.unit.cpp          
		calc_from_json.unit.cpp
		nfrc_102.unit.cpp
		nfrc_6046.unit.cpp
		nfrc_9023.unit.cpp
		nfrc_9223.unit.cpp
		nfrc_18100.unit.cpp
		nfrc_21467.unit.cpp
		cgdb_3000_nfrc_102.unit.cpp
		cgdb_18000_nfrc_102.unit.cpp
		user_woven_nfrc_102.unit.cpp
		main.cpp
		paths.h 
		util.h
		util.cpp)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)

target_compile_features(${PROJECT_TEST_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_TEST_NAME} gmock_main ${LIB_NAME})

add_test(NAME ${PROJECT_TEST_NAME}-runner COMMAND ${PROJECT_TEST_NAME} "${CMAKE_CURRENT_LIST_DIR}")

