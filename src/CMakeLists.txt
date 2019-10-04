set( CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON )


add_library( ${LIB_NAME}   
		optical_calcs.h
		optical_calcs.cpp
		optical_results.h
		create_wce_objects.h
		create_wce_objects.cpp
		glazing_system.h
		glazing_system.cpp
		gap.h
		gap.cpp
		optical_calc_params.h
		environmental_conditions.h
		environmental_conditions.cpp
		product_data.h
		product_data.cpp
		convert_optics_parser.h
		convert_optics_parser.cpp
		util.h
		util.cpp)



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

target_compile_features(${LIB_NAME} PUBLIC cxx_std_17)
target_link_libraries(${LIB_NAME} PUBLIC window_standards OpticalMeasurementParser Windows-CalcEngine)




