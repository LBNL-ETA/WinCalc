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

class Test_1_layer_nfrc_102_dir_dif : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path clear_3_path(test_dir);
        clear_3_path /= "products";
        clear_3_path /= "CLEAR_3_dir_dif.DAT";

        std::vector<OpticsParser::ProductData> products;
        OpticsParser::Parser parser;
        auto clear_3 = parser.parseFile(clear_3_path.string());
        products.push_back(clear_3);

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

TEST_F(Test_1_layer_nfrc_102_dir_dif, Test_Thermal)
{
    test_thermal_results("1_layer/nfrc_102/dir-dif", "thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results(
      "1_layer/nfrc_102/dir-dif", "thermal_SHGC_Environment", glazing_system_shgc, update_results);
}

TEST_F(Test_1_layer_nfrc_102_dir_dif, Test_Optical)
{
    test_optical_results("1_layer/nfrc_102/dir-dif", glazing_system_u, update_results);
}

TEST_F(Test_1_layer_nfrc_102_dir_dif, Test_NoBSDFHemisphere)
{
    std::filesystem::path clear_3_path(test_dir);
    clear_3_path /= "products";
    clear_3_path /= "CLEAR_3_dir_dif.DAT";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::Parser parser;
    auto clear_3 = parser.parseFile(clear_3_path.string());
    products.push_back(clear_3);

    std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());

    auto glazing_system = std::make_shared<Glazing_System>(
        standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments());

    EXPECT_THROW(glazing_system->optical_method_results("SOLAR"), std::runtime_error);
}