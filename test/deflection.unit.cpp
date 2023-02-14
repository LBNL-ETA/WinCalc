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

class TestDeflection : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system;

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

        glazing_system = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments());
    }
};

TEST_F(TestDeflection, Test_Deflection_Off)
{
    test_deflection_results(
      "NFRC_102_NFRC_102", "deflection/deflection_off", glazing_system, update_results);
}

TEST_F(TestDeflection, Test_Deflection_On)
{
    glazing_system->enable_deflection(true);
    auto deflection_results =
      glazing_system->calc_deflection_properties(Tarcog::ISO15099::System::Uvalue);

    test_deflection_results(
      "NFRC_102_NFRC_102", "deflection/deflection_on", glazing_system, update_results);
}
