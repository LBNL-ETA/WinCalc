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

class Test_2_layers_nfrc_102_air_nfrc_102_forced_ventilation : public testing::Test
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
		products.push_back(clear_3);

		auto gap_thickness = 0.0127;
		auto gap_layer = Tarcog::ISO15099::Layers::gap(gap_thickness);
		double gap_air_speed = 1;
		double gap_inlet_air_temperature = 294.15;
		auto ventilated_gap = Tarcog::ISO15099::Layers::forcedVentilationGap(gap_layer, gap_air_speed, gap_inlet_air_temperature);
        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> gaps;
		gaps.push_back(ventilated_gap);

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments());
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_shgc_environments());
    }
};

TEST_F(Test_2_layers_nfrc_102_air_nfrc_102_forced_ventilation, Test_Thermal)
{
    test_thermal_results("2_layers/nfrc_102_air_nfrc_102/forced_ventilation", "thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results(
      "2_layers/nfrc_102_air_nfrc_102/forced_ventilation", "thermal_SHGC_Environment", glazing_system_shgc, update_results);
}


TEST_F(Test_2_layers_nfrc_102_air_nfrc_102_forced_ventilation, Test_Optical)
{
    test_optical_results("2_layers/nfrc_102_air_nfrc_102/forced_ventilation", glazing_system_u, update_results);
}
