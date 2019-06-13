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

TEST_F(TestGlazingSystem, Test_NFRC_102_glazing_system)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 102 as a glazing system.");

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

    EXPECT_NEAR(tdw_results.direct_direct.tf, 0.84166947254223046, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.tb, 0.84166947254223046, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.rf, 0.081669348122390156, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.rb, 0.081821469468301128, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.tf, 0.76538949170866932, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.tb, 0.76538949170866932, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.rf, 0.14446919978411446, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.rb, 0.14462322981142764, 1e-14);

    WCE_Optical_Result tkr_results = glazing_system.all_method_values(Method_Type::TKR);

    EXPECT_NEAR(tkr_results.direct_direct.tf, 0.72756118296130756, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.tb, 0.72756118296130756, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.rf, 0.076400289881924099, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.rb, 0.076797679220795181, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.tf, 0.65458124235662229, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.tb, 0.65458124235662229, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.rf, 0.13546933439526809, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.rb, 0.13587503517132948, 1e-14);

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

    EXPECT_NEAR(color_results.direct_direct.tf.lab.L, 95.972303072706140, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.a, -0.95083962881142847, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.b, 0.17036999848574208, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.G, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.B, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.X, 84.755218557622285, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Y, 89.941031751191431, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Z, 96.272498423487846, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.tb.lab.L, 95.972303072706140, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.a, -0.95083962881142847, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.b, 0.17036999848574208, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.G, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.B, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.X, 84.755218557622285, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Y, 89.941031751191431, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Z, 96.272498423487846, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.rf.lab.L, 34.538795714754407, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.a, -0.57014300716473620, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.b, -0.75762082784675000, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.R, 88, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.G, 90, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.B, 90, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.X, 7.7779321056277446, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Y, 8.2699083985629223, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Z, 9.1090816495843967, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.rb.lab.L, 34.538853599204501, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.a, -0.56970898770367095, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.b, -0.75752102707072888, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.R, 88, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.G, 90, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.B, 90, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.X, 7.7780055130769723, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Y, 8.2699368143358818, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Z, 9.1090816495843967, 1e-14);

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

    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.L, 92.664205431775031, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.a, -1.0720003464831374, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.b, 0.16624816089467753, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.G, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.B, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.X, 77.388794857803703, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Y, 82.202600827001675, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Z, 87.988029370583064, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.L, 92.664205431775031, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.a, -1.0720003464831374, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.b, 0.16624816089467753, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.G, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.B, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.X, 77.388794857803703, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Y, 82.202600827001675, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Z, 87.988029370583064, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.L, 45.320875340223516, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.a, -0.61072432920805664, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.b, -0.47392296711112891, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.R, 117, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.G, 119, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.B, 118, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.X, 13.906410553814327, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Y, 14.772400359833146, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Z, 16.068411963255556, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.L, 45.320915099507701, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.a, -0.61042649780562730, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.b, -0.47385441662115557, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.R, 117, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.G, 119, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.B, 118, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.X, 13.906484785003659, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Y, 14.772429094278383, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Z, 16.068411963255556, 1e-14);
}


