cmake_minimum_required(VERSION 3.5)

project( wincalc VERSION 1.1.0 LANGUAGES CXX )
set(LIB_NAME ${PROJECT_NAME})

if(NOT "${CMAKE_CXX_STANDARD}")
	set(CMAKE_CXX_STANDARD 14)
endif()

if(NOT "${CMAKE_CXX_EXTENSIONS}")
	set(CMAKE_CXX_EXTENSIONS OFF)
endif()

include(GNUInstallDirs)
if(NOT "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()

if(NOT "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()

if(NOT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
endif()

set(BUILD_Windows_CalcStandards_tests OFF CACHE BOOL "")
set(BUILD_Optical_Measurement_Parser_Tests OFF CACHE BOOL "")
set(BUILD_WCE_TESTING OFF CACHE BOOL "")

include(CMakeLists-Windows-Standards.txt)
include(CMakeLists-OpticalMeasurementParser.txt)
include(CMakeLists-Windows-CalcEngine.txt)

add_subdirectory(src)

target_include_directories(${LIB_NAME}
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

Option(BUILD_WinCalc_tests "Build WinCalc tests." ON)

if(BUILD_WinCalc_tests)
	enable_testing()
	add_subdirectory( test )
endif()

