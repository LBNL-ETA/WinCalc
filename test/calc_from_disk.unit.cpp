#include <memory>
#include <gtest/gtest.h>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <filesystem>

#include "thermal_calcs_from_measured_data_source.h"
#include "paths.h"

class TestCalcFromDisk : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};


TEST_F(TestCalcFromDisk, Test_NFRC_103_optics)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 103 using optics file as data source.");
    std::filesystem::path clear_3_path(test_dir);
    clear_3_path /= "products";
    clear_3_path /= "CLEAR_3.DAT";
    std::vector<OpticsParser::ProductData> products;
    OpticsParser::Parser parser;
    OpticsParser::ProductData clear_3 = parser.parseFile(clear_3_path.string());
    products.push_back(clear_3);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";

    Standard standard = load_standard(standard_path);

    Thermal_Result u_result = calc_u(products, gaps, standard, 1.0, 1.0);
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-14);

    Thermal_Result shgc_result = calc_shgc(products, gaps, standard, 1.0, 1.0);
    EXPECT_NEAR(shgc_result.result, 0.86063515079923925, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-14);
}

TEST_F(TestCalcFromDisk, Test_NFRC_103_103_optics)
{
    SCOPED_TRACE("Begin Test: Double clear NFRC 103-103 using optics file as data source.");

    std::filesystem::path clear_3_path(test_dir);
    clear_3_path /= "products";
    clear_3_path /= "CLEAR_3.DAT";
    std::vector<OpticsParser::ProductData> products;
    OpticsParser::Parser parser;
    OpticsParser::ProductData clear_3 = parser.parseFile(clear_3_path.string());
    products.push_back(clear_3);
    products.push_back(clear_3);

    Gap_Data air_gap{Gas_Type::AIR, 0.0127};
    std::vector<Gap_Data> gaps;
    gaps.push_back(air_gap);

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";

    Standard standard = load_standard(standard_path);

    Thermal_Result u_result = calc_u(products, gaps, standard, 1.0, 1.0);
    EXPECT_NEAR(u_result.result, 2.7296194478984446, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.70330055648824519, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.096485399860676058, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[1], 0.072254488683882165, 1e-14);

    Thermal_Result shgc_result = calc_shgc(products, gaps, standard, 1.0, 1.0);
    EXPECT_NEAR(shgc_result.result, 0.76330593361763821, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.70330055648824519, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.096485399860676058, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[1], 0.072254488683882165, 1e-14);
}
