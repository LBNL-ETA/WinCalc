cmake_minimum_required(VERSION 3.14)

project( wincalc VERSION 2.6.12 LANGUAGES CXX )
set(LIB_NAME ${PROJECT_NAME})

# Enable MSVC file-level parallel compilation (/MP) for every target, including
# the FetchContent dependencies pulled in below via add_subdirectory. Without it
# the Visual Studio generator compiles the .cpp files inside a single .vcxproj
# (notably the large wincalc-test target) serially, regardless of --parallel.
if(MSVC)
    add_compile_options(/MP)
endif()

# Graceful local-sibling fallback: the `local` CMake preset sets
# FETCHCONTENT_SOURCE_DIR_<NAME> to ${sourceDir}/../<DepName> for each declared
# dependency. If a sibling working copy is missing, unset that variable here so
# FetchContent falls back to the declared remote instead of erroring out.
foreach(_dep WINDOWSCALCSTANDARDS OPTICALMEASUREMENTPARSER THMXPARSER WINDOWSCALC-ENGINE LIBRARYFEMTHERM GOOGLETEST)
    if(DEFINED FETCHCONTENT_SOURCE_DIR_${_dep})
        if(NOT EXISTS "${FETCHCONTENT_SOURCE_DIR_${_dep}}/CMakeLists.txt")
            message(STATUS
                "Local sibling for ${_dep} not found at "
                "${FETCHCONTENT_SOURCE_DIR_${_dep}}; falling back to remote.")
            unset(FETCHCONTENT_SOURCE_DIR_${_dep} CACHE)
        endif()
    endif()
endforeach()

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

set(WindowsCalcStandards_Branch "v1.2.4")
if(NOT TARGET window_standards)
    FetchContent_Declare(
        WindowsCalcStandards
        GIT_REPOSITORY https://github.com/LBNL-ETA/Windows-CalcStandards.git
        GIT_TAG ${WindowsCalcStandards_Branch}
    )
    FetchContent_MakeAvailable(WindowsCalcStandards)
endif()

set(OpticalMeasurementParser_Branch "v2.3.9")
if(NOT TARGET OpticalMeasurementParser)
    FetchContent_Declare(
        OpticalMeasurementParser
        GIT_REPOSITORY https://github.com/LBNL-ETA/OpticalMeasurementParser.git
        GIT_TAG ${OpticalMeasurementParser_Branch}
    )
    FetchContent_MakeAvailable(OpticalMeasurementParser)
endif()

set(THMXParser_Branch "v1.1.6")
if(NOT TARGET THMXParser)
    FetchContent_Declare(
        THMXParser
        GIT_REPOSITORY https://github.com/LBNL-ETA/THMXParser.git
        GIT_TAG ${THMXParser_Branch}
    )
    FetchContent_MakeAvailable(THMXParser)
endif()

set(WindowsCalcEngine_Branch "Version_1.0.71")
if(NOT TARGET Windows-CalcEngine)
    FetchContent_Declare(
        WindowsCalc-Engine
        GIT_REPOSITORY https://github.com/LBNL-ETA/Windows-CalcEngine.git
        GIT_TAG ${WindowsCalcEngine_Branch}
    )
    FetchContent_MakeAvailable(WindowsCalc-Engine)
endif()

set(LibraryFEMTherm_Branch "Version_0.21.3")
if(NOT TARGET LibraryFEMTHERM)
    FetchContent_Declare(
        LibraryFEMTherm
        GIT_REPOSITORY https://github.com/LBNL-ETA/LibraryFEMTherm.git
        GIT_TAG ${LibraryFEMTherm_Branch}
    )
    FetchContent_MakeAvailable(LibraryFEMTherm)
endif()

# Read transitive dependency versions from targets for the dependency diagram
get_property(BSDFXMLParser_Branch TARGET OpticalMeasurementParser PROPERTY BSDFXMLParser_Branch)
get_property(nlohmann_omp_Branch TARGET OpticalMeasurementParser PROPERTY nlohmann_Branch)
get_property(FileParse_Branch TARGET BSDFXMLParser PROPERTY FileParse_Branch)
get_property(miniz_Branch TARGET LibraryFEMTHERM PROPERTY miniz_Branch)
get_property(LBNLCPPCommon_Branch TARGET LibraryFEMTHERM PROPERTY LBNLCPPCommon_Branch)

configure_file(doc/WinCalc.md.in ${CMAKE_CURRENT_SOURCE_DIR}/doc/WinCalc.md @ONLY)

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
