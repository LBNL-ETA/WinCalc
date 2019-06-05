#include <memory>
#include <gtest/gtest.h>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "wincalc/wincalc.h"

#include "paths.h"

class TestGlazingSystem : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};

TEST_F(TestGlazingSystem, Test_NFRC_103_glazing_system)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 103 as a glazing system.");

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
    Standard standard = load_standard(standard_path.string());

    Glazing_System glazing_system(products, gaps, standard, 1.0, 1.0);

    Thermal_Result u_result = glazing_system.u();
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-14);

    Thermal_Result shgc_result = glazing_system.shgc();
    EXPECT_NEAR(shgc_result.result, 0.86063515079923925, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-14);

    WCE_Optical_Result solar_results = glazing_system.all_method_values(Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.direct_direct.tf, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.tb, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.rf, 0.074765048919287855, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.rb, 0.074855794767823960, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.tf, 0.75334837568243473, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.tb, 0.75334837568243473, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.rf, 0.13582160214429620, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.rb, 0.13591473635815252, 1e-14);

    WCE_Optical_Result photopic_results = glazing_system.all_method_values(Method_Type::PHOTOPIC);

    EXPECT_NEAR(photopic_results.direct_direct.tf, 0.89925998453884770, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.tb, 0.89925998453884770, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.rf, 0.082563172831176687, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.rb, 0.082563521272277285, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.tf, 0.82185034176842187, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.tb, 0.82185034176842187, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.rf, 0.14757793943808908, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.rb, 0.14757829179791318, 1e-14);

    WCE_Optical_Result spf_results = glazing_system.all_method_values(Method_Type::SPF);

    EXPECT_NEAR(spf_results.direct_direct.tf, 0.092847372531649014, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.tb, 0.092847372531649014, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.rf, 0.049777244859762357, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.rb, 0.050726658558665656, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.tf, 0.078035374127229148, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.tb, 0.078035374127229148, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.rf, 0.098178562240941875, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.rb, 0.099197614848102633, 1e-14);

    WCE_Optical_Result tdw_results = glazing_system.all_method_values(Method_Type::TDW);

    EXPECT_NEAR(tdw_results.direct_direct.tf, 0.88455027707939504, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.tb, 0.88455027707939504, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.rf, 0.082076383610412737, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.rb, 0.082103867417672005, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.tf, 0.80695444628614621, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.tb, 0.80695444628614621, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.rf, 0.14632052149223410, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.rb, 0.14634816212636417, 1e-14);

    WCE_Optical_Result tkr_results = glazing_system.all_method_values(Method_Type::TKR);

    EXPECT_NEAR(tkr_results.direct_direct.tf, 0.87316376513614979, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.tb, 0.87316376513614979, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.rf, 0.083803337959073798, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.rb, 0.083881876534500452, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.tf, 0.79627298835580007, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.tb, 0.79627298835580007, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.rf, 0.14771062629560355, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.rb, 0.14778961302433385, 1e-14);

    WCE_Optical_Result tuv_results = glazing_system.all_method_values(Method_Type::TUV);

    EXPECT_NEAR(tuv_results.direct_direct.tf, 0.71627812708040550, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.tb, 0.71627812708040550, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.rf, 0.075711443154079433, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.rb, 0.076467194319106452, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.tf, 0.64123952691013475, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.tb, 0.64123952691013475, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.rf, 0.13353575295457998, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.rb, 0.13429242840518707, 1e-14);

    WCE_Color_Result color_results = glazing_system.color();

    EXPECT_NEAR(color_results.direct_direct.tf.lab.a, -0.95083962881142847, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.b, 0.17036999848574208, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.L, 95.972303072706140, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.G, 225, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.B, 170, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.X, 113.43654324410734, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Y, 89.941031751191431, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Z, 42.099918055206423, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.tb.lab.a, -0.95083962881142847, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.b, 0.17036999848574208, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.L, 95.972303072706140, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.G, 225, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.B, 170, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.X, 113.43654324410734, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Y, 89.941031751191431, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Z, 42.099918055206423, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.rf.lab.a, -0.57014300716473620, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.b, -0.75762082784675000, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.L, 34.538795714754407, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.R, 133, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.G, 74, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.B, 54, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.X, 10.409998896409247, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Y, 8.2699083985629223, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Z, 3.9833970997487507, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.rb.lab.a, -0.56970898770367095, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.b, -0.75752102707072888, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.L, 34.538853599204501, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.R, 133, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.G, 74, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.B, 54, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.X, 10.410097145076767, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Y, 8.2699368143358800, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Z, 3.9833970997487507, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.a, -1.0720003464831374, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.b, 0.16624816089467753, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.L, 92.664205431775031, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.G, 217, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.B, 164, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.X, 103.57730796869151, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Y, 82.202600827001675, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Z, 38.477123654213784, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.a, -1.0720003464831374, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.b, 0.16624816089467753, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.L, 92.664205431775031, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.G, 217, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.B, 164, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.X, 103.57730796869151, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Y, 82.202600827001675, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Z, 38.477123654213784, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.a, -0.61072432920805664, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.b, -0.47392296711112891, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.L, 45.320875340223516, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.R, 174, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.G, 98, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.B, 73, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.X, 18.612365928660576, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Y, 14.772400359833146, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Z, 7.0267089564314791, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.a, -0.61042649780562730, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.b, -0.47385441662115557, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.L, 45.320915099507701, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.R, 174, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.G, 98, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.B, 73, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.X, 18.612465279823397, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Y, 14.772429094278383, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Z, 7.0267089564314791, 1e-14);
}

