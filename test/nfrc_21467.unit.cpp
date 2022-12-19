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

class TestNFRC21467 : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path product_path(test_dir);
        product_path /= "products";
        product_path /= "21467.json";

        std::vector<OpticsParser::ProductData> products;
        auto product = OpticsParser::parseJSONFile(product_path.string());
        products.push_back(product);

        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> gaps;

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


TEST_F(TestNFRC21467, Test_Thermal)
{
    test_thermal_results("NFRC_21467", "thermal_U_Environment", glazing_system_u, update_results);
    test_thermal_results(
      "NFRC_21467", "thermal_SHGC_Environment", glazing_system_shgc, update_results);
}

TEST_F(TestNFRC21467, Test_Optical)
{
    test_optical_results("NFRC_21467", glazing_system_u, update_results);
}
