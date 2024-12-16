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

class Test_3_layers_nfrc_102_air_nfrc_102_2011SA1 : public testing::Test
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

        std::vector<OpticsParser::ProductData> products;
        auto clear_3 = OpticsParser::parseJSONFile(clear_3_path.string());
        products.push_back(clear_3);

        auto bsdf = OpticsParser::parseBSDFXMLFile(bsdf_path.string());
        products.push_back(bsdf);

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
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Small);

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments(), bsdf_hemisphere);
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_shgc_environments(), bsdf_hemisphere);
    }
};

TEST_F(Test_3_layers_nfrc_102_air_nfrc_102_2011SA1, Test_Thermal)
{
    test_thermal_results("3_layers/nfrc_102_air_nfrc_102_air_0.0381_bsdf_xml_2011SA1_same_solar_and_visible_only_normal/small_basis",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("3_layers/nfrc_102_air_nfrc_102_air_0.0381_bsdf_xml_2011SA1_same_solar_and_visible_only_normal/small_basis",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}

TEST_F(Test_3_layers_nfrc_102_air_nfrc_102_2011SA1, Test_Optical)
{
    test_optical_results(
      "3_layers/nfrc_102_air_nfrc_102_air_0.0381_bsdf_xml_2011SA1_same_solar_and_visible_only_normal/small_basis", glazing_system_u, update_results);
}
