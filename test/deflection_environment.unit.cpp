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

class TestDeflectionEnvironment : public testing::Test
{
protected:
	std::shared_ptr<Glazing_System> glazing_system;

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

		glazing_system = std::make_shared<Glazing_System>(
			standard, products, gaps, 1.0, 1.0, 90, nfrc_shgc_environments());
	}
};

TEST_F(TestDeflectionEnvironment, Test_Deflection_Off)
{
	// test to make sure nothing affects results with deflection turned off.
	// test is not completed, need to test other results later.
	auto deflection_results = glazing_system->calc_deflection_properties(Tarcog::ISO15099::System::Uvalue);

	EXPECT_NEAR(0.0, deflection_results.deflection_max[0], 1e-6);
	EXPECT_NEAR(0.0, deflection_results.deflection_max[1], 1e-6);
	EXPECT_NEAR(0.0, deflection_results.deflection_mean[0], 1e-6);
	EXPECT_NEAR(0.0, deflection_results.deflection_mean[1], 1e-6);
	EXPECT_NEAR(0.0, deflection_results.pressure_difference[0], 1e-6);
	EXPECT_NEAR(0.0, deflection_results.pressure_difference[1], 1e-6);
}

TEST_F(TestDeflectionEnvironment, Test_Deflection_On)
{
	glazing_system->enable_deflection(true);
	auto deflection_results = glazing_system->calc_deflection_properties(Tarcog::ISO15099::System::Uvalue);

	EXPECT_NEAR(0.00044850958847475459, deflection_results.deflection_max[0], 1e-6);
	EXPECT_NEAR(-0.00044850958847463153, deflection_results.deflection_max[1], 1e-6);
	EXPECT_NEAR(0.00018789233554336596, deflection_results.deflection_mean[0], 1e-6);
	EXPECT_NEAR(-0.00018789233554331440, deflection_results.deflection_mean[1], 1e-6);
	EXPECT_NEAR(20.528391356654740, deflection_results.pressure_difference[0], 1e-6);
	EXPECT_NEAR(-20.528391356648967, deflection_results.pressure_difference[1], 1e-6);

	//change environment pressure, make sure deflection results change
	auto new_env = nfrc_shgc_environments();
	new_env.inside.pressure = 99000;
	new_env.outside.pressure = 110000;
	glazing_system->environments(new_env);
	deflection_results = glazing_system->calc_deflection_properties(Tarcog::ISO15099::System::Uvalue);
	
	EXPECT_NEAR(0.00044868691081281808, deflection_results.deflection_max[0], 1e-6);
	EXPECT_NEAR(-0.00044868691081287256, deflection_results.deflection_max[1], 1e-6);
	EXPECT_NEAR(0.00018796662048419868, deflection_results.deflection_mean[0], 1e-6);
	EXPECT_NEAR(-0.00018796662048422151, deflection_results.deflection_mean[1], 1e-6);
	EXPECT_NEAR(20.536708267866576, deflection_results.pressure_difference[0], 1e-6);
	EXPECT_NEAR(-20.536708267869130, deflection_results.pressure_difference[1], 1e-6);

}