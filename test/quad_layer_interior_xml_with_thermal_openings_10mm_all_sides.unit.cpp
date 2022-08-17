#include <memory>
#include <gtest/gtest.h>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "wincalc/wincalc.h"
#include "optical_calcs.h"
#include "util.h"
#include "convert_optics_parser.h"
#include "paths.h"


using namespace wincalc;
using namespace window_standards;

class TestQuadLayerInteriorXMLWithThermalOpenings : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path clear_3_path(test_dir);
        clear_3_path /= "products";
        clear_3_path /= "CLEAR_3.json";

		std::filesystem::path SolarEPlusArBl6_path(test_dir);
		SolarEPlusArBl6_path /= "products";
		SolarEPlusArBl6_path /= "SolarEPlusArBl6.json";

		std::filesystem::path CG_Prem2_5_path(test_dir);
		CG_Prem2_5_path /= "products";
		CG_Prem2_5_path /= "CG_Prem2_5.json";

        std::filesystem::path shade_path(test_dir);
        shade_path /= "products";
        shade_path /= "shade_full.xml";

        std::vector<std::variant<std::shared_ptr<OpticsParser::ProductData>,
                                 wincalc::Product_Data_Optical_Thermal>>
          products;

        auto clear_3 = OpticsParser::parseJSONFile(clear_3_path.string());
        auto shade = OpticsParser::parseBSDFXMLFile(shade_path.string());
        auto converted_shade = wincalc::convert_to_solid_layer(shade);
        converted_shade.thermal_data->opening_top = 0.01;
        converted_shade.thermal_data->opening_bottom = 0.01;
        converted_shade.thermal_data->opening_left = 0.01;
        converted_shade.thermal_data->opening_right = 0.01;

		auto SolarEPlusArBl6 = OpticsParser::parseJSONFile(SolarEPlusArBl6_path.string());
		auto CG_Prem2_5 = OpticsParser::parseJSONFile(CG_Prem2_5_path.string());

		products.push_back(SolarEPlusArBl6);
        products.push_back(clear_3);
		products.push_back(CG_Prem2_5);
        products.push_back(converted_shade);

        Engine_Gap_Info air_gap(Gases::GasDef::Air, 0.0127);
        Predefined_Gas_Mixture_Component air_10_percent{Gases::GasDef::Air, .1};
        Predefined_Gas_Mixture_Component argon_90_percent{Gases::GasDef::Argon, .9};
		std::vector<Predefined_Gas_Mixture_Component> gas_mix_components;
		gas_mix_components.push_back(air_10_percent);
		gas_mix_components.push_back(argon_90_percent);
        Engine_Gap_Info argon_mix(gas_mix_components, .0127);
        std::vector<Engine_Gap_Info> gaps;
        gaps.push_back(argon_mix);
        gaps.push_back(argon_mix);
        gaps.push_back(air_gap);

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        auto bsdf_hemisphere =
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Full);

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments(), bsdf_hemisphere);
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_shgc_environments(), bsdf_hemisphere);
    }
};

TEST_F(TestQuadLayerInteriorXMLWithThermalOpenings, Test_Thermal)
{
    test_thermal_results(
      "quad_layer_interior_xml_with_thermal_openings_10mm_all_sides", "thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results(
      "quad_layer_interior_xml_with_thermal_openings_10mm_all_sides", "thermal_SHGC_Environment", glazing_system_shgc, update_results);
}

TEST_F(TestQuadLayerInteriorXMLWithThermalOpenings, Test_Optical)
{
    test_optical_results("quad_layer_interior_xml_with_thermal_openings_10mm_all_sides", glazing_system_u, update_results);
}
