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

class Test_2_layers_nfrc_102_air_nfrc_102_deflection_youngs_modulus : public testing::Test
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
        clear_3.youngsModulus = 7.2e10;
        products.push_back(clear_3);
        products.push_back(clear_3);

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

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments());

        glazing_system_shgc = std::make_shared<Glazing_System>(
                standard, products, gaps, 1.0, 1.0, 90, nfrc_shgc_environments());
    }
};

TEST_F(Test_2_layers_nfrc_102_air_nfrc_102_deflection_youngs_modulus, Test_Deflection_Off)
{
    test_deflection_results("2_layers/nfrc_102_air_nfrc_102/deflection/youngs_modulus",
                            "deflection_off",
                            glazing_system_u,
                            Tarcog::ISO15099::System::Uvalue,
                            update_results);
}

TEST_F(Test_2_layers_nfrc_102_air_nfrc_102_deflection_youngs_modulus, Test_Deflection_On)
{
    glazing_system_u->enable_deflection(true);

    test_deflection_results("2_layers/nfrc_102_air_nfrc_102/deflection/youngs_modulus",
                            "deflection_on_winter_u_run",
                            glazing_system_u,
                            Tarcog::ISO15099::System::Uvalue,
                            update_results);
    test_deflection_results("2_layers/nfrc_102_air_nfrc_102/deflection/youngs_modulus",
                            "deflection_on_winter_shgc_run",
                            glazing_system_u,
                            Tarcog::ISO15099::System::SHGC,
                            update_results);

    glazing_system_shgc->enable_deflection(true);

    test_deflection_results("2_layers/nfrc_102_air_nfrc_102/deflection/youngs_modulus",
                            "deflection_on_summer_u_run",
                            glazing_system_shgc,
                            Tarcog::ISO15099::System::Uvalue,
                            update_results);
    test_deflection_results("2_layers/nfrc_102_air_nfrc_102/deflection/youngs_modulus",
                            "deflection_on_summer_shgc_run",
                            glazing_system_shgc,
                            Tarcog::ISO15099::System::SHGC,
                            update_results);
}
