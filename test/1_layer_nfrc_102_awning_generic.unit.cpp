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

class Test_1_layer_nfrc_102_awning_generic : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path clear_3_path(test_dir);
        clear_3_path /= "products";
        clear_3_path /= "CLEAR_3.json";

        std::vector<OpticsParser::ProductData> products;
        OpticsParser::Parser parser;
        auto clear_3 = parser.parseJSONFile(clear_3_path.string());
        products.push_back(clear_3);

        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> gaps;

        std::filesystem::path awning_path(test_dir);
        awning_path /= "products";
        awning_path /= "generic_awning_condensed_wavelengths.xml";
        auto awning = OpticsParser::parseBSDFXMLFile(awning_path.string());

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        auto bsdf_hemisphere =
            SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 0.885700, 0.885700, 90, nfrc_u_environments(), bsdf_hemisphere, Spectal_Data_Wavelength_Range_Method::FULL, 5, 10, awning);
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 0.885700, 0.885700, 90, nfrc_shgc_environments(), bsdf_hemisphere, Spectal_Data_Wavelength_Range_Method::FULL, 5, 10, awning);
    }
};

TEST_F(Test_1_layer_nfrc_102_awning_generic, Test_Thermal)
{
    test_thermal_results("1_layer/nfrc_102_awning_generic/default", "thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results(
      "1_layer/nfrc_102_awning_generic/default", "thermal_SHGC_Environment", glazing_system_shgc, update_results);
}

#if 0
TEST_F(Test_1_layer_nfrc_102_awning_generic, Test_Thermal_Conductivity_0_2)
{
    glazing_system_u->set_solid_layer_conductivites({0.2});
    glazing_system_shgc->set_solid_layer_conductivites({0.2});
    test_thermal_results("1_layer/nfrc_102_awning_generic/conductivity_0.2", "thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results(
      "1_layer/nfrc_102_awning_generic/conductivity_0.2", "thermal_SHGC_Environment", glazing_system_shgc, update_results);
}

TEST_F(Test_1_layer_nfrc_102_awning_generic, Test_Thermal_Conductivity_0_5)
{
    glazing_system_u->set_solid_layer_conductivity(0, 0.5);
    glazing_system_shgc->set_solid_layer_conductivity(0, 0.5);
    test_thermal_results("1_layer/nfrc_102_awning_generic/conductivity_0.5", "thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results(
      "1_layer/nfrc_102_awning_generic/conductivity_0.5", "thermal_SHGC_Environment", glazing_system_shgc, update_results);
}
#endif

TEST_F(Test_1_layer_nfrc_102_awning_generic, Test_Optical)
{
    test_optical_results("1_layer/nfrc_102_awning_generic/default", glazing_system_u, update_results);
}
