set( CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON )


add_library( ${LIB_NAME}   
		optical_calcs.h
		optical_calcs.cpp  
		thermal_calcs.h
		thermal_calcs.cpp       
		thermal_calcs_disk.h
		thermal_calcs_disk.cpp
		create_wce_objects.h
		create_wce_objects.cpp  
		results.h
		results.cpp)



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


target_link_libraries( ${LIB_NAME} PRIVATE window_standards OpticalMeasurementParser Windows-CalcEngine)


