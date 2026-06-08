cmake_minimum_required(VERSION 3.14)

project( wincalc VERSION 2.6.15 LANGUAGES CXX )
set(LIB_NAME ${PROJECT_NAME})

# Enable MSVC file-level parallel compilation (/MP) for every target, including
# the FetchContent dependencies pulled in below via add_subdirectory. Without it
# the Visual Studio generator compiles the .cpp files inside a single .vcxproj
# (notably the large wincalc-test target) serially, regardless of --parallel.
if(MSVC)
    add_compile_options(/MP)
endif()

# Local development mode. The `local` preset sets LBNL_LOCAL_SIBLINGS=ON (a cache
# variable, so it cascades into FetchContent sub-builds). When it is on, prefer a
# sibling working copy (../<Name>) of each of THIS project's direct dependencies,
# so the whole dependency graph can be built from local disk during development.
# A dependency whose sibling is not checked out falls back to its declared remote,
# independently. Propagation: a sibling dependency that also honors this flag wires
# up its own siblings in turn. When the flag is off (e.g. the `default` preset),
# nothing here runs and every dependency comes from its remote.
# Note: each pair is "<folder>=<FETCHCONTENT var suffix>"; they differ where a repo's
# folder name (e.g. Windows-CalcStandards, Windows-CalcEngine) is not just the
# uppercased FetchContent_Declare name.
if(LBNL_LOCAL_SIBLINGS)
    foreach(_sibling
            "Windows-CalcStandards=WINDOWSCALCSTANDARDS"
            "OpticalMeasurementParser=OPTICALMEASUREMENTPARSER"
            "THMXParser=THMXPARSER"
            "Windows-CalcEngine=WINDOWSCALC-ENGINE"
            "LibraryFEMTherm=LIBRARYFEMTHERM"
            "googletest=GOOGLETEST")
        string(REPLACE "=" ";" _sibling "${_sibling}")
        list(GET _sibling 0 _sibling_dir)
        list(GET _sibling 1 _sibling_var)
        if(NOT FETCHCONTENT_SOURCE_DIR_${_sibling_var}
           AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../${_sibling_dir}/CMakeLists.txt")
            set(FETCHCONTENT_SOURCE_DIR_${_sibling_var}
                "${CMAKE_CURRENT_SOURCE_DIR}/../${_sibling_dir}")
            message(STATUS "[local] ${_sibling_dir} -> ${FETCHCONTENT_SOURCE_DIR_${_sibling_var}}")
        endif()
    endforeach()
endif()

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

set(WindowsCalcStandards_Branch "v1.2.5")
if(NOT TARGET window_standards)
    FetchContent_Declare(
        WindowsCalcStandards
        GIT_REPOSITORY https://github.com/LBNL-ETA/Windows-CalcStandards.git
        GIT_TAG ${WindowsCalcStandards_Branch}
    )
    FetchContent_MakeAvailable(WindowsCalcStandards)
endif()

set(OpticalMeasurementParser_Branch "v2.3.10")
if(NOT TARGET OpticalMeasurementParser)
    FetchContent_Declare(
        OpticalMeasurementParser
        GIT_REPOSITORY https://github.com/LBNL-ETA/OpticalMeasurementParser.git
        GIT_TAG ${OpticalMeasurementParser_Branch}
    )
    FetchContent_MakeAvailable(OpticalMeasurementParser)
endif()

set(THMXParser_Branch "v1.1.7")
if(NOT TARGET THMXParser)
    FetchContent_Declare(
        THMXParser
        GIT_REPOSITORY https://github.com/LBNL-ETA/THMXParser.git
        GIT_TAG ${THMXParser_Branch}
    )
    FetchContent_MakeAvailable(THMXParser)
endif()

set(WindowsCalcEngine_Branch "Version_1.0.74")
if(NOT TARGET Windows-CalcEngine)
    FetchContent_Declare(
        WindowsCalc-Engine
        GIT_REPOSITORY https://github.com/LBNL-ETA/Windows-CalcEngine.git
        GIT_TAG ${WindowsCalcEngine_Branch}
    )
    FetchContent_MakeAvailable(WindowsCalc-Engine)
endif()

set(LibraryFEMTherm_Branch "Version_0.22.2")
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
