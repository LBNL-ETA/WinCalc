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
    OpticsParser::ProductData clear_3 = OpticsParser::parseJSONFile(clear_3_path.string());
    products.push_back(clear_3);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Standard standard = load_standard(standard_path);

    Glazing_System glazing_system(products, gaps, standard, 1.0, 1.0);

    Thermal_Result u_result = glazing_system.u();
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.83380702773635118, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091376375800192963, 1e-14);

    Thermal_Result shgc_result = glazing_system.shgc();
    EXPECT_NEAR(shgc_result.result, 0.86058891721415542, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.83380702773635118, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091376375800192963, 1e-14);

    WCE_Optical_Result solar_results = glazing_system.all_method_values(Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.tf_direct_direct, 0.83384277455847700, 1e-14);
    EXPECT_NEAR(solar_results.tb_direct_direct, 0.83384277455847700, 1e-14);
    EXPECT_NEAR(solar_results.rf_direct_direct, 0.074760828764400350, 1e-14);
    EXPECT_NEAR(solar_results.rb_direct_direct, 0.074851523059539488, 1e-14);
    EXPECT_NEAR(solar_results.tf_direct_diffuse, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.tb_direct_diffuse, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.rf_direct_diffuse, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.rb_direct_diffuse, 0.0, 1e-14);
    EXPECT_NEAR(solar_results.tf_diffuse_diffuse, 0.75268016487508993, 1e-14);
    EXPECT_NEAR(solar_results.tb_diffuse_diffuse, 0.75268016487508993, 1e-14);
    EXPECT_NEAR(solar_results.rf_diffuse_diffuse, 0.14598681372974426, 1e-14);
    EXPECT_NEAR(solar_results.rb_diffuse_diffuse, 0.14608026163424109, 1e-14);

    WCE_Color_Result color_results = glazing_system.color();

    EXPECT_NEAR(color_results.tf_direct_direct.lab.a, -0.95083962881142847, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_direct.lab.b, 0.17036999848574208, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_direct.lab.L, 95.972303072706140, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_direct.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_direct.rgb.G, 225, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_direct.rgb.B, 170, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_direct.trichromatic.X, 113.43654324410734, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_direct.trichromatic.Y, 89.941031751191431, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_direct.trichromatic.Z, 42.099918055206423, 1e-14);

    EXPECT_NEAR(color_results.tb_direct_direct.lab.a, -0.95083962881142847, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_direct.lab.b, 0.17036999848574208, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_direct.lab.L, 95.972303072706140, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_direct.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_direct.rgb.G, 225, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_direct.rgb.B, 170, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_direct.trichromatic.X, 113.43654324410734, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_direct.trichromatic.Y, 89.941031751191431, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_direct.trichromatic.Z, 42.099918055206423, 1e-14);

    EXPECT_NEAR(color_results.rf_direct_direct.lab.a, -0.57014300716473620, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_direct.lab.b, -0.75762082784675000, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_direct.lab.L, 34.538795714754407, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_direct.rgb.R, 133, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_direct.rgb.G, 74, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_direct.rgb.B, 54, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_direct.trichromatic.X, 10.409998896409247, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_direct.trichromatic.Y, 8.2699083985629223, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_direct.trichromatic.Z, 3.9833970997487507, 1e-14);

    EXPECT_NEAR(color_results.rb_direct_direct.lab.a, -0.56970898770367095, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_direct.lab.b, -0.75752102707072888, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_direct.lab.L, 34.538853599204501, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_direct.rgb.R, 133, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_direct.rgb.G, 74, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_direct.rgb.B, 54, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_direct.trichromatic.X, 10.410097145076767, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_direct.trichromatic.Y, 8.2699368143358800, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_direct.trichromatic.Z, 3.9833970997487507, 1e-14);

    EXPECT_NEAR(color_results.tf_direct_diffuse.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_diffuse.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_diffuse.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_diffuse.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_diffuse.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_diffuse.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_diffuse.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_diffuse.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tf_direct_diffuse.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.tb_direct_diffuse.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_diffuse.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_diffuse.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_diffuse.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_diffuse.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_diffuse.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_diffuse.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_diffuse.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.tb_direct_diffuse.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.rf_direct_diffuse.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_diffuse.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_diffuse.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_diffuse.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_diffuse.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_diffuse.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_diffuse.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_diffuse.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rf_direct_diffuse.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.rb_direct_diffuse.lab.a, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_diffuse.lab.b, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_diffuse.lab.L, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_diffuse.rgb.R, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_diffuse.rgb.G, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_diffuse.rgb.B, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_diffuse.trichromatic.X, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_diffuse.trichromatic.Y, 0.0, 1e-14);
    EXPECT_NEAR(color_results.rb_direct_diffuse.trichromatic.Z, 0.0, 1e-14);

    EXPECT_NEAR(color_results.tf_diffuse_diffuse.lab.a, -1.0763618564854638, 1e-14);
    EXPECT_NEAR(color_results.tf_diffuse_diffuse.lab.b, 0.16632766497590712, 1e-14);
    EXPECT_NEAR(color_results.tf_diffuse_diffuse.lab.L, 92.640282191769245, 1e-14);
    EXPECT_NEAR(color_results.tf_diffuse_diffuse.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.tf_diffuse_diffuse.rgb.G, 217, 1e-14);
    EXPECT_NEAR(color_results.tf_diffuse_diffuse.rgb.B, 164, 1e-14);
    EXPECT_NEAR(color_results.tf_diffuse_diffuse.trichromatic.X, 103.50585736609243, 1e-14);
    EXPECT_NEAR(color_results.tf_diffuse_diffuse.trichromatic.Y, 82.148320221313327, 1e-14);
    EXPECT_NEAR(color_results.tf_diffuse_diffuse.trichromatic.Z, 38.451644599921785, 1e-14);

    EXPECT_NEAR(color_results.tb_diffuse_diffuse.lab.a, -1.0763618564854638, 1e-14);
    EXPECT_NEAR(color_results.tb_diffuse_diffuse.lab.b, 0.16632766497590712, 1e-14);
    EXPECT_NEAR(color_results.tb_diffuse_diffuse.lab.L, 92.640282191769245, 1e-14);
    EXPECT_NEAR(color_results.tb_diffuse_diffuse.rgb.R, 255, 1e-14);
    EXPECT_NEAR(color_results.tb_diffuse_diffuse.rgb.G, 217, 1e-14);
    EXPECT_NEAR(color_results.tb_diffuse_diffuse.rgb.B, 164, 1e-14);
    EXPECT_NEAR(color_results.tb_diffuse_diffuse.trichromatic.X, 103.50585736609243, 1e-14);
    EXPECT_NEAR(color_results.tb_diffuse_diffuse.trichromatic.Y, 82.148320221313327, 1e-14);
    EXPECT_NEAR(color_results.tb_diffuse_diffuse.trichromatic.Z, 38.451644599921785, 1e-14);

    EXPECT_NEAR(color_results.rf_diffuse_diffuse.lab.a, -0.60578535627420216, 1e-14);
    EXPECT_NEAR(color_results.rf_diffuse_diffuse.lab.b, -0.45204697491307844, 1e-14);
    EXPECT_NEAR(color_results.rf_diffuse_diffuse.lab.L, 46.742995946800185, 1e-14);
    EXPECT_NEAR(color_results.rf_diffuse_diffuse.rgb.R, 180, 1e-14);
    EXPECT_NEAR(color_results.rf_diffuse_diffuse.rgb.G, 101, 1e-14);
    EXPECT_NEAR(color_results.rf_diffuse_diffuse.rgb.B, 75, 1e-14);
    EXPECT_NEAR(color_results.rf_diffuse_diffuse.trichromatic.X, 19.941808426633948, 1e-14);
    EXPECT_NEAR(color_results.rf_diffuse_diffuse.trichromatic.Y, 15.824200934457444, 1e-14);
    EXPECT_NEAR(color_results.rf_diffuse_diffuse.trichromatic.Z, 7.5201853479775478, 1e-14);

    EXPECT_NEAR(color_results.rb_diffuse_diffuse.lab.a, -0.60549954829924868, 1e-14);
    EXPECT_NEAR(color_results.rb_diffuse_diffuse.lab.b, -0.45198117705140817, 1e-14);
    EXPECT_NEAR(color_results.rb_diffuse_diffuse.lab.L, 46.743034109559957, 1e-14);
    EXPECT_NEAR(color_results.rb_diffuse_diffuse.rgb.R, 180, 1e-14);
    EXPECT_NEAR(color_results.rb_diffuse_diffuse.rgb.G, 101, 1e-14);
    EXPECT_NEAR(color_results.rb_diffuse_diffuse.rgb.B, 75, 1e-14);
    EXPECT_NEAR(color_results.rb_diffuse_diffuse.trichromatic.X, 19.941908262739332, 1e-14);
    EXPECT_NEAR(color_results.rb_diffuse_diffuse.trichromatic.Y, 15.824229809180611, 1e-14);
    EXPECT_NEAR(color_results.rb_diffuse_diffuse.trichromatic.Z, 7.5201853479775478, 1e-14);
}

TEST_F(TestGlazingSystem, Test_NFRC_103_103_glazing_system)
{
    SCOPED_TRACE("Begin Test: Double clear NFRC 103-103 as a glazing system.");

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

    Glazing_System glazing_system(products, gaps, standard, 1.0, 1.0);

    Thermal_Result u_result = glazing_system.u();
    EXPECT_NEAR(u_result.result, 2.7296194478984446, 1e-14);
    EXPECT_NEAR(u_result.t_sol, 0.70324342292094888, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.096478606342105686, 1e-14);
    EXPECT_NEAR(u_result.layer_solar_absorptances[1], 0.072235301276579358, 1e-14);

    Thermal_Result shgc_result = glazing_system.shgc();
    EXPECT_NEAR(shgc_result.result, 0.76323563155897300, 1e-14);
    EXPECT_NEAR(shgc_result.t_sol, 0.70324342292094888, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.096478606342105686, 1e-14);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[1], 0.072235301276579358, 1e-14);
}
