#if 0
#include <memory>
#include <gtest/gtest.h>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <filesystem>
#include <OpticsParser.hpp>

#include "thermal_calcs.h"
#include "paths.h"
#include "environmental_conditions.h"



using namespace wincalc;
using namespace window_standards;

class TestCalcFromDisk : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};


TEST_F(TestCalcFromDisk, Test_NFRC_102_optics)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 102 using optics file as data source.");
    std::filesystem::path clear_3_path(test_dir);
    clear_3_path /= "products";
    clear_3_path /= "CLEAR_3.DAT";
    ::vector<std::shared_ptr<OpticsParser::ProductData>> products;
    OpticsParser::Parser parser;
    auto clear_3 = parser.parseFile(clear_3_path.string());
    products.push_back(clear_3);

    std::vector<Engine_Gap_Info> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";

    Optical_Standard standard = load_optical_standard(standard_path.string());

    double u_result = calc_u(products, gaps, standard, 1.0, 1.0, nfrc_u_environments());
    EXPECT_NEAR(u, 5.9125145552954441, 1e-6);
#if 0
    EXPECT_NEAR(u_result.t_sol, 0.83384927443226908, 1e-6);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091386111722449287, 1e-6);
#endif

    double shgc_result = calc_shgc(products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments());
    EXPECT_NEAR(shgc_result, 0.86063405690810812, 1e-6);
#if 0
    EXPECT_NEAR(shgc_result, 0.83384927443226908, 1e-6);
    EXPECT_NEAR(shgc_result, 0.091386111722449287, 1e-6);
#endif
}

TEST_F(TestCalcFromDisk, Test_NFRC_102_102_optics)
{
    SCOPED_TRACE("Begin Test: Double clear NFRC 102-102 using optics file as data source.");

    std::filesystem::path clear_3_path(test_dir);
    clear_3_path /= "products";
    clear_3_path /= "CLEAR_3.DAT";
    ::vector<std::shared_ptr<OpticsParser::ProductData>> products;
    OpticsParser::Parser parser;
    auto clear_3 = parser.parseFile(clear_3_path.string());
    products.push_back(clear_3);
    products.push_back(clear_3);

    Gap_Data air_gap{Gas_Type::AIR, 0.0127};
    std::vector<Engine_Gap_Info> gaps = convert({air_gap});    

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";

    Optical_Standard standard = load_optical_standard(standard_path.string());

    Thermal_Result u_result = calc_u(products, gaps, standard, 1.0, 1.0, nfrc_u_environments());
    EXPECT_NEAR(u_result.result, 2.7296194478984446, 1e-6);
    EXPECT_NEAR(u_result.t_sol, 0.70329763318245331, 1e-6);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.096487646903148996, 1e-6);
    EXPECT_NEAR(u_result.layer_solar_absorptances[1], 0.072256097417367798, 1e-6);

    Thermal_Result shgc_result = calc_shgc(products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments());
    EXPECT_NEAR(shgc_result.result, 0.76330438232287146, 1e-6);
    EXPECT_NEAR(shgc_result.t_sol, 0.70329763318245331, 1e-6);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.096487646903148996, 1e-6);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[1], 0.072256097417367798, 1e-6);
}

#endif
