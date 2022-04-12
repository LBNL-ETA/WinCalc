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

class TestPVSingleLayer : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path clear_3_path(test_dir);
        clear_3_path /= "products";
        clear_3_path /= "generic_pv.json";

        std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
        OpticsParser::Parser parser;
        auto product = parser.parseJSONFile(clear_3_path.string());
        products.push_back(product);

        std::vector<Engine_Gap_Info> gaps;

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

TEST_F(TestPVSingleLayer, Test_Thermal)
{
    test_thermal_results("PV_single_layer", "thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results(
      "PV_single_layer", "thermal_SHGC_Environment", glazing_system_shgc, update_results);
}


TEST_F(TestPVSingleLayer, Test_Optical)
{
    test_optical_results("PV_single_layer", glazing_system_u, update_results);
}
