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

class TestCGDB18100 : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path product_path(test_dir);
        product_path /= "products";
        product_path /= "CGD89_092661-2-Efb.json";

        std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
        OpticsParser::Parser parser;
        auto product = parser.parseJSONFile(product_path.string());
        products.push_back(product);

        std::vector<Engine_Gap_Info> gaps;

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, nfrc_u_environments());
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, nfrc_shgc_environments());
    }
};

TEST_F(TestCGDB18100, Test_Thermal)
{
    test_thermal_results("CGDB_18100/thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results(
      "CGDB_18100/thermal_SHGC_Environment", glazing_system_shgc, update_results);
}

TEST_F(TestCGDB18100, Test_Optical)
{
    WCE_Optical_Results solar_results = glazing_system_u->optical_method_results("SOLAR");
    test_optical_results("CGDB_18100", glazing_system_u, update_results);
}
