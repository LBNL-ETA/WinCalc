#include <memory>
#include <gtest/gtest.h>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "wincalc/wincalc.h"
#include "util.h"
#include "paths.h"


using namespace wincalc;
using namespace window_standards;

class TestCMA : public testing::Test
{
protected:
    
	thmxParser::ThmxFileContents top_frame;
	thmxParser::ThmxFileContents bottom_frame;
	thmxParser::ThmxFileContents left_frame;
	thmxParser::ThmxFileContents right_frame;
	thmxParser::ThmxFileContents spacer;

    virtual void SetUp()
    {
        std::filesystem::path top_frame_path(test_dir);
		top_frame_path /= "products";
		top_frame_path /= "sample-head_CMA.thmx";

		std::filesystem::path bottom_frame_path(test_dir);
		bottom_frame_path /= "products";
		bottom_frame_path /= "sample-sill_CMA.thmx";

		std::filesystem::path jamb_frame_path(test_dir);
		jamb_frame_path /= "products";
		jamb_frame_path /= "sample-jamb_CMA.thmx";

		std::filesystem::path spacer_path(test_dir);
		spacer_path /= "products";
		spacer_path /= "Spacer_CMA.thmx";

		top_frame = thmxParser::parseFile(top_frame_path.string());
		bottom_frame = thmxParser::parseFile(bottom_frame_path.string());
		left_frame = thmxParser::parseFile(jamb_frame_path.string());;
		right_frame = left_frame;

		spacer = thmxParser::parseFile(spacer_path.string());
    }
};

TEST_F(TestCMA, Test_CMA_Single_Vision)
{
	double glazing_system_u = 1.25800;
	double glazing_system_shgc = 0.34100; // SHGC for NFRC 102 and default NFRC SHGC environment
	double tvis = 0.53500; // direct-hemispheric visible front transmittance for NFRC 102
	double tsol = 0.8992599767990073; // direct-hemispheric solar front transmittance for NFRC 102
	double height = 1.5;
	double width = 1.2;

	auto results = wincalc::cmaSingleVision(top_frame, bottom_frame, left_frame, right_frame, spacer, width, height, glazing_system_u, glazing_system_shgc, tvis, tsol);
	EXPECT_EQ(results.u, 1.0);
	EXPECT_EQ(results.shgc, 1.0);
	EXPECT_EQ(results.vt, 1.0);
}
