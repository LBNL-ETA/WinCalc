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

class TestBSDFThermalUserDefinedOpenness : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path clear_3_path(test_dir);
        clear_3_path /= "products";
        clear_3_path /= "CLEAR_3.json";

        std::filesystem::path shade_path(test_dir);
        shade_path /= "products";
        shade_path /= "2011-SA1.XML";

        std::vector<std::variant<OpticsParser::ProductData, wincalc::Product_Data_Optical_Thermal>>
          products;

        auto clear_3 = OpticsParser::parseJSONFile(clear_3_path.string());
        auto shade = OpticsParser::parseBSDFXMLFile(shade_path.string());
        auto converted_shade = wincalc::convert_to_solid_layer(shade);
        converted_shade.thermal_data->opening_top = 0.01;
        converted_shade.thermal_data->opening_bottom = 0.01;
        converted_shade.thermal_data->opening_left = 0;
        converted_shade.thermal_data->opening_right = 0;
        converted_shade.thermal_data->opening_front = 0.222;
		
	std::dynamic_pointer_cast<Product_Data_Dual_Band_Optical_BSDF>(converted_shade.optical_data)
            ->user_defined_effective_values = true;
        converted_shade.optical_data->permeability_factor = 0.0789;
        converted_shade.optical_data->thickness_meters = 0.0123;

        products.push_back(clear_3);
        products.push_back(converted_shade);

        double gap_thickness = 0.0127;
        double gap_pressure = Gases::DefaultPressure;
        auto air_gap = std::make_shared<Tarcog::ISO15099::CIGUGapLayer>(
			gap_thickness, gap_pressure, Gases::CGas({{1.0, Gases::GasDef::Air}}));
        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> gaps;
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

TEST_F(TestBSDFThermalUserDefinedOpenness, Test_Thermal)
{
    test_thermal_results("thermal_user_defined_openings_xml_shade_10mm_top_and_bottom",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("thermal_user_defined_openings_xml_shade_10mm_top_and_bottom",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}

TEST_F(TestBSDFThermalUserDefinedOpenness, Test_Optical)
{
    test_optical_results(
      "thermal_openings_xml_shade_10mm_top_and_bottom", glazing_system_u, update_results);
}
