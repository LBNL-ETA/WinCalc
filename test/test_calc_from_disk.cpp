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
	paths.push_back("invalid_product_path")
	std::vector<Gap_Data> gaps;
	gaps.push_back({"Air", 1.0});

	std::sting standard_path = "invalid_standard_path";

	double result = calc_u(product_paths, gaps, standard_path, 1.0, 1.0);

	EXPECT_EQ(result, 1.0);
}

