#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "calc_from_disk.h"

class TestCalcFromDisk : public testing::Test {

protected:
	virtual void SetUp() {

	}

};

TEST_F(TestCalcFromDisk, TestCalcFromDisk_1) {
	SCOPED_TRACE( "Begin Test: Calc from disk." );

	std::vector<std::string> product_paths;
	product_paths.push_back("C:\\Users\\dev\\Documents\\Source\\Repos\\wincalc\\test\\CLEAR_3.DAT");
	std::vector<Gap_Data> gaps;
//	gaps.push_back({"Air", 1.0});

	std::string standard_path = "C:\\Users\\dev\\Documents\\Source\\Repos\\wincalc\\test\\standards\\W5_NFRC_2003.std";

	double u_result = calc_u(product_paths, gaps, standard_path, 1.0, 1.0);

	double shgc_result = calc_shgc(product_paths, gaps, standard_path, 1.0, 1.0);
}

