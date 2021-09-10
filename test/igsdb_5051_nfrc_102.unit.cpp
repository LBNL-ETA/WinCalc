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

class TestIGSDB5051NFRC102 : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path clear_3_path(test_dir);
        clear_3_path /= "products";
        clear_3_path /= "CLEAR_3.json";

        std::filesystem::path lowe_path(test_dir);
        lowe_path /= "products";
        lowe_path /= "igsdb_5051.json";

        std::vector<std::variant<std::shared_ptr<OpticsParser::ProductData>,
                                 wincalc::Product_Data_Optical_Thermal>>
          products;
        OpticsParser::Parser parser;
        auto clear_3 = parser.parseJSONFile(clear_3_path.string());
        auto lowe = parser.parseJSONFile(lowe_path.string());

        products.push_back(lowe);
        products.push_back(clear_3);


        Engine_Gap_Info air_gap(Gases::GasDef::Air, 0.0127);
        std::vector<Engine_Gap_Info> gaps;
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

TEST_F(TestIGSDB5051NFRC102, Test_Thermal)
{
	test_thermal_results("IGSDB_5051_NFRC_102/thermal_U_Environment", glazing_system_u, update_results);
	test_thermal_results("IGSDB_5051_NFRC_102/thermal_SHGC_Environment", glazing_system_shgc, update_results);
}

TEST_F(TestIGSDB5051NFRC102, Test_Optical)
{
	test_optical_results("IGSDB_5051_NFRC_102", glazing_system_u, update_results);
}

TEST_F(TestIGSDB5051NFRC102, Test_Thermal_Flipped_LowE)
{
	glazing_system_u->flip_layer(0, true);
	glazing_system_shgc->flip_layer(0, true);
	test_thermal_results("IGSDB_5051_NFRC_102_Flipped_LowE/thermal_U_Environment", glazing_system_u, update_results);
	test_thermal_results("IGSDB_5051_NFRC_102_Flipped_LowE/thermal_SHGC_Environment", glazing_system_shgc, update_results);
}

TEST_F(TestIGSDB5051NFRC102, Test_Optical_Flipped_LowE)
{
	glazing_system_shgc->flip_layer(0, true);
	test_optical_results("IGSDB_5051_NFRC_102_Flipped_LowE", glazing_system_u, update_results);
}