TEST_F(TestGlazingSystem, Test_NFRC_103_103_glazing_system)
{
    SCOPED_TRACE("Begin Test: Double clear NFRC 103-103 as a glazing system.");

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
    Standard standard = load_standard(standard_path.string());

    Glazing_System glazing_system(products, gaps, standard, 1.0, 1.0);

    Thermal_Result u_result = glazing_system.u();
    EXPECT_NEAR(u_result.result, 2.7296194478984446, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.70330055648824519, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.096485399860676058, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[1], 0.072254488683882165, 1e-14);

    Thermal_Result shgc_result = glazing_system.shgc();
    EXPECT_NEAR(shgc_result.result, 0.76330593361763821, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.70330055648824519, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.096485399860676058, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[1], 0.072254488683882165, 1e-14);
}

#if 0

TEST_F(TestGlazingSystem, Test_NFRC_9023_glazing_system)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 9023 as a glazing system.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "9023.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::ProductData product = OpticsParser::parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Standard standard = load_standard(standard_path.string());

    Glazing_System glazing_system(products, gaps, standard, 1.0, 1.0);

    Thermal_Result u_result = glazing_system.u();
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-14);

    Thermal_Result shgc_result = glazing_system.shgc();
    EXPECT_NEAR(shgc_result.result, 0.86063515079923925, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-14);

    WCE_Optical_Result photopic_results = glazing_system.all_method_values(Method_Type::PHOTOPIC);

    EXPECT_NEAR(photopic_results.direct_direct.tf, 0.89925998453884770, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.tb, 0.89925998453884770, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.rf, 0.082563172831176687, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.rb, 0.082563521272277285, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.tf, 0.82185034176842187, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.tb, 0.82185034176842187, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.rf, 0.14757793943808908, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.rb, 0.14757829179791318, 1e-14);

    WCE_Optical_Result solar_results = glazing_system.all_method_values(Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.direct_direct.tf, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.tb, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.rf, 0.074765048919287855, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.rb, 0.074855794767823960, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.tf, 0.75334837568243473, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.tb, 0.75334837568243473, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.rf, 0.13582160214429620, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.rb, 0.13591473635815252, 1e-14);

    WCE_Optical_Result spf_results = glazing_system.all_method_values(Method_Type::SPF);

    EXPECT_NEAR(spf_results.direct_direct.tf, 0.092847372531649014, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.tb, 0.092847372531649014, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.rf, 0.049777244859762357, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.rb, 0.050726658558665656, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.tf, 0.078035374127229148, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.tb, 0.078035374127229148, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.rf, 0.098178562240941875, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.rb, 0.099197614848102633, 1e-14);

    WCE_Optical_Result tdw_results = glazing_system.all_method_values(Method_Type::TDW);

    EXPECT_NEAR(tdw_results.direct_direct.tf, 0.88455027707939504, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.tb, 0.88455027707939504, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.rf, 0.082076383610412737, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.rb, 0.082103867417672005, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.tf, 0.80695444628614621, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.tb, 0.80695444628614621, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.rf, 0.14632052149223410, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.rb, 0.14634816212636417, 1e-14);

    WCE_Optical_Result tkr_results = glazing_system.all_method_values(Method_Type::TKR);

    EXPECT_NEAR(tkr_results.direct_direct.tf, 0.87316376513614979, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.tb, 0.87316376513614979, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.rf, 0.083803337959073798, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.rb, 0.083881876534500452, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.tf, 0.79627298835580007, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.tb, 0.79627298835580007, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.rf, 0.14771062629560355, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.rb, 0.14778961302433385, 1e-14);

    WCE_Optical_Result tuv_results = glazing_system.all_method_values(Method_Type::TUV);

    EXPECT_NEAR(tuv_results.direct_direct.tf, 0.71627812708040550, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.tb, 0.71627812708040550, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.rf, 0.075711443154079433, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.rb, 0.076467194319106452, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.tf, 0.64123952691013475, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.tb, 0.64123952691013475, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.rf, 0.13353575295457998, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.rb, 0.13429242840518707, 1e-14);

    WCE_Color_Result color_results = glazing_system.color();

    EXPECT_NEAR(color_results.direct_direct.tf.lab.a, -0.95083962881142847, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.b, 0.17036999848574208, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.L, 95.972303072706140, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.G, 225, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.B, 170, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.X, 113.43654324410734, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Y, 89.941031751191431, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Z, 42.099918055206423, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.tb.lab.a, -0.95083962881142847, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.b, 0.17036999848574208, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.L, 95.972303072706140, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.G, 225, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.B, 170, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.X, 113.43654324410734, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Y, 89.941031751191431, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Z, 42.099918055206423, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.rf.lab.a, -0.57014300716473620, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.b, -0.75762082784675000, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.L, 34.538795714754407, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.R, 133, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.G, 74, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.B, 54, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.X, 10.409998896409247, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Y, 8.2699083985629223, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Z, 3.9833970997487507, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.rb.lab.a, -0.56970898770367095, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.b, -0.75752102707072888, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.L, 34.538853599204501, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.R, 133, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.G, 74, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.B, 54, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.X, 10.410097145076767, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Y, 8.2699368143358800, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Z, 3.9833970997487507, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.a, -1.0720003464831374, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.b, 0.16624816089467753, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.L, 92.664205431775031, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.G, 217, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.B, 164, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.X, 103.57730796869151, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Y, 82.202600827001675, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Z, 38.477123654213784, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.a, -1.0720003464831374, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.b, 0.16624816089467753, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.L, 92.664205431775031, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.G, 217, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.B, 164, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.X, 103.57730796869151, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Y, 82.202600827001675, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Z, 38.477123654213784, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.a, -0.61072432920805664, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.b, -0.47392296711112891, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.L, 45.320875340223516, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.R, 174, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.G, 98, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.B, 73, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.X, 18.612365928660576, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Y, 14.772400359833146, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Z, 7.0267089564314791, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.a, -0.61042649780562730, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.b, -0.47385441662115557, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.L, 45.320915099507701, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.R, 174, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.G, 98, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.B, 73, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.X, 18.612465279823397, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Y, 14.772429094278383, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Z, 7.0267089564314791, 1e-14);
}

