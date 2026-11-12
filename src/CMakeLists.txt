set( CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON )


add_library( ${LIB_NAME}   
		optical_calcs.h
		optical_calcs.cpp
		optical_results.h
		create_wce_objects.h
		create_wce_objects.cpp
		glazing_system.h
		glazing_system.cpp
		layer_cache.h
		optical_calc_params.h
		environmental_conditions.h
		environmental_conditions.cpp
		product_data.h
		product_data.cpp
		convert_optics_parser.h
		convert_optics_parser.cpp
		util.h
		util.cpp
		cma.h
		cma.cpp
		thermal_ir.h
		thermal_ir.cpp
		thmz.h
		thmz.cpp
		deflection_results.h
		shade_factories.h
		shade_factories.cpp
		report/thermal_report/thermal_report.h
		report/thermal_report/thermal_report.cpp
		report/thermal_report/thermal_report_serializers.hxx
		report/thermal_report/thermal_report_io.cpp)



if(NOT "${fPIC}")
  set(fPIC ON)
endif()

if(fPIC)
  set_property(TARGET ${LIB_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

if ( CMAKE_COMPILER_IS_GNUCC )
    target_compile_options(${LIB_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
if ( MSVC )
    target_compile_options(${LIB_NAME} PRIVATE /W4 /WX)
endif()

target_compile_features(${LIB_NAME} PUBLIC cxx_std_20)
target_link_libraries(${LIB_NAME} PUBLIC window_standards OpticalMeasurementParser THMXParser Windows-CalcEngine LibraryFEMTHERM)

# The report serializers (src/report) write XML and JSON through FileParse. The FileParse target
# already exists in this build: LibraryFEMTherm and BSDFXMLParser bring it in. Its headers are
# included as <fileParse/...>, so the include root is the parent of the target's source folder.
get_target_property(FileParse_Source_Dir FileParse SOURCE_DIR)
target_link_libraries(${LIB_NAME} PRIVATE FileParse)
target_include_directories(${LIB_NAME} SYSTEM PRIVATE "${FileParse_Source_Dir}/..")




