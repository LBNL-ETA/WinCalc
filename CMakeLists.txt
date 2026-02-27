cmake_minimum_required(VERSION 3.14)

project( wincalc VERSION 2.6.4 LANGUAGES CXX )
set(LIB_NAME ${PROJECT_NAME})

if(NOT "${CMAKE_CXX_STANDARD}")
	set(CMAKE_CXX_STANDARD 20)
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
set(BUILD_THMX_Parser_Tests OFF CACHE BOOL "")
set(BUILD_BSDFXMLParser_Tests OFF CACHE BOOL "")
set(BUILD_FileParse_tests OFF CACHE BOOL "")

include(FetchContent)

set(WindowsCalcStandards_Branch "v1.2.2")
if(NOT TARGET window_standards)
    FetchContent_Declare(
        WindowsCalcStandards
        GIT_REPOSITORY https://github.com/LBNL-ETA/Windows-CalcStandards.git
        GIT_TAG ${WindowsCalcStandards_Branch}
    )
    FetchContent_MakeAvailable(WindowsCalcStandards)
endif()

set(OpticalMeasurementParser_Branch "feature/disable-wide-char")
if(NOT TARGET OpticalMeasurementParser)
    FetchContent_Declare(
        OpticalMeasurementParser
        GIT_REPOSITORY https://github.com/LBNL-ETA/OpticalMeasurementParser.git
        GIT_TAG ${OpticalMeasurementParser_Branch}
    )
    FetchContent_MakeAvailable(OpticalMeasurementParser)
endif()

set(THMXParser_Branch "feature/disable-wide-char")
if(NOT TARGET THMXParser)
    FetchContent_Declare(
        THMXParser
        GIT_REPOSITORY https://github.com/LBNL-ETA/THMXParser.git
        GIT_TAG ${THMXParser_Branch}
    )
    FetchContent_MakeAvailable(THMXParser)
endif()

set(WindowsCalcEngine_Branch "feature/disable-wide-char")
if(NOT TARGET Windows-CalcEngine)
    FetchContent_Declare(
        WindowsCalc-Engine
        GIT_REPOSITORY https://github.com/LBNL-ETA/Windows-CalcEngine.git
        GIT_TAG ${WindowsCalcEngine_Branch}
    )
    FetchContent_MakeAvailable(WindowsCalc-Engine)
endif()

set(LibraryFEMTherm_Branch "feature/disable-wide-char")
if(NOT TARGET LibraryFEMTHERM)
    FetchContent_Declare(
        LibraryFEMTherm
        GIT_REPOSITORY https://github.com/LBNL-ETA/LibraryFEMTherm.git
        GIT_TAG ${LibraryFEMTherm_Branch}
    )
    FetchContent_MakeAvailable(LibraryFEMTherm)
endif()

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

	# Set the path to the test directory
    set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")
    target_compile_definitions(wincalc-test PRIVATE TEST_DATA_DIR="${TEST_DATA_DIR}")

    # Add the test with the specified arguments
    add_test(NAME wincalc-test
             COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/wincalc-test --gtest_catch_exceptions=0)
endif()
