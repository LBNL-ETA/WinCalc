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

class Test_1_layer_coated_nfrc_9924 : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;
    std::shared_ptr<Glazing_System> glazing_system_u_quarter_basis;
    std::shared_ptr<Glazing_System> glazing_system_u_full_basis;

    virtual void SetUp()
    {
        std::filesystem::path product_path(test_dir);
        product_path /= "products";
        product_path /= "LOW-E_6.LOF";

        std::vector<OpticsParser::ProductData> products;
        OpticsParser::Parser parser;
        auto product = parser.parseFile(product_path.string());
        products.push_back(product);

        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> gaps;

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        auto bsdf_hemisphere =
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);

        auto bsdf_hemisphere_full =
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Full);

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments());
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_shgc_environments());
        glazing_system_u_quarter_basis = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments(), bsdf_hemisphere);
        glazing_system_u_full_basis = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments(), bsdf_hemisphere_full);
    }
};

TEST_F(Test_1_layer_coated_nfrc_9924, Test_Thermal)
{
    test_thermal_results("1_layer/coated_nfrc_9924/no_basis", "thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results(
      "1_layer/coated_nfrc_9924/no_basis", "thermal_SHGC_Environment", glazing_system_shgc, update_results);
}


TEST_F(Test_1_layer_coated_nfrc_9924, Test_Optical)
{
    std::vector<std::pair<double, double>> incident_angles{{0, 0},
                                                           {10, 0},
                                                           {20, 0},
                                                           {30, 0},
                                                           {40, 0},
                                                           {50, 0},
                                                           {60, 0},
                                                           {70, 0},
                                                           {80, 0},
                                                           {15.0, 270.0}};
    test_optical_results("1_layer/coated_nfrc_9924/no_basis", glazing_system_u, update_results, incident_angles);
    test_optical_results("1_layer/coated_nfrc_9924/quarter_basis", glazing_system_u_quarter_basis, update_results, incident_angles);
    test_optical_results("1_layer/coated_nfrc_9924/full_basis", glazing_system_u_full_basis, update_results, incident_angles);
}
