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

class TestNFRC102_102 : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path clear_3_path(test_dir);
        clear_3_path /= "products";
        clear_3_path /= "CLEAR_3.json";

        std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
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

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, nfrc_u_environments());
        glazing_system_shgc =
			std::make_shared<Glazing_System>(standard, products, gaps, 1.0, 1.0, nfrc_shgc_environments());
    }
};

TEST_F(TestNFRC102_102, Test_Deflection_Off)
{
	auto deflection_max_u = glazing_system_u->deflection_max(Tarcog::ISO15099::System::Uvalue);
	auto deflection_max_shgc = glazing_system_shgc->deflection_max(Tarcog::ISO15099::System::SHGC);
	auto deflection_mean_u = glazing_system_u->deflection_mean(Tarcog::ISO15099::System::Uvalue);
	auto deflection_mean_shgc = glazing_system_shgc->deflection_mean(Tarcog::ISO15099::System::SHGC);

	EXPECT_NEAR(0.0, deflection_max_u[0], 1e-6);
	EXPECT_NEAR(0.0, deflection_max_u[1], 1e-6);
	EXPECT_NEAR(0.0, deflection_max_shgc[0], 1e-6);
	EXPECT_NEAR(0.0, deflection_max_shgc[1], 1e-6);
	EXPECT_NEAR(0.0, deflection_mean_u[0], 1e-6);
	EXPECT_NEAR(0.0, deflection_mean_u[1], 1e-6);
	EXPECT_NEAR(0.0, deflection_mean_shgc[0], 1e-6);
	EXPECT_NEAR(0.0, deflection_mean_shgc[1], 1e-6);
}

TEST_F(TestNFRC102_102, Test_Deflection_On)
{
	glazing_system_u->enable_deflection(true);
	glazing_system_shgc->enable_deflection(true);
	auto deflection_max_u = glazing_system_u->deflection_max(Tarcog::ISO15099::System::Uvalue);
	auto deflection_max_shgc = glazing_system_shgc->deflection_max(Tarcog::ISO15099::System::SHGC);
	auto deflection_mean_u = glazing_system_u->deflection_mean(Tarcog::ISO15099::System::Uvalue);
	auto deflection_mean_shgc = glazing_system_shgc->deflection_mean(Tarcog::ISO15099::System::SHGC);

	EXPECT_NEAR(-0.0011908618786342045, deflection_max_u[0], 1e-6);
	EXPECT_NEAR(0.0011908618786343212, deflection_max_u[1], 1e-6);
	EXPECT_NEAR(0.00044850958847475459, deflection_max_shgc[0], 1e-6);
	EXPECT_NEAR(-0.00044850958847463153, deflection_max_shgc[1], 1e-6);
	EXPECT_NEAR(-0.00049888302376557912, deflection_mean_u[0], 1e-6);
	EXPECT_NEAR(0.00049888302376562802, deflection_mean_u[1], 1e-6);
	EXPECT_NEAR(0.00018789233554336596, deflection_mean_shgc[0], 1e-6);
	EXPECT_NEAR(-0.00018789233554331440, deflection_mean_shgc[1], 1e-6);
}