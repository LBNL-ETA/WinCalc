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

class Test_1_layer_nfrc_102_WCE : public testing::Test
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

        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> gaps;

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        auto env = nfrc_u_environments();
        env.outside.air_temperature = 300.0;
        env.outside.air_speed = 5.5;
        env.outside.radiation_temperature = 270;
        env.outside.direct_solar_radiation = 789;
        env.outside.coefficient_model = Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH;
        
        env.inside.air_temperature = 294.15;
        glazing_system = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, env);
    }
};

TEST_F(Test_1_layer_nfrc_102_WCE, Test_Thermal)
{
    test_thermal_results("1_layer/nfrc_102/default", "thermal_WCE_Environment", glazing_system, update_results);
}
