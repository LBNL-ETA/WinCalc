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

class TestNFRC102SingleBandHomogeneousDiffusingShade : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path clear_3_path(test_dir);
        clear_3_path /= "products";
        clear_3_path /= "CLEAR_3.json";

        OpticsParser::Parser parser;
        auto clear_3 = parser.parseJSONFile(clear_3_path.string());


        std::shared_ptr<Product_Data_Optical> shade_optical_data =
          std::make_shared<Product_Data_Dual_Band_Optical_Hemispheric>(
            0.1, 0.1, 0.7, 0.7, 0.1, 0.1, 0.7, 0.7, 0.0006, 0.0, 0.0, 0.9, 0.9);

        std::shared_ptr<Product_Data_Thermal> shade_thermal_data =
          std::make_shared<Product_Data_Thermal>(160, 0.0006);

        auto shade_layer = wincalc::create_homogeneous_diffusing_shade(shade_optical_data, shade_thermal_data);

        std::vector<std::variant<OpticsParser::ProductData, Product_Data_Optical_Thermal>> products;

        products.push_back(clear_3);
        products.push_back(shade_layer);

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

TEST_F(TestNFRC102SingleBandHomogeneousDiffusingShade, Test_Thermal_0_Permeability)
{
    test_thermal_results("NFRC_102_Single_Band_Homogeneous_Diffusing_Shade\\0_Permeability", "thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results("NFRC_102_Single_Band_Homogeneous_Diffusing_Shade\\0_Permeability",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}


TEST_F(TestNFRC102SingleBandHomogeneousDiffusingShade, Test_Optical_0_Permeability)
{
    test_optical_results("NFRC_102_Single_Band_Homogeneous_Diffusing_Shade\\0_Permeability", glazing_system_u, update_results);
}

TEST_F(TestNFRC102SingleBandHomogeneousDiffusingShade, Test_Thermal_15_Permeability)
{
    auto layers = glazing_system_u->solid_layers();
    layers[1].thermal_data->effective_front_thermal_openness_area = 0.15;
    glazing_system_u->solid_layers(layers);
    glazing_system_shgc->solid_layers(layers);
    test_thermal_results("NFRC_102_Single_Band_Homogeneous_Diffusing_Shade\\0.15_Permeability",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("NFRC_102_Single_Band_Homogeneous_Diffusing_Shade\\0.15_Permeability",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}


TEST_F(TestNFRC102SingleBandHomogeneousDiffusingShade, Test_Optical_15_Permeability)
{
    auto layers = glazing_system_u->solid_layers();
    layers[1].thermal_data->effective_front_thermal_openness_area = 0.15;
    glazing_system_u->solid_layers(layers);\
    test_optical_results(
      "NFRC_102_Single_Band_Homogeneous_Diffusing_Shade\\0.15_Permeability", glazing_system_u, update_results);
}

TEST_F(TestNFRC102SingleBandHomogeneousDiffusingShade, Test_Thermal_75_Permeability)
{
    auto layers = glazing_system_u->solid_layers();
    layers[1].thermal_data->effective_front_thermal_openness_area = 0.75;
    glazing_system_u->solid_layers(layers);
    glazing_system_shgc->solid_layers(layers);
    test_thermal_results("NFRC_102_Single_Band_Homogeneous_Diffusing_Shade\\0.75_Permeability",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("NFRC_102_Single_Band_Homogeneous_Diffusing_Shade\\0.75_Permeability",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}


TEST_F(TestNFRC102SingleBandHomogeneousDiffusingShade, Test_Optical_75_Permeability)
{
    auto layers = glazing_system_u->solid_layers();
    layers[1].thermal_data->effective_front_thermal_openness_area = 0.75;
    glazing_system_u->solid_layers(layers);
    test_optical_results(
      "NFRC_102_Single_Band_Homogeneous_Diffusing_Shade\\0.75_Permeability", glazing_system_u, update_results);
}