TEST_F(TestGlazingSystem, Test_NFRC_21467_glazing_system)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 21467 as a glazing system.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "21467.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::ProductData product = OpticsParser::parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Standard standard = load_standard(standard_path.string());

    Glazing_System glazing_system(products, gaps, standard, 1.0, 1.0);

    Thermal_Result u_result = glazing_system.u();
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-14);

    Thermal_Result shgc_result = glazing_system.shgc();
    EXPECT_NEAR(shgc_result.result, 0.86063515079923925, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-14);

    WCE_Optical_Result solar_results = glazing_system.all_method_values(Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.direct_direct.tf, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.tb, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.rf, 0.074765048919287855, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.rb, 0.074855794767823960, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.tf, 0.75334837568243473, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.tb, 0.75334837568243473, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.rf, 0.13582160214429620, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.rb, 0.13591473635815252, 1e-14);

    WCE_Optical_Result photopic_results = glazing_system.all_method_values(Method_Type::PHOTOPIC);

    EXPECT_NEAR(photopic_results.direct_direct.tf, 0.89925998453884770, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.tb, 0.89925998453884770, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.rf, 0.082563172831176687, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.rb, 0.082563521272277285, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.tf, 0.82185034176842187, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.tb, 0.82185034176842187, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.rf, 0.14757793943808908, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.rb, 0.14757829179791318, 1e-14);

    WCE_Optical_Result spf_results = glazing_system.all_method_values(Method_Type::SPF);

    EXPECT_NEAR(spf_results.direct_direct.tf, 0.092847372531649014, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.tb, 0.092847372531649014, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.rf, 0.049777244859762357, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.rb, 0.050726658558665656, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.tf, 0.078035374127229148, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.tb, 0.078035374127229148, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.rf, 0.098178562240941875, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.rb, 0.099197614848102633, 1e-14);

    WCE_Optical_Result tdw_results = glazing_system.all_method_values(Method_Type::TDW);

    EXPECT_NEAR(tdw_results.direct_direct.tf, 0.88455027707939504, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.tb, 0.88455027707939504, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.rf, 0.082076383610412737, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.rb, 0.082103867417672005, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.tf, 0.80695444628614621, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.tb, 0.80695444628614621, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.rf, 0.14632052149223410, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.rb, 0.14634816212636417, 1e-14);

    WCE_Optical_Result tkr_results = glazing_system.all_method_values(Method_Type::TKR);

    EXPECT_NEAR(tkr_results.direct_direct.tf, 0.87316376513614979, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.tb, 0.87316376513614979, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.rf, 0.083803337959073798, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.rb, 0.083881876534500452, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.tf, 0.79627298835580007, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.tb, 0.79627298835580007, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.rf, 0.14771062629560355, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.rb, 0.14778961302433385, 1e-14);

    WCE_Optical_Result tuv_results = glazing_system.all_method_values(Method_Type::TUV);

    EXPECT_NEAR(tuv_results.direct_direct.tf, 0.71627812708040550, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.tb, 0.71627812708040550, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.rf, 0.075711443154079433, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.rb, 0.076467194319106452, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.tf, 0.64123952691013475, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.tb, 0.64123952691013475, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.rf, 0.13353575295457998, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.rb, 0.13429242840518707, 1e-14);

    WCE_Color_Result color_results = glazing_system.color();

    EXPECT_NEAR(color_results.direct_direct.tf.lab.a, -0.95083962881142847, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.b, 0.17036999848574208, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.L, 95.972303072706140, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.G, 225, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.B, 170, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.X, 113.43654324410734, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Y, 89.941031751191431, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Z, 42.099918055206423, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.tb.lab.a, -0.95083962881142847, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.b, 0.17036999848574208, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.L, 95.972303072706140, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.G, 225, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.B, 170, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.X, 113.43654324410734, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Y, 89.941031751191431, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Z, 42.099918055206423, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.rf.lab.a, -0.57014300716473620, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.b, -0.75762082784675000, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.L, 34.538795714754407, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.R, 133, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.G, 74, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.B, 54, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.X, 10.409998896409247, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Y, 8.2699083985629223, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Z, 3.9833970997487507, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.rb.lab.a, -0.56970898770367095, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.b, -0.75752102707072888, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.L, 34.538853599204501, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.R, 133, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.G, 74, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.B, 54, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.X, 10.410097145076767, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Y, 8.2699368143358800, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Z, 3.9833970997487507, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.a, -1.0720003464831374, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.b, 0.16624816089467753, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.L, 92.664205431775031, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.G, 217, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.B, 164, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.X, 103.57730796869151, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Y, 82.202600827001675, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Z, 38.477123654213784, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.a, -1.0720003464831374, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.b, 0.16624816089467753, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.L, 92.664205431775031, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.G, 217, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.B, 164, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.X, 103.57730796869151, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Y, 82.202600827001675, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Z, 38.477123654213784, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.a, -0.61072432920805664, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.b, -0.47392296711112891, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.L, 45.320875340223516, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.R, 174, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.G, 98, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.B, 73, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.X, 18.612365928660576, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Y, 14.772400359833146, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Z, 7.0267089564314791, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.a, -0.61042649780562730, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.b, -0.47385441662115557, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.L, 45.320915099507701, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.R, 174, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.G, 98, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.B, 73, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.X, 18.612465279823397, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Y, 14.772429094278383, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Z, 7.0267089564314791, 1e-14);
}

