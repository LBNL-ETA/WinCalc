#include <memory>
#include <gtest/gtest.h>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "thermal_calcs_from_measured_data_source.h"

#include "paths.h"

class TestCalcFromDisk : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};

TEST_F(TestCalcFromDisk, Test_NFRC_103_json)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 103 using json as a data source.");

    std::filesystem::path clear_3_path(test_dir);
    clear_3_path /= "products";
    clear_3_path /= "CLEAR_3.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::ProductData clear_3 = OpticsParser::parseJSONFile(clear_3_path.string());
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

TEST_F(TestCalcFromDisk, Test_NFRC_103_103_json)
{
    SCOPED_TRACE("Begin Test: Double clear NFRC 103-103 using json as a data source.");

    std::filesystem::path clear_3_path(test_dir);
    clear_3_path /= "products";
    clear_3_path /= "CLEAR_3.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::ProductData clear_3 = OpticsParser::parseJSONFile(clear_3_path.string());
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

TEST_F(TestCalcFromDisk, Test_NFRC_913_json)
{
    SCOPED_TRACE("Begin Test: NFRC 913 using json as a data source.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "913.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::ProductData product = OpticsParser::parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Standard standard = load_standard(standard_path);

    Thermal_Result u_result = calc_u(products, gaps, standard, 1.0, 1.0);
    EXPECT_NEAR(u_result.result, 5.8512829756503013, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.51340056427384817, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.43152333420448147, 1e-14);

    Thermal_Result shgc_result = calc_shgc(products, gaps, standard, 1.0, 1.0);
    EXPECT_NEAR(shgc_result.result, 0.64595738334695485, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.51340056427384817, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.43152333420448147, 1e-14);
}

TEST_F(TestCalcFromDisk, Test_NFRC_21515_json)
{
    SCOPED_TRACE("Begin Test: NFRC 21515 using json as a data source.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "21515.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::ProductData product = OpticsParser::parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Standard standard = load_standard(standard_path);
    EXPECT_THROW(calc_u(products, gaps, standard, 1.0, 1.0), std::runtime_error)
      << "NFRC 21515 data has errors in the wavelength measurements.";
    
    EXPECT_THROW(calc_shgc(products, gaps, standard, 1.0, 1.0), std::runtime_error)
                                   << "NFRC 21515 data has errors in the wavelength measurements.";
    	
}

TEST_F(TestCalcFromDisk, Test_NFRC_21000_json)
{
    SCOPED_TRACE("Begin Test: NFRC 21000 using json as a data source.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "21000.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::ProductData product = OpticsParser::parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Standard standard = load_standard(standard_path);

    Thermal_Result u_result = calc_u(products, gaps, standard, 1.0, 1.0);
    EXPECT_NEAR(u_result.result, 5.9142344855758333, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.90344456267444773, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.016308899104861548, 1e-14);

    Thermal_Result shgc_result = calc_shgc(products, gaps, standard, 1.0, 1.0);
    EXPECT_NEAR(shgc_result.result, 0.90816549494516030, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.90344456267444773, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.016308899104861548, 1e-14);
}

TEST_F(TestCalcFromDisk, Test_NFRC_2600_json)
{
    SCOPED_TRACE("Begin Test: NFRC 2600 using json as a data source.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "2600.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::ProductData product = OpticsParser::parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Standard standard = load_standard(standard_path);

    Thermal_Result u_result = calc_u(products, gaps, standard, 1.0, 1.0);
    EXPECT_NEAR(u_result.result, 5.5661650115454711, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.85412076207801813, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.075260529980339358, 1e-14);

    Thermal_Result shgc_result = calc_shgc(products, gaps, standard, 1.0, 1.0);
    EXPECT_NEAR(shgc_result.result, 0.87692233510952988, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.85412076207801813, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.075260529980339358, 1e-14);
}