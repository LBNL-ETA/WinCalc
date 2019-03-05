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

class TestCalcFromDisk : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};

#define GTEST_COUT std::cerr << "[          ] [ INFO ]"

TEST_F(TestCalcFromDisk, Test_NFRC_103)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 103.");

    GTEST_COUT << "test_dir" << std::endl;
    GTEST_COUT << test_dir << std::endl;

    std::string path_sep = separator();

    std::vector<std::string> product_paths;
    std::string clear_3_path = test_dir + path_sep + "products" + path_sep + "CLEAR_3.DAT";
    product_paths.push_back(clear_3_path);
    std::vector<Gap_Data> gaps;

    std::string standard_path = test_dir + path_sep + "standards" + path_sep + "W5_NFRC_2003.std";

    Thermal_Result u_result = calc_u(product_paths, gaps, standard_path, 1.0, 1.0);
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.83380702773635118, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091376375800192963, 1e-14);

    Thermal_Result shgc_result = calc_shgc(product_paths, gaps, standard_path, 1.0, 1.0);
    EXPECT_NEAR(shgc_result.result, 0.86058891721415542, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.83380702773635118, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091376375800192963, 1e-14);
}

TEST_F(TestCalcFromDisk, Test_NFRC_103_103)
{
    SCOPED_TRACE("Begin Test: Double clear NFRC 103-103.");

    GTEST_COUT << "test_dir" << std::endl;
    GTEST_COUT << test_dir << std::endl;

    std::string path_sep = separator();

    std::vector<std::string> product_paths;
    std::string clear_3_path = test_dir + path_sep + "products" + path_sep + "CLEAR_3.DAT";
    product_paths.push_back(clear_3_path);
    product_paths.push_back(clear_3_path);

    Gap_Data air_gap{Gas_Type::AIR, 0.0127};
    std::vector<Gap_Data> gaps;
    gaps.push_back(air_gap);

    std::string standard_path = test_dir + path_sep + "standards" + path_sep + "W5_NFRC_2003.std";

    Thermal_Result u_result = calc_u(product_paths, gaps, standard_path, 1.0, 1.0);
    EXPECT_NEAR(u_result.result, 2.7296194478984446, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.70324342292094888, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.096478606342105686, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[1], 0.072235301276579358, 1e-14);

    Thermal_Result shgc_result = calc_shgc(product_paths, gaps, standard_path, 1.0, 1.0);
    EXPECT_NEAR(shgc_result.result, 0.86058891721415542, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.83380702773635118, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091376375800192963, 1e-14);
}
