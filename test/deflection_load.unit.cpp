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

class TestDeflectionLoad : public testing::Test
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

        Engine_Gap_Info air_gap(Gases::GasDef::Air, 0.0127);
        std::vector<Engine_Gap_Info> gaps;
        gaps.push_back(air_gap);

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        glazing_system = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments());
    }
};

TEST_F(TestDeflectionLoad, Test_Deflection_Off)
{
    // test to make sure nothing affects results with deflection turned off.
    // test is not completed, need to test other results later.
    glazing_system->set_applied_loads({12, 23});
    test_deflection_results("NFRC_102_NFRC_102",
                            "deflection/applied_loads/deflection_off",
                            glazing_system,
                            update_results);
}

TEST_F(TestDeflectionLoad, Test_Deflection_On)
{
    glazing_system->enable_deflection(true);
    glazing_system->set_applied_loads({12, 23});

    test_deflection_results("NFRC_102_NFRC_102",
                            "deflection/applied_loads/deflection_on_applied_loads_1",
                            glazing_system,
                            update_results);
    // change applied loads, make sure results change
    glazing_system->set_applied_loads({1, 1000});
    test_deflection_results("NFRC_102_NFRC_102",
                            "deflection/applied_loads/deflection_on_applied_loads_2",
                            glazing_system,
                            update_results);
}
