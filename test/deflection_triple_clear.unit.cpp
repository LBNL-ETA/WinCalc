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

class TestDeflectionTripleClear : public testing::Test
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
        products.push_back(clear_3);

        double gap_1_thickness = 0.006;
        double gap_pressure = Gases::DefaultPressure;
		auto air_gas = Gases::CGas({{1.0, Gases::GasDef::Air}});
        auto air_gap_1 =
          std::make_shared<Tarcog::ISO15099::CIGUGapLayer>(gap_1_thickness, gap_pressure, air_gas);

        double gap_2_thickness = 0.025;
        auto air_gap_2 =
          std::make_shared<Tarcog::ISO15099::CIGUGapLayer>(gap_2_thickness, gap_pressure, air_gas);

        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> gaps;
        gaps.push_back(air_gap_1);
        gaps.push_back(air_gap_2);

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        auto environments = nfrc_u_environments();
        glazing_system =
          std::make_shared<Glazing_System>(standard, products, gaps, 1.0, 1.0, 90, environments);
    }
};

TEST_F(TestDeflectionTripleClear, Test_Deflection_Off)
{
    test_deflection_results("NFRC_102_NFRC_102_NFRC_102",
                            "deflection/deflection_off",
                            glazing_system,
                            Tarcog::ISO15099::System::Uvalue,
                            update_results);
}

TEST_F(TestDeflectionTripleClear, Test_Deflection_On)
{
    glazing_system->enable_deflection(true);
    glazing_system->set_deflection_properties(273, 101325);
    test_deflection_results("NFRC_102_NFRC_102_NFRC_102",
                            "deflection/deflection_on",
                            glazing_system,
                            Tarcog::ISO15099::System::Uvalue,
                            update_results);
}

TEST_F(TestDeflectionTripleClear, Test_Deflection_Measured)
{
    glazing_system->enable_deflection(true);
    const std::vector<double> measured_deflected_gaps = {0.0135, 0.013};
    glazing_system->set_deflection_properties(measured_deflected_gaps);
    test_deflection_results("NFRC_102_NFRC_102_NFRC_102",
                            "deflection/deflection_measured",
                            glazing_system,
                            Tarcog::ISO15099::System::Uvalue,
                            update_results);
}
