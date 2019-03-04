#include <memory>
#include <gtest/gtest.h>
#include <cstdlib>
#include <memory>
#include <iostream>

#include "wincalc/wincalc.h"

#include "paths.h"

inline std::string separator()
{
#if defined _WIN32 || defined __CYGWIN__
    return "\\";
#else
    return "/";
#endif
}

class TestCalcFromDisk : public testing::Test {

protected:
	virtual void SetUp() {

	}

};

#define GTEST_COUT std::cerr << "[          ] [ INFO ]"

TEST_F(TestCalcFromDisk, TestCalcFromDisk_1) {
	SCOPED_TRACE( "Begin Test: Calc from disk." );

	GTEST_COUT << "test_dir" << std::endl;
	GTEST_COUT << test_dir << std::endl;

	std::string path_sep = separator();

	std::vector<std::string> product_paths;
	std::string clear_3_path = test_dir + path_sep + "products" + path_sep + "CLEAR_3.DAT";
	product_paths.push_back(clear_3_path);
	std::vector<Gap_Data> gaps;

	std::string standard_path = test_dir + path_sep + "standards" + path_sep + "W5_NFRC_2003.std";

	Thermal_Result u_result = calc_u(product_paths, gaps, standard_path, 1.0, 1.0);

	Thermal_Result shgc_result = calc_shgc(product_paths, gaps, standard_path, 1.0, 1.0);

}

