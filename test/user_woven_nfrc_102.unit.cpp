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

class TestUserWovenNFRC102 : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path clear_3_path(test_dir);
        clear_3_path /= "products";
        clear_3_path /= "CLEAR_3.json";

        // Use the same material as in the venetian test above for consistancy with WCE tests
        std::filesystem::path venetian_material_path(test_dir);
        venetian_material_path /= "products";
        venetian_material_path /= "igsdb_12852.json";

        std::vector<std::shared_ptr<OpticsParser::ProductData>> parsed_products;
        OpticsParser::Parser parser;
        auto clear_3 = parser.parseJSONFile(clear_3_path.string());
        auto shade_material = parser.parseJSONFile(venetian_material_path.string());
        auto shade_geometry = std::make_shared<OpticsParser::WovenGeometry>(0.002, 0.003, 0.002);
        auto shade_composition_info = std::shared_ptr<OpticsParser::CompositionInformation>(
          new OpticsParser::CompositionInformation{shade_material, shade_geometry});
        OpticsParser::ProductData shade_layer_info(
          "User Woven Shade", "shading", "User Manufacturer");
        auto woven_shade = std::make_shared<OpticsParser::ComposedProductData>(
          shade_layer_info, shade_composition_info);

        parsed_products.push_back(woven_shade);
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
          standard, converted_products, gaps, 1.0, 1.0, 90, nfrc_u_environments(), bsdf_hemisphere);
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, converted_products, gaps, 1.0, 1.0, 90, nfrc_shgc_environments(), bsdf_hemisphere);
    }
};

TEST_F(TestUserWovenNFRC102, Test_Thermal)
{
	test_thermal_results("User_Woven_NFRC_102/thermal_U_Environment", glazing_system_u, update_results);
	test_thermal_results("User_Woven_NFRC_102/thermal_SHGC_Environment", glazing_system_shgc, update_results);
}

TEST_F(TestUserWovenNFRC102, Test_Optical)
{
	test_optical_results("User_Woven_NFRC_102", glazing_system_u, update_results);
}
