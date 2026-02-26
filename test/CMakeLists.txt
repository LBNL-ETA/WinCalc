include_directories("${PROJECT_SOURCE_DIR}")

set(PROJECT_TEST_NAME ${LIB_NAME}-test)

# GoogleTest
if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

include(FetchContent)

if(NOT TARGET gtest)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

add_executable(${PROJECT_TEST_NAME}
		1_layer_cgdb_31017.unit.cpp
		1_layer_cgdb_31018.unit.cpp
		1_layer_nfrc_102_awning_generic.unit.cpp
		#1_layer_nfrc_102_awning_2011-SA1.unit.cpp
		1_layer_nfrc_102_WCE.unit.cpp
		1_layer_applied_film_nfrc_9223.unit.cpp
		1_layer_cellular_shade_CS03_genBSDF.unit.cpp
		1_layer_coated_nfrc_6400.cpp
		1_layer_coated_nfrc_6046.unit.cpp
		1_layer_coated_nfrc_18100.unit.cpp
		1_layer_coated_nfrc_20748.unit.cpp
 		1_layer_coated_nfrc_21467.unit.cpp
		1_layer_laminate_optics_nanometers.unit.cpp
		1_layer_louvered_shade_bsdf_xml.unit.cpp
		1_layer_nfrc_102.unit.cpp
		1_layer_nfrc_102_as_bsdf.unit.cpp
		1_layer_nfrc_102_as_dir_dif.unit.cpp
		1_layer_perforated_screen_cgdb_18000.unit.cpp
		1_layer_pleated_shade_genBSDF_cgdb_24040.unit.cpp
		1_layer_pv_generic.unit.cpp
		1_layer_roller_shade_bsdf_xml_cgdb_14060.unit.cpp
		1_layer_roller_shade_bsdf_xml_cgdb_46016.unit.cpp
		1_layer_roman_shade_genBSDF.unit.cpp
		1_layer_venetian_cgdb_24000.unit.cpp
		1_layer_venetian_cgdb_24001.unit.cpp
		1_layer_venetian_cgdb_24002.unit.cpp
		1_layer_woven_shade.unit.cpp
		2_layers_laminate_nfrc_18000_air_NFRC_102_quarter_basis.unit.cpp
		2_layers_nfrc_102_air_bsdf_xml_thermal_openness_10_mm_top_and_bottom.unit.cpp
		2_layers_nfrc_102_air_bsdf_xml_thermal_openness_10_mm_top_and_bottom_0.222_effective_front_thermal_openness_area_0.0789_permeability_factor.unit.cpp
		2_layers_nfrc_102_air_homogeneous_diffusing_shade_pella_white_venetian_blind_slat.unit.cpp
		2_layers_nfrc_102_air_homogeneous_diffusing_shade_single_band.unit.cpp
		2_layers_nfrc_102_air_nfrc_102_deflection.unit.cpp
		2_layers_nfrc_102_air_nfrc_102_deflection_density.unit.cpp
		2_layers_nfrc_102_air_nfrc_102_deflection_environment.unit.cpp
		2_layers_nfrc_102_air_nfrc_102_deflection_load.unit.cpp
		2_layers_nfrc_102_air_nfrc_102_deflection_tilt.unit.cpp
		2_layers_nfrc_102_air_nfrc_102_deflection_youngs_modulus.unit.cpp
		2_layers_nfrc_102_air_perfectly_diffusing_shade_pella_white_venetian_blind_slat.unit.cpp
		2_layers_nfrc_102_as_awning_nfrc_102.unit.cpp
		2_layers_nfrc_103_air_nfrc_5487.unit.cpp
		2_layers_venetian_cgdb_3000_uniform_diffuse_air_nfrc_102.unit.cpp
		2_layers_vertical_venetian_cgdb_3000_uniform_diffuse_air_nfrc_102.unit.cpp
		2_layers_nfrc_102_air_nfrc_102_forced_ventilation.unit.cpp
		2_layers_nfrc_102_vacuum_nfrc_102.unit.cpp
		2_layers_vertical_venetian_pella_white_venetian_blind_slat_user_geometry_air_nfrc_102.unit.cpp
		2_layers_venetian_pella_white_venetian_blind_slat_user_geometry_air_nfrc_102.unit.cpp
		2_layers_perforated_screen_pella_white_venetian_blind_slat_user_geometry_air_nfrc_102.unit.cpp
		#double_layer_perfect_clear_exterior_interior_xml_with_thermal_openings_10mm_all_sides.unit.cpp
		#double_layer_interior_xml_with_thermal_openings_10mm_all_sides.unit.cpp
		2_layers_nfrc_102_air_nfrc_102.unit.cpp
 		2_layers_low_e_nfrc_2191_air_nfrc_102.unit.cpp
 		2_layers_venetian_cgdb_3001_air_nfrc_102.unit.cpp
 		2_layers_woven_pella_white_venetian_blind_slat_user_geometry_air_nfrc_102.unit.cpp
		3_layers_nfrc_102_air_nfrc_102_air_0.0381_bsdf_xml_2011SA1.unit.cpp
		3_layers_nfrc_102_air_nfrc_102_air_0.0381_bsdf_xml_2011SA1_same_solar_and_visible_only_normal.unit.cpp
 		cma.unit.cpp
		3_layers_nfrc_102_air_nfrc_102_air_nfrc_102_deflection_triple_clear.unit.cpp
		4_layers_coated_nfrc_9962_argon_air_nfrc_102_argon_air_coated_nfrc_11561_air_bsdf_shade_all_sides_0.01_opening.unit.cpp
		WINDOW_issue_1216.unit.cpp
		load_frame_data.unit.cpp
		nfrc_9023.unit.cpp
		main.cpp
		paths.h
		util.h
		util.cpp)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)

target_compile_features(${PROJECT_TEST_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_TEST_NAME} gmock_main ${LIB_NAME})
