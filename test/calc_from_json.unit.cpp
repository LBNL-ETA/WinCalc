#include <memory>
#include <gtest/gtest.h>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "thermal_calcs_from_measured_data_source.h"
#include "environmental_conditions.h"
#include "paths.h"

using namespace wincalc;
using namespace window_standards;

class TestCalcFromDisk : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};

TEST_F(TestCalcFromDisk, Test_NFRC_102_json)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 102 using json as a data source.");

    std::filesystem::path clear_3_path(test_dir);
    clear_3_path /= "products";
    clear_3_path /= "CLEAR_3.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::Parser parser;
    OpticsParser::ProductData clear_3 = parser.parseJSONFile(clear_3_path.string());
    products.push_back(clear_3);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());

    Thermal_Result u_result = calc_u(products, gaps, standard, 1.0, 1.0, nfrc_u_environments());
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.83384927443226908, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091386111722449287, 1e-14);

    Thermal_Result shgc_result = calc_shgc(products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments());
    EXPECT_NEAR(shgc_result.result, 0.86063405690810812, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.83384927443226908, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091386111722449287, 1e-14);
}

TEST_F(TestCalcFromDisk, Test_NFRC_102_102_json)
{
    SCOPED_TRACE("Begin Test: Double clear NFRC 102-102 using json as a data source.");

    std::filesystem::path clear_3_path(test_dir);
    clear_3_path /= "products";
    clear_3_path /= "CLEAR_3.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::Parser parser;
    OpticsParser::ProductData clear_3 = parser.parseJSONFile(clear_3_path.string());
    products.push_back(clear_3);
    products.push_back(clear_3);

    Gap_Data air_gap{Gas_Type::AIR, 0.0127};
    std::vector<Gap_Data> gaps;
    gaps.push_back(air_gap);

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());

    Thermal_Result u_result = calc_u(products, gaps, standard, 1.0, 1.0, nfrc_u_environments());
    EXPECT_NEAR(u_result.result, 2.7296194478984446, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.70329763318245331, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.096487646903148996, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[1], 0.072256097417367798, 1e-14);

    Thermal_Result shgc_result = calc_shgc(products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments());
    EXPECT_NEAR(shgc_result.result, 0.76330438232287146, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.70329763318245331, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.096487646903148996, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[1], 0.072256097417367798, 1e-14);
}

TEST_F(TestCalcFromDisk, Test_NFRC_913_json)
{
    SCOPED_TRACE("Begin Test: NFRC 913 using json as a data source.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "913.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::Parser parser;
    OpticsParser::ProductData product = parser.parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());

    Thermal_Result u_result = calc_u(products, gaps, standard, 1.0, 1.0, nfrc_u_environments());
    EXPECT_NEAR(u_result.result, 5.8512829756503013, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.51339858666473259, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.43152531181359716, 1e-14);

    Thermal_Result shgc_result = calc_shgc(products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments());
    EXPECT_NEAR(shgc_result.result, 0.64595603857916939, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.51339858666473259, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.43152531181359716, 1e-14);
}

#if 0

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
    Standard standard = load_optical_standard(standard_path.string());
    EXPECT_THROW(calc_u(products, gaps, standard, 1.0, 1.0), std::runtime_error)
      << "NFRC 21515 data has errors in the wavelength measurements.";
    
    EXPECT_THROW(calc_shgc(products, gaps, standard, 1.0, 1.0), std::runtime_error)
                                   << "NFRC 21515 data has errors in the wavelength measurements.";
    	
}

#endif

TEST_F(TestCalcFromDisk, Test_NFRC_21000_json)
{
    SCOPED_TRACE("Begin Test: NFRC 21000 using json as a data source.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "21000.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::Parser parser;
    OpticsParser::ProductData product = parser.parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());

    Thermal_Result u_result = calc_u(products, gaps, standard, 1.0, 1.0, nfrc_u_environments());
    EXPECT_NEAR(u_result.result, 5.9142344855758333, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.90343220261747503, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.016316611780412608, 1e-14);

    Thermal_Result shgc_result = calc_shgc(products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments());
    EXPECT_NEAR(shgc_result.result, 0.90815537392910106, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.90343220261747503, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.016316611780412608, 1e-14);
}

TEST_F(TestCalcFromDisk, Test_NFRC_2600_json)
{
    SCOPED_TRACE("Begin Test: NFRC 2600 using json as a data source.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "2600.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::Parser parser;
    OpticsParser::ProductData product = parser.parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());

    Thermal_Result u_result = calc_u(products, gaps, standard, 1.0, 1.0, nfrc_u_environments());
    EXPECT_NEAR(u_result.result, 5.7344786572407171, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.85412451953533775, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.075260134458516231, 1e-14);

    Thermal_Result shgc_result = calc_shgc(products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments());
    EXPECT_NEAR(shgc_result.result, 0.87732746171081011, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.85412451953533775, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.075260134458516231, 1e-14);
}

TEST_F(TestCalcFromDisk, Test_checker_tool_json_format)
{
    SCOPED_TRACE("Begin Test: Calculations using checker tool json as a data source.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "CGD89_092661-2-Efb.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::Parser parser;
    OpticsParser::ProductData product = parser.parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());

    Thermal_Result u_result = calc_u(products, gaps, standard, 1.0, 1.0, nfrc_u_environments());
    EXPECT_NEAR(u_result.result, 3.6258264794653012, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.68430691378849040, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.21092409927872730, 1e-14);

    Thermal_Result shgc_result = calc_shgc(products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments());
    EXPECT_NEAR(shgc_result.result, 0.72886709088851231, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.68430691378849040, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.21092409927872730, 1e-14);
}

