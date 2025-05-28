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

class Test_2_layers_nfrc_103_air_nfrc_5487 : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path layer_1_path(test_dir);
        layer_1_path /= "products";
        layer_1_path /= "igsdb_product_10693.json";

        std::filesystem::path layer_2_path(test_dir);
        layer_2_path /= "products";
        layer_2_path /= "igsdb_product_364.json";

        std::vector<OpticsParser::ProductData> products;
        OpticsParser::Parser parser;
        auto layer_1 = parser.parseJSONFile(layer_1_path.string());
        auto layer_2 = parser.parseJSONFile(layer_2_path.string());
        products.push_back(layer_1);
        products.push_back(layer_2);

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

#if 0
TEST_F(Test_2_layers_nfrc_103_air_nfrc_5487, Test_Thermal)
{
    test_thermal_results("2_layers/nfrc_103_air_nfrc_5487/full_basis", "thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results(
      "2_layers/nfrc_103_air_nfrc_5487/full_basis", "thermal_SHGC_Environment", glazing_system_shgc, update_results);
}
#endif

TEST_F(Test_2_layers_nfrc_103_air_nfrc_5487, Test_Optical)
{
    test_optical_results("2_layers/nfrc_103_air_nfrc_5487/full_basis", glazing_system_u, update_results);
}
