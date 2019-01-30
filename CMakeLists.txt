cmake_minimum_required(VERSION 2.8.9)

message("Start WinCalc CMakeLists.txt")

set(target_name WinCalc)

include(CMakeLists-Windows-Standards.txt)
include(CMakeLists-OpticalMeasurementParser.txt)
include(CMakeLists-Windows-CalcEngine.txt)
include(generic_cmake_build.txt)

target_link_libraries( ${target_name} PRIVATE Windows-Standards OpticalMeasurementParser Windows-CalcEngine)
