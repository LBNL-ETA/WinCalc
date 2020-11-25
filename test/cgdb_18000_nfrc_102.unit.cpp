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

class Test_CGDB_18000_NFRC_102 : public testing::Test
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
        shade_path /= "cgdb_18000.json";

        std::vector<std::shared_ptr<OpticsParser::ProductData>> parsed_products;
        OpticsParser::Parser parser;
        auto clear_3 = parser.parseJSONFile(clear_3_path.string());
        auto shade = parser.parseJSONFile(shade_path.string());
        parsed_products.push_back(shade);
        parsed_products.push_back(clear_3);

        auto converted_products = convert_to_solid_layers(parsed_products);

        Engine_Gap_Info air_gap(Gases::GasDef::Air, 0.0127);
        std::vector<Engine_Gap_Info> gaps;
        gaps.push_back(air_gap);

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        auto bsdf_hemisphere =
          SingleLayerOptics::CBSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, converted_products, gaps, 1.0, 1.0, nfrc_u_environments(), bsdf_hemisphere);
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, converted_products, gaps, 1.0, 1.0, nfrc_shgc_environments(), bsdf_hemisphere);
    }
};

TEST_F(Test_CGDB_18000_NFRC_102, Test_Thermal)
{
	test_thermal_results("CGDB_18000_NFRC_102/thermal_U_Environment", glazing_system_u, update_results);
	test_thermal_results("CGDB_18000_NFRC_102/thermal_SHGC_Environment", glazing_system_shgc, update_results);
}

TEST_F(Test_CGDB_18000_NFRC_102, Test_Optical)
{
	test_optical_results("CGDB_18000_NFRC_102", glazing_system_u, update_results);
}

