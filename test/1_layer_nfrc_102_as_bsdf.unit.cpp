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

class Test_1_layer_nfrc_102_as_bsdf : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path layer_1_path(test_dir);
        layer_1_path /= "products";
        layer_1_path /= "CLEAR_3.json";

        std::vector<OpticsParser::ProductData> products;
        auto layer_1 = OpticsParser::parseJSONFile(layer_1_path.string());
        products.push_back(layer_1);

        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> gaps;

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

TEST_F(Test_1_layer_nfrc_102_as_bsdf, Test_Thermal)
{
    test_thermal_results(
      "1_layer/nfrc_102/full_basis", "thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results(
      "1_layer/nfrc_102/full_basis", "thermal_SHGC_Environment", glazing_system_shgc, update_results);

}

TEST_F(Test_1_layer_nfrc_102_as_bsdf, Test_Optical)
{
    test_optical_results("1_layer/nfrc_102/full_basis", glazing_system_u, update_results);
}
