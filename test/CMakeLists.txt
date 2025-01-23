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
		2_layers_venetian_cgdb_3000_uniform_diffuse_air_nfrc_102.unit.cpp
		2_layers_laminate_nfrc_18000_air_NFRC_102_quarter_basis.unit.cpp
		2_layers_nfrc_102_air_homogeneous_diffusing_shade_pella_white_venetian_blind_slat.unit.cpp
        2_layers_nfrc_102_air_perfectly_diffusing_shade_pella_white_venetian_blind_slat.unit.cpp
		2_layers_vertical_venetian_cgdb_3000_uniform_diffuse_air_nfrc_102.unit.cpp
		2_layers_vertical_venetian_pella_white_venetian_blind_slat_user_geometry_air_nfrc_102.unit.cpp		
		2_layers_venetian_pella_white_venetian_blind_slat_user_geometry_air_nfrc_102.unit.cpp
		2_layers_venetian_cgdb_3001_air_nfrc_102.unit.cpp
 		2_layers_woven_pella_white_venetian_blind_slat_user_geometry_air_nfrc_102.unit.cpp
		main.cpp
		paths.h 
		util.h
		util.cpp)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)

target_compile_features(${PROJECT_TEST_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_TEST_NAME} gmock_main ${LIB_NAME})
