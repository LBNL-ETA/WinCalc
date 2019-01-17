cmake_minimum_required( VERSION 2.8.7 )

message("Start WinCalc CMakeLists.txt")

set( target_name WinCalc )

set(CMAKE_CXX_STANDARD 11)

project( ${target_name} )

set(GLOBAL_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

include( cmake/WinCalcProjectMacros.cmake )
include( cmake/WinCalcInternalUtils.cmake )
include( cmake/WinCalcCompilerFlags.cmake )

option( BUILD_WINDOWS_STANDARDS_TESTING "Build testing targets" ON )
option( SINGLE_PROJECT "Build windows standards library as single project" OFF )
set( DOWNLOAD_GTEST ON )

# Sets global output directory for single configuration
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GLOBAL_OUTPUT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GLOBAL_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GLOBAL_OUTPUT_PATH})

include(CMakeLists-Windows-CalcEngine.txt)
include(CMakeLists-OpticalMeasurementParser.txt)
include(CMakeLists-Windows-Standards.txt)

if( DOWNLOAD_GTEST STREQUAL ON ) # Do not need gtest since it will be processed by E+
	message("DOWNLOAD_GTEST STREQUAL ON")
	# include(CMakeLists-GTest.txt)
	configure_file(CMakeLists.txt.in ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
	execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
	execute_process(COMMAND ${CMAKE_COMMAND} --build .
  		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
  		
  	# Prevent GoogleTest from overriding our compiler/linker options
	# when building with Visual Studio
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
 
	# Add googletest directly to our build. This adds
	# the following targets: gtest, gtest_main, gmock
	# and gmock_main
	add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
    	${CMAKE_BINARY_DIR}/googletest-build)
 
	# Now simply link your own targets against gtest, gmock,
	# etc. as appropriate
  		
    include_directories( ${CMAKE_BINARY_DIR}/googletest-src/include )
endif ()

include_directories(include)
#file(GLOB SOURCES "src/*.cpp")

file( GLOB SOURCES_CPP "src/*.cpp" )
file( GLOB SOURCES_HPP "src/*.h" )
file( GLOB INTERFACE_HPP "include/wincalc/*.h" )
LIST(APPEND SOURCES ${SOURCES_HPP} ${SOURCES_CPP} ${INTERFACE_HPP})

#add_subdirectory(MultiLayerOpticsCMD)

link_directories(${GLOBAL_OUTPUT_PATH})

add_library( ${target_name} STATIC ${SOURCES} )

target_link_libraries( ${target_name} ${WindowsCalcEngine_LIB} )
add_dependencies( ${target_name} Windows-CalcEngine )

target_link_libraries( ${target_name} ${WindowsStandards_LIB} )
add_dependencies( ${target_name} Windows-Standards )

target_link_libraries( ${target_name} ${OpticalMeasurementParser_LIB} )
add_dependencies( ${target_name} OpticalMeasurementParser )

if(BUILD_WINCALC_TESTING )
	include_directories(src)
	file( GLOB TEST_CPP RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "test/*.cpp" )
		
	CREATE_TEST_TARGETS_WINCALC( ${target_name} "${TEST_CPP}" "" )
endif()

if(MSVC)
	#This sets MT and MTd flags for Debug and Release modes for Windows-CalcEngine library
	target_compile_options(Windows-CalcEngine PRIVATE "/MD$<$<CONFIG:Debug>:d>")
	target_compile_options(OpticalMeasurementParser PRIVATE "/MD$<$<CONFIG:Debug>:d>")
endif()

config_compiler_and_linker_wincalc()