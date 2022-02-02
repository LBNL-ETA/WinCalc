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

class TestIGSDB_14025 : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};

TEST_F(TestIGSDB_14025, Test_IR)
{
    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "14025.json";

    std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
    OpticsParser::Parser parser;
    auto product = parser.parseJSONFile(product_path.string());
    auto layer = convert_to_solid_layer(product);

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());
    auto ir_results = calc_thermal_ir(standard, layer);
    EXPECT_EQ(ir_results.transmittance_front_diffuse_diffuse, 0);
    EXPECT_EQ(ir_results.transmittance_back_diffuse_diffuse, 0);
    EXPECT_NEAR(ir_results.emissivity_front_hemispheric, 0.84026278241669772, 1e-6);
    EXPECT_NEAR(ir_results.emissivity_back_hemispheric, 0.037321857598487045, 1e-6);
}
