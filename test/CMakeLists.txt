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
		nfrc_102_nfrc_102_forced_ventilation.unit.cpp
		nfrc_102_nfrc_102_vacuum.unit.cpp
		deflection_triple_clear.unit.cpp
		deflection_youngs_modulus.unit.cpp
 		deflection.unit.cpp
 		deflection_environment.unit.cpp
 		deflection_load.unit.cpp
 		deflection_tilt.unit.cpp
 		deflection_density.unit.cpp
		user_vertical_venetian_nfrc_102.unit.cpp
		user_venetian_nfrc_102.unit.cpp
		user_perforated_nfrc_102.unit.cpp
		woven_shade.unit.cpp
		perforated_screen.unit.cpp
		#double_layer_perfect_clear_exterior_interior_xml_with_thermal_openings_10mm_all_sides.unit.cpp
		#double_layer_interior_xml_with_thermal_openings_10mm_all_sides.unit.cpp
		bsdf_xml_thermal_openness.unit.cpp
		CS03_genBSDF.unit.cpp
		SEATEX_Midnight.unit.cpp
		nfrc_102_as_bsdf.unit.cpp
		pv_single_layer.unit.cpp
		nfrc_102.unit.cpp
		nfrc_20748.unit.cpp
		nfrc_6046.unit.cpp
 		nfrc_9023.unit.cpp
 		nfrc_9223.unit.cpp
 		nfrc_18100.unit.cpp
 		nfrc_21467.unit.cpp
		igsdb_14025.cpp
		igsdb_5051_nfrc_102.unit.cpp
		WINDOW_issue_1216.unit.cpp
 		igsdb_14684_nfrc_102.unit.cpp
		cgdb_3000_vertical_nfrc_102.unit.cpp
 		cgdb_3000_nfrc_102.unit.cpp
 		cgdb_18000_nfrc_102.unit.cpp
 		user_woven_nfrc_102.unit.cpp
		nfrc_102_2011_SA1_same_solar_and_visible.unit.cpp
 		cma.unit.cpp
		quad_layer_interior_xml_with_thermal_openings_10mm_all_sides.unit.cpp
		main.cpp
		paths.h 
		util.h
		util.cpp)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)

target_compile_features(${PROJECT_TEST_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_TEST_NAME} gmock_main ${LIB_NAME})

add_test(NAME ${PROJECT_TEST_NAME}-runner COMMAND ${PROJECT_TEST_NAME} "${CMAKE_CURRENT_LIST_DIR}")