TEST_F(TestGlazingSystem, Test_NFRC_9223_glazing_system)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 9223 as a glazing system.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "9223.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::ProductData product = OpticsParser::parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Standard standard = load_standard(standard_path.string());

    Glazing_System glazing_system(products, gaps, standard, 1.0, 1.0);

    Thermal_Result u_result = glazing_system.u();
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-14);

    Thermal_Result shgc_result = glazing_system.shgc();
    EXPECT_NEAR(shgc_result.result, 0.86063515079923925, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-14);

    WCE_Optical_Result solar_results = glazing_system.all_method_values(Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.direct_direct.tf, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.tb, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.rf, 0.074765048919287855, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.rb, 0.074855794767823960, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.tf, 0.75334837568243473, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.tb, 0.75334837568243473, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.rf, 0.13582160214429620, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.rb, 0.13591473635815252, 1e-14);

    WCE_Optical_Result photopic_results = glazing_system.all_method_values(Method_Type::PHOTOPIC);

    EXPECT_NEAR(photopic_results.direct_direct.tf, 0.89925998453884770, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.tb, 0.89925998453884770, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.rf, 0.082563172831176687, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.rb, 0.082563521272277285, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.tf, 0.82185034176842187, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.tb, 0.82185034176842187, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.rf, 0.14757793943808908, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.rb, 0.14757829179791318, 1e-14);

    WCE_Optical_Result spf_results = glazing_system.all_method_values(Method_Type::SPF);

    EXPECT_NEAR(spf_results.direct_direct.tf, 0.092847372531649014, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.tb, 0.092847372531649014, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.rf, 0.049777244859762357, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.rb, 0.050726658558665656, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.tf, 0.078035374127229148, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.tb, 0.078035374127229148, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.rf, 0.098178562240941875, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.rb, 0.099197614848102633, 1e-14);

    WCE_Optical_Result tdw_results = glazing_system.all_method_values(Method_Type::TDW);

    EXPECT_NEAR(tdw_results.direct_direct.tf, 0.88455027707939504, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.tb, 0.88455027707939504, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.rf, 0.082076383610412737, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.rb, 0.082103867417672005, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.tf, 0.80695444628614621, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.tb, 0.80695444628614621, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.rf, 0.14632052149223410, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.rb, 0.14634816212636417, 1e-14);

    WCE_Optical_Result tkr_results = glazing_system.all_method_values(Method_Type::TKR);

    EXPECT_NEAR(tkr_results.direct_direct.tf, 0.87316376513614979, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.tb, 0.87316376513614979, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.rf, 0.083803337959073798, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.rb, 0.083881876534500452, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.tf, 0.79627298835580007, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.tb, 0.79627298835580007, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.rf, 0.14771062629560355, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.rb, 0.14778961302433385, 1e-14);

    WCE_Optical_Result tuv_results = glazing_system.all_method_values(Method_Type::TUV);

    EXPECT_NEAR(tuv_results.direct_direct.tf, 0.71627812708040550, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.tb, 0.71627812708040550, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.rf, 0.075711443154079433, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.rb, 0.076467194319106452, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.tf, 0.64123952691013475, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.tb, 0.64123952691013475, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.rf, 0.13353575295457998, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.rb, 0.13429242840518707, 1e-14);

    WCE_Color_Result color_results = glazing_system.color();

    EXPECT_NEAR(color_results.direct_direct.tf.lab.a, -0.95083962881142847, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.b, 0.17036999848574208, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.L, 95.972303072706140, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.G, 225, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.B, 170, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.X, 113.43654324410734, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Y, 89.941031751191431, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Z, 42.099918055206423, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.tb.lab.a, -0.95083962881142847, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.b, 0.17036999848574208, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.L, 95.972303072706140, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.G, 225, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.B, 170, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.X, 113.43654324410734, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Y, 89.941031751191431, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Z, 42.099918055206423, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.rf.lab.a, -0.57014300716473620, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.b, -0.75762082784675000, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.L, 34.538795714754407, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.R, 133, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.G, 74, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.B, 54, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.X, 10.409998896409247, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Y, 8.2699083985629223, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Z, 3.9833970997487507, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.rb.lab.a, -0.56970898770367095, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.b, -0.75752102707072888, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.L, 34.538853599204501, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.R, 133, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.G, 74, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.B, 54, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.X, 10.410097145076767, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Y, 8.2699368143358800, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Z, 3.9833970997487507, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.a, -1.0720003464831374, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.b, 0.16624816089467753, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.L, 92.664205431775031, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.G, 217, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.B, 164, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.X, 103.57730796869151, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Y, 82.202600827001675, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Z, 38.477123654213784, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.a, -1.0720003464831374, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.b, 0.16624816089467753, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.L, 92.664205431775031, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.G, 217, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.B, 164, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.X, 103.57730796869151, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Y, 82.202600827001675, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Z, 38.477123654213784, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.a, -0.61072432920805664, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.b, -0.47392296711112891, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.L, 45.320875340223516, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.R, 174, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.G, 98, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.B, 73, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.X, 18.612365928660576, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Y, 14.772400359833146, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Z, 7.0267089564314791, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.a, -0.61042649780562730, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.b, -0.47385441662115557, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.L, 45.320915099507701, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.R, 174, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.G, 98, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.B, 73, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.X, 18.612465279823397, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Y, 14.772429094278383, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Z, 7.0267089564314791, 1e-14);
}