TEST_F(TestGlazingSystem, Test_nfrc_18100)
{
    SCOPED_TRACE("Begin Test: Calculations using CEN standard and NFRC ID 18100.");

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
    Standard standard = load_standard(standard_path.string());

    Glazing_System glazing_system(products, gaps, standard, 1.0, 1.0);

    Thermal_Result u_result = glazing_system.u();
    EXPECT_NEAR(u_result.result, 5.9430476606957274, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.68430788281695709, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.21092862800360215, 1e-14);

    Thermal_Result shgc_result = glazing_system.shgc();
    EXPECT_NEAR(shgc_result.result, 0.74694841129369050, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.68430788281695709, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.21092862800360215, 1e-14);

    WCE_Optical_Result solar_results = glazing_system.all_method_values(Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.direct_direct.tf, 0.68430788281695709, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.tb, 0.68430788281695709, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.rf, 0.10476348917944101, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.rb, 0.10476348917944101, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.tf, 0.61300700039759082, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.tb, 0.61300700039759082, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.rf, 0.15347656356972597, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.rb, 0.15347656356972597, 1e-14);

    WCE_Optical_Result photopic_results = glazing_system.all_method_values(Method_Type::PHOTOPIC);

    EXPECT_NEAR(photopic_results.direct_direct.tf, 0.86832251036896257, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.tb, 0.86832251036896257, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.rf, 0.073941061735573410, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.rb, 0.073941061735573410, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.tf, 0.78938352538557022, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.tb, 0.78938352538557022, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.rf, 0.13523727253567985, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.rb, 0.13523727253567985, 1e-14);


    WCE_Optical_Result spf_results = glazing_system.all_method_values(Method_Type::SPF);

    EXPECT_NEAR(spf_results.direct_direct.tf, 0.10406612779131649, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.tb, 0.10406612779131649, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.rf, 0.023222366166546257, 1e-14);
    EXPECT_NEAR(spf_results.direct_direct.rb, 0.023222366166546257, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.tf, 0.083132884785762806, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.tb, 0.083132884785762806, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.rf, 0.062545528077484869, 1e-14);
    EXPECT_NEAR(spf_results.diffuse_diffuse.rb, 0.062545528077484869, 1e-14);

    WCE_Optical_Result tdw_results = glazing_system.all_method_values(Method_Type::TDW);

    EXPECT_NEAR(tdw_results.direct_direct.tf, 0.80541180103829935, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.tb, 0.80541180103829935, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.rf, 0.074301362945898120, 1e-14);
    EXPECT_NEAR(tdw_results.direct_direct.rb, 0.074301362945898120, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.tf, 0.72832986978770264, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.tb, 0.72832986978770264, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.rf, 0.13232795182714510, 1e-14);
    EXPECT_NEAR(tdw_results.diffuse_diffuse.rb, 0.13232795182714510, 1e-14);

    WCE_Optical_Result tkr_results = glazing_system.all_method_values(Method_Type::TKR);

    EXPECT_NEAR(tkr_results.direct_direct.tf, 0.70004453787036802, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.tb, 0.70004453787036802, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.rf, 0.063410223610606609, 1e-14);
    EXPECT_NEAR(tkr_results.direct_direct.rb, 0.063410223610606609, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.tf, 0.62581528054239899, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.tb, 0.62581528054239899, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.rf, 0.11826354008619838, 1e-14);
    EXPECT_NEAR(tkr_results.diffuse_diffuse.rb, 0.11826354008619838, 1e-14);

    WCE_Optical_Result tuv_results = glazing_system.all_method_values(Method_Type::TUV);

    EXPECT_NEAR(tuv_results.direct_direct.tf, 0.67881891042236853, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.tb, 0.67881891042236853, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.rf, 0.063074387898513190, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.rb, 0.063074387898513190, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.tf, 0.60210467096028131, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.tb, 0.60210467096028131, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.rf, 0.11784841786574150, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.rb, 0.11784841786574150, 1e-14);

    WCE_Color_Result color_results = glazing_system.color();

    EXPECT_NEAR(color_results.direct_direct.tf.lab.L, 94.555620447384200, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.a, 1.4508139564290978, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.b, 2.0114575478752172, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.G, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.B, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.X, 82.814625829526449, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Y, 86.570217965502778, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Z, 89.999761874675428, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.tb.lab.L, 94.555620447384200, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.a, 1.4508139564290978, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.b, 2.0114575478752172, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.G, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.B, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.X, 82.814625829526449, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Y, 86.570217965502778, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Z, 89.999761874675428, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.rf.lab.L, 33.291541055295447, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.a, -15.069320803774961, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.b, -7.6908288236109845, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.R, 39, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.G, 94, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.B, 98, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.X, 5.8327206142346890, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Y, 7.6726117204893045, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Z, 10.678492833796108, 1e-14);

    EXPECT_NEAR(color_results.direct_direct.rb.lab.L, 33.291541055295447, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.a, -15.069320803774961, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.b, -7.6908288236109845, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.R, 39, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.G, 94, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.B, 98, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.X, 5.8327206142346890, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Y, 7.6726117204893045, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Z, 10.678492833796108, 1e-14);

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

    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.L, 91.104767933391585, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.a, 1.2310697745389620, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.b, 1.9411819554705945, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.G, 252, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.B, 247, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.X, 75.214232201480627, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Y, 78.714084010519898, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Z, 81.842466036067222, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.L, 91.104767933391585, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.a, 1.2310697745389620, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.b, 1.9411819554705945, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.G, 252, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.B, 247, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.X, 75.214232201480627, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Y, 78.714084010519898, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Z, 81.842466036067222, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.L, 43.918398262135064, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.a, -10.062799492356140, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.b, -5.0820118667588066, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.R, 87, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.G, 121, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.B, 123, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.X, 11.595929068038634, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Y, 13.781821420487820, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Z, 17.083130677577881, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.L, 43.918398262135064, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.a, -10.062799492356140, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.b, -5.0820118667588066, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.R, 87, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.G, 121, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.B, 123, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.X, 11.595929068038634, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Y, 13.781821420487820, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Z, 17.083130677577881, 1e-14);
}

TEST_F(TestGlazingSystem, Test_CEN_18100)
{
    SCOPED_TRACE("Begin Test: Calculations using CEN standard and NFRC ID 18100.");

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
    standard_path /= "CENblackbody.std";
    Standard standard = load_standard(standard_path.string());

    Glazing_System glazing_system(products, gaps, standard, 1.0, 1.0);

#if 0   // CEN does not work with thermal calcs yet
    Thermal_Result u_result = glazing_system.u();
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-14);

    Thermal_Result shgc_result = glazing_system.shgc();
    EXPECT_NEAR(shgc_result.result, 0.86063515079923925, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.83385101472829093, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-14);
