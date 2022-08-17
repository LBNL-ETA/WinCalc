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

class TestNFRC102AsBSDF : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path layer_1_path(test_dir);
        layer_1_path /= "products";
        layer_1_path /= "CLEAR_3.json";

        std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
        auto layer_1 = OpticsParser::parseJSONFile(layer_1_path.string());
        products.push_back(layer_1);

        std::vector<Engine_Gap_Info> gaps;

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

#if 0
TEST_F(TestNFRC102AsBSDF, Test_Thermal)
{
    test_thermal_results(
      "NFRC_102_BSDF", "thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results(
      "NFRC_102_BSDF", "thermal_SHGC_Environment", glazing_system_shgc, update_results);

}
#endif

TEST_F(TestNFRC102AsBSDF, Test_Optical)
{
    test_optical_results("NFRC_102_BSDF", glazing_system_u, update_results);
}