TEST_F(TestGlazingSystem, Test_NFRC_6046_glazing_system)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 6046 as a glazing system.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "6046.json";

    std::vector<OpticsParser::ProductData> products;
    OpticsParser::ProductData product = OpticsParser::parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Standard standard = load_standard(standard_path.string());

    Glazing_System glazing_system(products, gaps, standard, 1.0, 1.0);

    Thermal_Result u_result = glazing_system.u();
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-14);

    Thermal_Result shgc_result = glazing_system.shgc();
    EXPECT_NEAR(shgc_result.result, 0.86063515079923925, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-14);

    WCE_Optical_Result solar_results = glazing_system.all_method_values(Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.direct_direct.tf, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.tb, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.rf, 0.074765048919287855, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.rb, 0.074855794767823960, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.tf, 0.75334837568243473, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.tb, 0.75334837568243473, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.rf, 0.13582160214429620, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.rb, 0.13591473635815252, 1e-14);

    WCE_Optical_Result photopic_results = glazing_system.all_method_values(Method_Type::PHOTOPIC);

    EXPECT_NEAR(photopic_results.direct_direct.tf, 0.89925998453884770, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.tb, 0.89925998453884770, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.rf, 0.082563172831176687, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.rb, 0.082563521272277285, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.tf, 0.82185034176842187, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.tb, 0.82185034176842187, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.rf, 0.14757793943808908, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.rb, 0.14757829179791318, 1e-14);

    WCE_Optical_Result spf_results = glazing_system.all_method_values(Method_Type::SPF);

    EXPECT_NEAR(spf_results.direct_direct.tf, 0.092847372531649014, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.tb, 0.092847372531649014, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.rf, 0.049777244859762357, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.rb, 0.050726658558665656, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.tf, 0.078035374127229148, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.tb, 0.078035374127229148, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.rf, 0.098178562240941875, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.rb, 0.099197614848102633, 1e-14);

    WCE_Optical_Result tdw_results = glazing_system.all_method_values(Method_Type::TDW);

    EXPECT_NEAR(tdw_results.direct_direct.tf, 0.88455027707939504, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.tb, 0.88455027707939504, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.rf, 0.082076383610412737, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.rb, 0.082103867417672005, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.tf, 0.80695444628614621, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.tb, 0.80695444628614621, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.rf, 0.14632052149223410, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.rb, 0.14634816212636417, 1e-14);

    WCE_Optical_Result tkr_results = glazing_system.all_method_values(Method_Type::TKR);

    EXPECT_NEAR(tkr_results.direct_direct.tf, 0.87316376513614979, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.tb, 0.87316376513614979, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.rf, 0.083803337959073798, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.rb, 0.083881876534500452, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.tf, 0.79627298835580007, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.tb, 0.79627298835580007, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.rf, 0.14771062629560355, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.rb, 0.14778961302433385, 1e-14);

    WCE_Optical_Result tuv_results = glazing_system.all_method_values(Method_Type::TUV);

    EXPECT_NEAR(tuv_results.direct_direct.tf, 0.71627812708040550, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.tb, 0.71627812708040550, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.rf, 0.075711443154079433, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.rb, 0.076467194319106452, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.tf, 0.64123952691013475, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.tb, 0.64123952691013475, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.rf, 0.13353575295457998, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.rb, 0.13429242840518707, 1e-14);

    WCE_Color_Result color_results = glazing_system.color();

    EXPECT_NEAR(color_results.direct_direct.tf.lab.a, -0.95083962881142847, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.b, 0.17036999848574208, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.L, 95.972303072706140, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.G, 225, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.B, 170, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.X, 113.43654324410734, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Y, 89.941031751191431, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Z, 42.099918055206423, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.tb.lab.a, -0.95083962881142847, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.b, 0.17036999848574208, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.L, 95.972303072706140, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.G, 225, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.B, 170, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.X, 113.43654324410734, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Y, 89.941031751191431, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Z, 42.099918055206423, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.rf.lab.a, -0.57014300716473620, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.b, -0.75762082784675000, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.L, 34.538795714754407, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.R, 133, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.G, 74, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.B, 54, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.X, 10.409998896409247, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Y, 8.2699083985629223, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Z, 3.9833970997487507, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.rb.lab.a, -0.56970898770367095, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.b, -0.75752102707072888, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.L, 34.538853599204501, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.R, 133, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.G, 74, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.B, 54, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.X, 10.410097145076767, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Y, 8.2699368143358800, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Z, 3.9833970997487507, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tf.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.tb.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rf.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.direct_diffuse.rb.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.a, -1.0720003464831374, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.b, 0.16624816089467753, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.L, 92.664205431775031, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.G, 217, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.B, 164, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.X, 103.57730796869151, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Y, 82.202600827001675, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Z, 38.477123654213784, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.a, -1.0720003464831374, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.b, 0.16624816089467753, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.L, 92.664205431775031, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.G, 217, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.B, 164, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.X, 103.57730796869151, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Y, 82.202600827001675, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Z, 38.477123654213784, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.a, -0.61072432920805664, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.b, -0.47392296711112891, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.L, 45.320875340223516, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.R, 174, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.G, 98, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.B, 73, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.X, 18.612365928660576, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Y, 14.772400359833146, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Z, 7.0267089564314791, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.a, -0.61042649780562730, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.b, -0.47385441662115557, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.L, 45.320915099507701, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.R, 174, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.G, 98, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.B, 73, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.X, 18.612465279823397, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Y, 14.772429094278383, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Z, 7.0267089564314791, 1e-14);
}
#endif