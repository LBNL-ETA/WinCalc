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

class TestWINDOWIssue1216 : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path layer_1_path(test_dir);
        layer_1_path /= "products";
        layer_1_path /= "igsdb_5406.json";

        std::filesystem::path bsdf_path(test_dir);
        bsdf_path /= "products";
        bsdf_path /= "2011-SA45.XML";

        std::filesystem::path layer_3_path(test_dir);
        layer_3_path /= "products";
        layer_3_path /= "igsdb_9817.json";

        std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
        auto layer_1 = OpticsParser::parseJSONFile(layer_1_path.string());
        // products.push_back(layer_1);

        auto bsdf = OpticsParser::parseBSDFXMLFile(bsdf_path.string());
        // products.push_back(bsdf);

        auto layer_3 = OpticsParser::parseJSONFile(layer_3_path.string());
        products.push_back(layer_3);
        // products.push_back(layer_3);

        Engine_Gap_Info air_gap(Gases::GasDef::Air, 0.0127);
        // std::vector<Engine_Gap_Info> gaps{air_gap, air_gap};
        std::vector<Engine_Gap_Info> gaps;

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        auto bsdf_hemisphere =
          SingleLayerOptics::CBSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Full);

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments(), bsdf_hemisphere);
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_shgc_environments(), bsdf_hemisphere);

        // glazing_system_u->flip_layer(2, true);
        // glazing_system_shgc->flip_layer(2, true);
    }
};

#if 0
TEST_F(TestWINDOWIssue1216, Test_Thermal)
{
    test_thermal_results(
      "WINDOW_Issue_1216", "thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results(
      "WINDOW_Issue_1216", "thermal_SHGC_Environment", glazing_system_shgc, update_results);
}
#endif

TEST_F(TestWINDOWIssue1216, Test_Optical)
{
    test_optical_results("WINDOW_Issue_1216", glazing_system_u, update_results);
}
