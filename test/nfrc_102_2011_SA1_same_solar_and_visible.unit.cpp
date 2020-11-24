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

class TestNFRC1022011SA1 : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path clear_3_path(test_dir);
        clear_3_path /= "products";
        clear_3_path /= "CLEAR_3.json";

        std::filesystem::path bsdf_path(test_dir);
        bsdf_path /= "products";
        bsdf_path /=
          "2011-SA1_same_solar_and_visible_only_normal.XML";   //"2011-SA1_same_solar_and_visible.XML";

        std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
        auto clear_3 = OpticsParser::parseJSONFile(clear_3_path.string());
        products.push_back(clear_3);

        auto bsdf = OpticsParser::parseBSDFXMLFile(bsdf_path.string());
        products.push_back(bsdf);

        Engine_Gap_Info air_gap(Gases::GasDef::Air, 0.0127);
        std::vector<Engine_Gap_Info> gaps;
        gaps.push_back(air_gap);

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        auto bsdf_hemisphere =
          SingleLayerOptics::CBSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Small);

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, nfrc_u_environments(), bsdf_hemisphere);
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, nfrc_shgc_environments(), bsdf_hemisphere);
    }
};

TEST_F(TestNFRC1022011SA1, Test_Thermal)
{
	test_thermal_results("NFRC_102_2011_SA1_XML_same_solar_and_visible/thermal_U_Environment", glazing_system_u, update_results);
	test_thermal_results("NFRC_102_2011_SA1_XML_same_solar_and_visible/thermal_SHGC_Environment", glazing_system_shgc, update_results);
}

TEST_F(TestNFRC1022011SA1, Test_Optical)
{
    test_optical_results(
      "NFRC_102_2011_SA1_XML_same_solar_and_visible", glazing_system_u, update_results);
}