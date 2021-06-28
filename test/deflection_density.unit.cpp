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

class TestDeflectionDensity : public testing::Test
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
		clear_3->density = 100;
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

TEST_F(TestDeflectionDensity, Test_Deflection_Off)
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

TEST_F(TestDeflectionDensity, Test_Deflection_On)
{
	glazing_system->enable_deflection(true);
	auto deflection_results = glazing_system->calc_deflection_properties(Tarcog::ISO15099::System::Uvalue);

	EXPECT_NEAR(-0.0011908618786342045, deflection_results.deflection_max[0], 1e-6);
	EXPECT_NEAR(0.0011908618786343212, deflection_results.deflection_max[1], 1e-6);
	EXPECT_NEAR(-0.00049888302376557912, deflection_results.deflection_mean[0], 1e-6);
	EXPECT_NEAR(0.00049888302376562802, deflection_results.deflection_mean[1], 1e-6);
	EXPECT_NEAR(-55.346780018703818, deflection_results.pressure_difference[0], 1e-6);
	EXPECT_NEAR(55.346780018709296, deflection_results.pressure_difference[1], 1e-6);
}