#endif

    WCE_Optical_Result solar_results = glazing_system.all_method_values(Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.direct_direct.tf, 0.71099480233705104, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.tb, 0.71099480233705104, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.rf, 0.096206310544553184, 1e-14);
    EXPECT_NEAR(solar_results.direct_direct.rb, 0.096206310544553184, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.tf, 0.63794850948211068, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.tb, 0.63794850948211068, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.rf, 0.14708624967220474, 1e-14);
    EXPECT_NEAR(solar_results.diffuse_diffuse.rb, 0.14708624967220474, 1e-14);

    WCE_Optical_Result photopic_results = glazing_system.all_method_values(Method_Type::PHOTOPIC);

    EXPECT_NEAR(photopic_results.direct_direct.tf, 0.86829208459208473, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.tb, 0.86829208459208473, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.rf, 0.073958714858714872, 1e-14);
    EXPECT_NEAR(photopic_results.direct_direct.rb, 0.073958714858714872, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.tf, 0.78935221564455493, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.tb, 0.78935221564455493, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.rf, 0.13525667684831866, 1e-14);
    EXPECT_NEAR(photopic_results.diffuse_diffuse.rb, 0.13525667684831866, 1e-14);

    WCE_Optical_Result tuv_results = glazing_system.all_method_values(Method_Type::TUV);

    EXPECT_NEAR(tuv_results.direct_direct.tf, 0.58680022914517072, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.tb, 0.58680022914517072, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.rf, 0.062468365658078714, 1e-14);
    EXPECT_NEAR(tuv_results.direct_direct.rb, 0.062468365658078714, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.tf, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.tb, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.rf, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.direct_diffuse.rb, 0.0, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.tf, 0.51624387678949246, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.tb, 0.51624387678949246, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.rf, 0.11499438276955672, 1e-14);
    EXPECT_NEAR(tuv_results.diffuse_diffuse.rb, 0.11499438276955672, 1e-14);

    WCE_Color_Result color_results = glazing_system.color();

	EXPECT_NEAR(color_results.direct_direct.tf.lab.L, 94.666217263591847, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.a, 1.0038394903462700, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.lab.b, 2.3170364131629428, 1e-14);    
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.G, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.rgb.B, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.X, 83.028312591606294, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Y, 86.830285384922604, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tf.trichromatic.Z, 91.092529581183740, 1e-14);

	EXPECT_NEAR(color_results.direct_direct.tb.lab.L, 94.666217263591847, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.a, 1.0038394903462700, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.lab.b, 2.3170364131629428, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.G, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.rgb.B, 255, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.X, 83.028312591606294, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Y, 86.830285384922604, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.tb.trichromatic.Z, 91.092529581183740, 1e-14);

	EXPECT_NEAR(color_results.direct_direct.rf.lab.L, 32.685672100834950, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.a, -13.456067709025742, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.lab.b, -9.4328475174762723, 1e-14);    
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.R, 38, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.G, 92, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.rgb.B, 101, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.X, 5.7584248621303411, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Y, 7.3931504074726693, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rf.trichromatic.Z, 11.074221343637136, 1e-14);

	EXPECT_NEAR(color_results.direct_direct.rb.lab.L, 32.685672100834950, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.a, -13.456067709025742, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.lab.b, -9.4328475174762723, 1e-14);    
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.R, 38, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.G, 92, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.rgb.B, 101, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.X, 5.7584248621303411, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Y, 7.3931504074726693, 1e-14);
    EXPECT_NEAR(color_results.direct_direct.rb.trichromatic.Z, 11.074221343637136, 1e-14);

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

	EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.L, 91.205381649557779, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.a, 0.81757398116044655, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.lab.b, 2.2108499191388242, 1e-14);    
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.G, 253, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.rgb.B, 249, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.X, 75.404098662194983, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Y, 78.936123385628918, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tf.trichromatic.Z, 82.856762497727459, 1e-14);

    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.L, 91.205381649557779, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.a, 0.81757398116044655, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.lab.b, 2.2108499191388242, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.G, 253, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.rgb.B, 249, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.X, 75.404098662194983, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Y, 78.936123385628918, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.tb.trichromatic.Z, 82.856762497727459, 1e-14);

	EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.L, 43.540458616337347, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.a, -9.1523474616345695, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.lab.b, -6.2165296319391494, 1e-14);    
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.R, 87, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.G, 119, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.rgb.B, 125, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.X, 11.523015097588450, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Y, 13.522673394145196, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rf.trichromatic.Z, 17.554745273890433, 1e-14);

	EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.L, 43.540458616337347, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.a, -9.1523474616345695, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.lab.b, -6.2165296319391494, 1e-14);    
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.R, 87, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.G, 119, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.rgb.B, 125, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.X, 11.523015097588450, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Y, 13.522673394145196, 1e-14);
    EXPECT_NEAR(color_results.diffuse_diffuse.rb.trichromatic.Z, 17.554745273890433, 1e-14);
}

TEST_F(TestGlazingSystem, Test_NFRC_102_102_glazing_system)
{
    SCOPED_TRACE("Begin Test: Double clear NFRC 102-102 as a glazing system.");

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
