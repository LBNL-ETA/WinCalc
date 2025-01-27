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

class Test_2_layers_venetian_cgdb_3001_air_nfrc_102 : public testing::Test
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
        shade_path /= "cgdb_3001.json";

        std::vector<std::variant<OpticsParser::ProductData,
                                 wincalc::Product_Data_Optical_Thermal>>
          products;
        OpticsParser::Parser parser;
        auto clear_3 = parser.parseJSONFile(clear_3_path.string());
        auto shade = parser.parseJSONFile(shade_path.string());

        products.push_back(shade);
        products.push_back(clear_3);


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
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments(), bsdf_hemisphere);
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_shgc_environments(), bsdf_hemisphere);
    }
};

TEST_F(Test_2_layers_venetian_cgdb_3001_air_nfrc_102, Test_Thermal)
{
    test_thermal_results(
      "2_layers/venetian_cgdb_3001_air_nfrc_102/quarter_basis", "thermal_SHGC_Environment", glazing_system_shgc, update_results);
    test_thermal_results(
      "2_layers/venetian_cgdb_3001_air_nfrc_102/quarter_basis", "thermal_U_Environment", glazing_system_u, update_results);
}

TEST_F(Test_2_layers_venetian_cgdb_3001_air_nfrc_102, Test_Optical)
{
    test_optical_results("2_layers/venetian_cgdb_3001_air_nfrc_102/quarter_basis", glazing_system_u, update_results);
}

TEST_F(Test_2_layers_venetian_cgdb_3001_air_nfrc_102, Test_Thermal_Flipped_shade)
{
    glazing_system_u->flip_layer(0, true);
    glazing_system_shgc->flip_layer(0, true);
    test_thermal_results("2_layers/venetian_cgdb_3001_air_nfrc_102/venetian_flipped/quarter_basis",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("2_layers/venetian_cgdb_3001_air_nfrc_102/venetian_flipped/quarter_basis",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}

TEST_F(Test_2_layers_venetian_cgdb_3001_air_nfrc_102, Test_Optical_Flipped_shade)
{
    glazing_system_shgc->flip_layer(0, true);
    test_optical_results("2_layers/venetian_cgdb_3001_air_nfrc_102/venetian_flipped/quarter_basis", glazing_system_u, update_results);
}
