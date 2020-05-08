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

class TestGlazingSystem : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};

#if 1
TEST_F(TestGlazingSystem, Test_NFRC_102_glazing_system)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 102 as a glazing system.");

    std::filesystem::path clear_3_path(test_dir);
    clear_3_path /= "products";
    clear_3_path /= "CLEAR_3.json";

    std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
    OpticsParser::Parser parser;
    auto clear_3 = parser.parseJSONFile(clear_3_path.string());
    products.push_back(clear_3);

    std::vector<Engine_Gap_Info> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());

    Glazing_System glazing_system_u(products, gaps, standard, 1.0, 1.0, nfrc_u_environments());
    Glazing_System glazing_system_shgc(
      products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments());

    auto u_result = glazing_system_u.u();
    EXPECT_NEAR(u_result, 5.9125145552954441, 1e-6);

    auto u_layer_temperatues_u =
      glazing_system_u.layer_temperatures(Tarcog::ISO15099::System::Uvalue);

    EXPECT_NEAR(u_layer_temperatues_u[0], 263.01621462125490, 1e-6);
    EXPECT_NEAR(u_layer_temperatues_u[1], 263.71904705715122, 1e-6);

    auto u_layer_temperatues_shgc =
      glazing_system_u.layer_temperatures(Tarcog::ISO15099::System::SHGC);

    EXPECT_NEAR(u_layer_temperatues_shgc[0], 263.01621462383559, 1e-6);
    EXPECT_NEAR(u_layer_temperatues_shgc[1], 263.71904705531443, 1e-6);

    //    EXPECT_NEAR(u_result.t_sol, 0.83384927443226908, 1e-6);
    //    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091386111722449287, 1e-6);

    auto shgc_result = glazing_system_shgc.shgc();
    EXPECT_NEAR(shgc_result, 0.86063405698157958, 1e-6);
    //    EXPECT_NEAR(shgc_result.t_sol, 0.83384927443226908, 1e-6);
    //    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091386111722449287, 1e-6);

    auto shgc_layer_temperatues_u =
      glazing_system_shgc.layer_temperatures(Tarcog::ISO15099::System::Uvalue);

    EXPECT_NEAR(shgc_layer_temperatues_u[0], 303.05518474277812, 1e-6);
    EXPECT_NEAR(shgc_layer_temperatues_u[1], 302.92520195300676, 1e-6);

    auto shgc_layer_temperatues_shgc =
      glazing_system_shgc.layer_temperatures(Tarcog::ISO15099::System::SHGC);

    EXPECT_NEAR(shgc_layer_temperatues_shgc[0], 305.53865133878355, 1e-6);
    EXPECT_NEAR(shgc_layer_temperatues_shgc[1], 305.45379473432968, 1e-6);

    WCE_Optical_Results solar_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SOLAR);

    EXPECT_NEAR(
      solar_results.system_results.front.transmittance.direct_direct, 0.83384927443226908, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.back.transmittance.direct_direct, 0.83384927443226908, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.front.reflectance.direct_direct, 0.074764613845282393, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.back.reflectance.direct_direct, 0.074855359693818513, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.front.transmittance.diffuse_diffuse, 0.75334651440609124, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.back.transmittance.diffuse_diffuse, 0.75334651440609124, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.front.reflectance.diffuse_diffuse, 0.13582105577994347, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.back.reflectance.diffuse_diffuse, 0.13591418999379981, 1e-6);
    EXPECT_NEAR(
      solar_results.layer_results[0].front.absorptance.direct, 0.091386111722449287, 1e-6);
    EXPECT_NEAR(
      solar_results.layer_results[0].front.absorptance.diffuse, 0.10065783542220720, 1e-6);
    EXPECT_NEAR(solar_results.layer_results[0].back.absorptance.direct, 0.091386111722449287, 1e-6);
    EXPECT_NEAR(solar_results.layer_results[0].back.absorptance.diffuse, 0.10065783542220720, 1e-6);

    WCE_Optical_Results solar_results_50_degrees =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SOLAR, 50);
    EXPECT_NEAR(solar_results_50_degrees.system_results.front.transmittance.direct_direct,
                0.79665121300654163,
                1e-6);

    WCE_Optical_Results photopic_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::PHOTOPIC);

    EXPECT_NEAR(
      photopic_results.system_results.front.transmittance.direct_direct, 0.89925998453884770, 1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.back.transmittance.direct_direct, 0.89925998453884770, 1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.front.reflectance.direct_direct, 0.082563172831176687, 1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.back.reflectance.direct_direct, 0.082563521272277285, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.transmittance.diffuse_diffuse,
                0.82185034176842187,
                1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.diffuse_diffuse,
                0.82185034176842187,
                1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.front.reflectance.diffuse_diffuse, 0.14757793943808908, 1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.back.reflectance.diffuse_diffuse, 0.14757829179791318, 1e-6);

#    if 0
    WCE_Optical_Results spf_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SPF);

    EXPECT_NEAR(spf_results.system_results.front.transmittance.direct_direct, 0.092848281215754655, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.direct_direct, 0.092848281215754655, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.direct_direct, 0.049777244859762357, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.direct_direct, 0.050726658558665656, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.transmittance.diffuse_diffuse, 0.077907596000480486, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.diffuse_diffuse, 0.077907596000480486, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.diffuse_diffuse, 0.098173459998706705, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.diffuse_diffuse, 0.099192441334194997, 1e-6);
#    endif

    WCE_Optical_Results tdw_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TDW);

    EXPECT_NEAR(
      tdw_results.system_results.front.transmittance.direct_direct, 0.84164657137221865, 1e-6);
    EXPECT_NEAR(
      tdw_results.system_results.back.transmittance.direct_direct, 0.84164657137221865, 1e-6);
    EXPECT_NEAR(
      tdw_results.system_results.front.reflectance.direct_direct, 0.081666803547944408, 1e-6);
    EXPECT_NEAR(
      tdw_results.system_results.back.reflectance.direct_direct, 0.081818924893855380, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(
      tdw_results.system_results.front.transmittance.diffuse_diffuse, 0.76536538284501265, 1e-6);
    EXPECT_NEAR(
      tdw_results.system_results.back.transmittance.diffuse_diffuse, 0.76536538284501265, 1e-6);
    EXPECT_NEAR(
      tdw_results.system_results.front.reflectance.diffuse_diffuse, 0.14446518639920292, 1e-6);
    EXPECT_NEAR(
      tdw_results.system_results.back.reflectance.diffuse_diffuse, 0.14461921642651610, 1e-6);

    WCE_Optical_Results tkr_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TKR);

    EXPECT_NEAR(
      tkr_results.system_results.front.transmittance.direct_direct, 0.72756582869388287, 1e-6);
    EXPECT_NEAR(
      tkr_results.system_results.back.transmittance.direct_direct, 0.72756582869388287, 1e-6);
    EXPECT_NEAR(
      tkr_results.system_results.front.reflectance.direct_direct, 0.076395644149348768, 1e-6);
    EXPECT_NEAR(
      tkr_results.system_results.back.reflectance.direct_direct, 0.076793033488219850, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(
      tkr_results.system_results.front.transmittance.diffuse_diffuse, 0.65458547879017848, 1e-6);
    EXPECT_NEAR(
      tkr_results.system_results.back.transmittance.diffuse_diffuse, 0.65458547879017848, 1e-6);
    EXPECT_NEAR(
      tkr_results.system_results.front.reflectance.diffuse_diffuse, 0.13546504847396040, 1e-6);
    EXPECT_NEAR(
      tkr_results.system_results.back.reflectance.diffuse_diffuse, 0.13587074925002177, 1e-6);

    WCE_Optical_Results tuv_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TUV);

    EXPECT_NEAR(
      tuv_results.system_results.front.transmittance.direct_direct, 0.71453569050965315, 1e-6);
    EXPECT_NEAR(
      tuv_results.system_results.back.transmittance.direct_direct, 0.71453569050965315, 1e-6);
    EXPECT_NEAR(
      tuv_results.system_results.front.reflectance.direct_direct, 0.075577409571713883, 1e-6);
    EXPECT_NEAR(
      tuv_results.system_results.back.reflectance.direct_direct, 0.076199127154375323, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(
      tuv_results.system_results.front.transmittance.diffuse_diffuse, 0.63942124224017627, 1e-6);
    EXPECT_NEAR(
      tuv_results.system_results.back.transmittance.diffuse_diffuse, 0.63942124224017627, 1e-6);
    EXPECT_NEAR(
      tuv_results.system_results.front.reflectance.diffuse_diffuse, 0.13329118020770511, 1e-6);
    EXPECT_NEAR(
      tuv_results.system_results.back.reflectance.diffuse_diffuse, 0.13391614814582639, 1e-6);

    WCE_Color_Results color_results = glazing_system_u.color();

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.L,
                95.972303072706140,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.a,
                -0.95083962881142847,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.b,
                0.17036999848574208,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.G, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.B, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.X,
                84.755218557622285,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Y,
                89.941031751191431,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Z,
                96.272498423487846,
                1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.L,
                95.972303072706140,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.a,
                -0.95083962881142847,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.b,
                0.17036999848574208,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.G, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.B, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.X,
                84.755218557622285,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Y,
                89.941031751191431,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Z,
                96.272498423487846,
                1e-6);

    EXPECT_NEAR(
      color_results.system_results.front.reflectance.direct_direct.lab.L, 34.538795714754407, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.a,
                -0.57014300716473620,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.b,
                -0.75762082784675000,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.R, 88, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.G, 90, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.B, 90, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.X,
                7.7779321056277446,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Y,
                8.2699083985629223,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Z,
                9.1090816495843967,
                1e-6);

    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_direct.lab.L, 34.538853599204501, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.a,
                -0.56970898770367095,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.b,
                -0.75752102707072888,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.R, 88, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.G, 90, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.B, 90, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.X,
                7.7780055130769723,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Y,
                8.2699368143358818,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Z,
                9.1090816495843967,
                1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.L,
                92.664201196391531,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.a,
                -1.0720111896778683,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.b,
                0.16625723793792790,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.G, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.B, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.X,
                77.388780400865073,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Y,
                82.202591215017947,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Z,
                87.988006272784745,
                1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.L,
                92.664201196391531,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.a,
                -1.0720111896778683,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.b,
                0.16625723793792790,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.G, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.B, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.X,
                77.388780400865073,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Y,
                82.202591215017947,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Z,
                87.988006272784745,
                1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.L,
                45.320877948833605,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.a,
                -0.61075331638588137,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.b,
                -0.47391730817181177,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.R, 117, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.G, 119, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.B, 118, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.X,
                13.906407746754049,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Y,
                14.772402245101446,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Z,
                16.068411436111603,
                1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.L,
                45.320917706228727,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.a,
                -0.61045549925031750,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.b,
                -0.47384876093883310,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.R, 117, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.G, 119, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.B, 118, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.X,
                13.906481974388109,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Y,
                14.772430978183893,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Z,
                16.068411436111603,
                1e-6);
}


TEST_F(TestGlazingSystem, Test_nfrc_18100)
{
    SCOPED_TRACE("Begin Test: Calculations using CEN standard and NFRC ID 18100.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "CGD89_092661-2-Efb.json";

    std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
    OpticsParser::Parser parser;
    auto product = parser.parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Engine_Gap_Info> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());

    Glazing_System glazing_system_u(products, gaps, standard, 1.0, 1.0, nfrc_u_environments());
    Glazing_System glazing_system_shgc(
      products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments());

    auto u_result = glazing_system_u.u();
    EXPECT_NEAR(u_result, 3.6258264794653012, 1e-6);
    //    EXPECT_NEAR(u_result.t_sol, 0.68430691378849040, 1e-6);
    //    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.21092409927872730, 1e-6);

    auto shgc_result = glazing_system_shgc.shgc();
    EXPECT_NEAR(shgc_result, 0.72886709074334710, 1e-6);
    //    EXPECT_NEAR(shgc_result.t_sol, 0.68430691378849040, 1e-6);
    //    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.21092409927872730, 1e-6);

    WCE_Optical_Results solar_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SOLAR);

    EXPECT_NEAR(
      solar_results.system_results.front.transmittance.direct_direct, 0.68430691378849040, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.back.transmittance.direct_direct, 0.68430691378849040, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.front.reflectance.direct_direct, 0.10476898693278254, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.back.reflectance.direct_direct, 0.10476898693278254, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.front.transmittance.diffuse_diffuse, 0.61300591609510646, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.back.transmittance.diffuse_diffuse, 0.61300591609510646, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.front.reflectance.diffuse_diffuse, 0.15348172531293150, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.back.reflectance.diffuse_diffuse, 0.15348172531293150, 1e-6);

    WCE_Optical_Results photopic_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::PHOTOPIC);

    EXPECT_NEAR(
      photopic_results.system_results.front.transmittance.direct_direct, 0.86832251036896257, 1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.back.transmittance.direct_direct, 0.86832251036896257, 1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.front.reflectance.direct_direct, 0.073941061735573410, 1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.back.reflectance.direct_direct, 0.073941061735573410, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.transmittance.diffuse_diffuse,
                0.78938352538557022,
                1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.diffuse_diffuse,
                0.78938352538557022,
                1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.front.reflectance.diffuse_diffuse, 0.13523727253567985, 1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.back.reflectance.diffuse_diffuse, 0.13523727253567985, 1e-6);

#    if 0
    WCE_Optical_Results spf_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SPF);

    EXPECT_NEAR(spf_results.system_results.front.transmittance.direct_direct, 0.10406653669916405, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.direct_direct, 0.10406653669916405, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.direct_direct, 0.023223320284857192, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.direct_direct, 0.023223320284857192, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.transmittance.diffuse_diffuse, 0.082864881004934138, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.diffuse_diffuse, 0.082864881004934138, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.diffuse_diffuse, 0.062875851480523881, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.diffuse_diffuse, 0.062875851480523881, 1e-6);
#    endif

    WCE_Optical_Results tdw_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TDW);

    EXPECT_NEAR(
      tdw_results.system_results.front.transmittance.direct_direct, 0.80537668591094791, 1e-6);
    EXPECT_NEAR(
      tdw_results.system_results.back.transmittance.direct_direct, 0.80537668591094791, 1e-6);
    EXPECT_NEAR(
      tdw_results.system_results.front.reflectance.direct_direct, 0.074317902679795489, 1e-6);
    EXPECT_NEAR(
      tdw_results.system_results.back.reflectance.direct_direct, 0.074317902679795489, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(
      tdw_results.system_results.front.transmittance.diffuse_diffuse, 0.72829395039688338, 1e-6);
    EXPECT_NEAR(
      tdw_results.system_results.back.transmittance.diffuse_diffuse, 0.72829395039688338, 1e-6);
    EXPECT_NEAR(
      tdw_results.system_results.front.reflectance.diffuse_diffuse, 0.13234604089933558, 1e-6);
    EXPECT_NEAR(
      tdw_results.system_results.back.reflectance.diffuse_diffuse, 0.13234604089933558, 1e-6);

    WCE_Optical_Results tkr_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TKR);

    EXPECT_NEAR(
      tkr_results.system_results.front.transmittance.direct_direct, 0.70007334141233502, 1e-6);
    EXPECT_NEAR(
      tkr_results.system_results.back.transmittance.direct_direct, 0.70007334141233502, 1e-6);
    EXPECT_NEAR(
      tkr_results.system_results.front.reflectance.direct_direct, 0.063393034400077902, 1e-6);
    EXPECT_NEAR(
      tkr_results.system_results.back.reflectance.direct_direct, 0.063393034400077902, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(
      tkr_results.system_results.front.transmittance.diffuse_diffuse, 0.62584075119000171, 1e-6);
    EXPECT_NEAR(
      tkr_results.system_results.back.transmittance.diffuse_diffuse, 0.62584075119000171, 1e-6);
    EXPECT_NEAR(
      tkr_results.system_results.front.reflectance.diffuse_diffuse, 0.11824996341674447, 1e-6);
    EXPECT_NEAR(
      tkr_results.system_results.back.reflectance.diffuse_diffuse, 0.11824996341674447, 1e-6);

    WCE_Optical_Results tuv_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TUV);

    EXPECT_NEAR(
      tuv_results.system_results.front.transmittance.direct_direct, 0.68235739699681930, 1e-6);
    EXPECT_NEAR(
      tuv_results.system_results.back.transmittance.direct_direct, 0.68235739699681930, 1e-6);
    EXPECT_NEAR(
      tuv_results.system_results.front.reflectance.direct_direct, 0.060889640505954565, 1e-6);
    EXPECT_NEAR(
      tuv_results.system_results.back.reflectance.direct_direct, 0.060889640505954565, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(
      tuv_results.system_results.front.transmittance.diffuse_diffuse, 0.60579610242540716, 1e-6);
    EXPECT_NEAR(
      tuv_results.system_results.back.transmittance.diffuse_diffuse, 0.60579610242540716, 1e-6);
    EXPECT_NEAR(
      tuv_results.system_results.front.reflectance.diffuse_diffuse, 0.11519549389113358, 1e-6);
    EXPECT_NEAR(
      tuv_results.system_results.back.reflectance.diffuse_diffuse, 0.11519549389113358, 1e-6);

    WCE_Color_Results color_results = glazing_system_u.color();

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.L,
                94.555620447384200,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.a,
                1.4508139564290978,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.b,
                2.0114575478752172,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.G, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.B, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.X,
                82.814625829526449,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Y,
                86.570217965502778,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Z,
                89.999761874675428,
                1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.L,
                94.555620447384200,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.a,
                1.4508139564290978,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.b,
                2.0114575478752172,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.G, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.B, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.X,
                82.814625829526449,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Y,
                86.570217965502778,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Z,
                89.999761874675428,
                1e-6);

    EXPECT_NEAR(
      color_results.system_results.front.reflectance.direct_direct.lab.L, 33.291541055295447, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.a,
                -15.069320803774961,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.b,
                -7.6908288236109845,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.R, 39, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.G, 94, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.B, 98, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.X,
                5.8327206142346890,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Y,
                7.6726117204893045,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Z,
                10.678492833796108,
                1e-6);

    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_direct.lab.L, 33.291541055295447, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_direct.lab.a, -15.069320803774961, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_direct.lab.b, -7.6908288236109845, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.R, 39, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.G, 94, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.B, 98, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.X,
                5.8327206142346890,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Y,
                7.6726117204893045,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Z,
                10.678492833796108,
                1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.L,
                91.104767933391585,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.a,
                1.2310697745389620,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.b,
                1.9411819554705945,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.G, 252, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.B, 247, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.X,
                75.214232201480627,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Y,
                78.714084010519898,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Z,
                81.842466036067222,
                1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.L,
                91.104767933391585,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.a,
                1.2310697745389620,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.b,
                1.9411819554705945,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.G, 252, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.B, 247, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.X,
                75.214232201480627,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Y,
                78.714084010519898,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Z,
                81.842466036067222,
                1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.L,
                43.918398262135064,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.a,
                -10.062799492356140,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.b,
                -5.0820118667588066,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.R, 87, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.G, 121, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.B, 123, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.X,
                11.595929068038634,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Y,
                13.781821420487820,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Z,
                17.083130677577881,
                1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.L,
                43.918398262135064,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.a,
                -10.062799492356140,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.b,
                -5.0820118667588066,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.R, 87, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.G, 121, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.B, 123, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.X,
                11.595929068038634,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Y,
                13.781821420487820,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Z,
                17.083130677577881,
                1e-6);
}

TEST_F(TestGlazingSystem, Test_CEN_18100)
{
    SCOPED_TRACE("Begin Test: Calculations using CEN standard and NFRC ID 18100.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "CGD89_092661-2-Efb.json";

    std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
    OpticsParser::Parser parser;
    auto product = parser.parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Engine_Gap_Info> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "CENblackbody.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());

    Glazing_System glazing_system_u(products, gaps, standard, 1.0, 1.0, nfrc_u_environments());
    Glazing_System glazing_system_shgc(
      products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments());

#    if 0   // CEN does not work with thermal calcs yet
    Thermal_Result u_result = glazing_system_u.u();
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-6);
    EXPECT_NEAR(u_result.t_sol, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-6);

    Thermal_Result shgc_result = glazing_system_u.shgc();
    EXPECT_NEAR(shgc_result.result, 0.86063515079923925, 1e-6);
    EXPECT_NEAR(shgc_result.t_sol, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-6);
#    endif

    WCE_Optical_Results solar_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SOLAR);

    EXPECT_NEAR(
      solar_results.system_results.front.transmittance.direct_direct, 0.71099188861066809, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.back.transmittance.direct_direct, 0.71099188861066809, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.front.reflectance.direct_direct, 0.096218147557983649, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.back.reflectance.direct_direct, 0.096218147557983649, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.front.transmittance.diffuse_diffuse, 0.63794508795388927, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.back.transmittance.diffuse_diffuse, 0.63794508795388927, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.front.reflectance.diffuse_diffuse, 0.14709734859263215, 1e-6);
    EXPECT_NEAR(
      solar_results.system_results.back.reflectance.diffuse_diffuse, 0.14709734859263215, 1e-6);

    WCE_Optical_Results photopic_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::PHOTOPIC);

    EXPECT_NEAR(
      photopic_results.system_results.front.transmittance.direct_direct, 0.86829208459208473, 1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.back.transmittance.direct_direct, 0.86829208459208473, 1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.front.reflectance.direct_direct, 0.073958714858714872, 1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.back.reflectance.direct_direct, 0.073958714858714872, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.transmittance.diffuse_diffuse,
                0.78935221564455493,
                1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.diffuse_diffuse,
                0.78935221564455493,
                1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.front.reflectance.diffuse_diffuse, 0.13525667684831866, 1e-6);
    EXPECT_NEAR(
      photopic_results.system_results.back.reflectance.diffuse_diffuse, 0.13525667684831866, 1e-6);

#    if 0 
	//CHECK THIS
    WCE_Optical_Results tuv_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TUV);

    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_direct, 0.58733483792234731, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_direct, 0.58733483792234731, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_direct, 0.062256898186217748, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_direct, 0.062256898186217748, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.diffuse_diffuse, 0.51653097170954498, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.diffuse_diffuse, 0.51653097170954498, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.diffuse_diffuse, 0.11484012312285924, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.diffuse_diffuse, 0.11484012312285924, 1e-6);
#    endif

    WCE_Color_Results color_results = glazing_system_u.color();

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.L,
                94.666217263591847,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.a,
                1.0038394903462700,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.b,
                2.3170364131629428,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.G, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.B, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.X,
                83.028312591606294,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Y,
                86.830285384922604,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Z,
                91.092529581183740,
                1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.L,
                94.666217263591847,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.a,
                1.0038394903462700,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.b,
                2.3170364131629428,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.G, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.B, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.X,
                83.028312591606294,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Y,
                86.830285384922604,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Z,
                91.092529581183740,
                1e-6);

    EXPECT_NEAR(
      color_results.system_results.front.reflectance.direct_direct.lab.L, 32.685672100834950, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.a,
                -13.456067709025742,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.b,
                -9.4328475174762723,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.R, 38, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.G, 92, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.B, 101, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.X,
                5.7584248621303411,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Y,
                7.3931504074726693,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Z,
                11.074221343637136,
                1e-6);

    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_direct.lab.L, 32.685672100834950, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_direct.lab.a, -13.456067709025742, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_direct.lab.b, -9.4328475174762723, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.R, 38, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.G, 92, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.B, 101, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.X,
                5.7584248621303411,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Y,
                7.3931504074726693,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Z,
                11.074221343637136,
                1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(
      color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.L,
                91.205381649557779,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.a,
                0.81757398116044655,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.b,
                2.2108499191388242,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.G, 253, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.B, 249, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.X,
                75.404098662194983,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Y,
                78.936123385628918,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Z,
                82.856762497727459,
                1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.L,
                91.205381649557779,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.a,
                0.81757398116044655,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.b,
                2.2108499191388242,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.G, 253, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.B, 249, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.X,
                75.404098662194983,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Y,
                78.936123385628918,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Z,
                82.856762497727459,
                1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.L,
                43.540458616337347,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.a,
                -9.1523474616345695,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.b,
                -6.2165296319391494,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.R, 87, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.G, 119, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.B, 125, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.X,
                11.523015097588450,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Y,
                13.522673394145196,
                1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Z,
                17.554745273890433,
                1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.L,
                43.540458616337347,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.a,
                -9.1523474616345695,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.b,
                -6.2165296319391494,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.R, 87, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.G, 119, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.B, 125, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.X,
                11.523015097588450,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Y,
                13.522673394145196,
                1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Z,
                17.554745273890433,
                1e-6);
}

TEST_F(TestGlazingSystem, Test_NFRC_102_102_glazing_system)
{
    SCOPED_TRACE("Begin Test: Double clear NFRC 102-102 as a glazing system.");

    std::filesystem::path clear_3_path(test_dir);
    clear_3_path /= "products";
    clear_3_path /= "CLEAR_3.json";

    std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
    OpticsParser::Parser parser;
    auto clear_3 = parser.parseJSONFile(clear_3_path.string());
    products.push_back(clear_3);
    products.push_back(clear_3);

    Engine_Gap_Info air_gap(Gases::GasDef::Air, 0.0127);
    std::vector<Engine_Gap_Info> gaps;
    gaps.push_back(air_gap);

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());

    Glazing_System glazing_system_u(products, gaps, standard, 1.0, 1.0, nfrc_u_environments());
    Glazing_System glazing_system_shgc(
      products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments());

    auto u_result = glazing_system_u.u();
    EXPECT_NEAR(u_result, 2.7297109415164953, 1e-6);
    //    EXPECT_NEAR(u_result.t_sol, 0.70329763318245331, 1e-6);
    //    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.096487646903148996, 1e-6);
    //    EXPECT_NEAR(u_result.layer_solar_absorptances[1], 0.072256097417367798, 1e-6);

    auto u_layer_effective_conductivity_solid_layers =
      glazing_system_u.solid_layers_effective_conductivities(Tarcog::ISO15099::System::Uvalue);

    EXPECT_NEAR(u_layer_effective_conductivity_solid_layers[0], 1, 1e-6);
    EXPECT_NEAR(u_layer_effective_conductivity_solid_layers[1], 1, 1e-6);

    auto u_layer_effective_conductivity_gap_layers =
      glazing_system_u.gap_layers_effective_conductivities(Tarcog::ISO15099::System::Uvalue);

    EXPECT_NEAR(u_layer_effective_conductivity_gap_layers[0], 0.066903670731241469, 1e-6);

    auto u_layer_effective_conductivity =
      glazing_system_u.system_effective_conductivity(Tarcog::ISO15099::System::Uvalue);

    EXPECT_NEAR(u_layer_effective_conductivity, 0.095941163943300142, 1e-6);

    auto shgc_result = glazing_system_shgc.shgc();
    EXPECT_NEAR(shgc_result, 0.76330438238470044, 1e-6);
    //    EXPECT_NEAR(shgc_result.t_sol, 0.70329763318245331, 1e-6);
    //    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.096487646903148996, 1e-6);
    //    EXPECT_NEAR(shgc_result.layer_solar_absorptances[1], 0.072256097417367798, 1e-6);
}

#endif

#if 1
TEST_F(TestGlazingSystem, Test_CGDB_3000_NFRC_102_glazing_system)
{
    SCOPED_TRACE("Begin Test: Exterior Venetian on NFRC 102 as a glazing system.");

    std::filesystem::path clear_3_path(test_dir);
    clear_3_path /= "products";
    clear_3_path /= "CLEAR_3.json";

    std::filesystem::path venetian_path(test_dir);
    venetian_path /= "products";
    venetian_path /= "igsdb_12149.json";

    std::vector<std::variant<std::shared_ptr<OpticsParser::ProductData>,
                             wincalc::Product_Data_Optical_Thermal>>
      products;
    OpticsParser::Parser parser;
    auto clear_3 = parser.parseJSONFile(clear_3_path.string());
    auto venetian = parser.parseJSONFile(venetian_path.string());
    auto converted_venetian = wincalc::convert_to_solid_layer(venetian);
    std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Venetian>(
      converted_venetian.optical_data)
      ->distribution_method = SingleLayerOptics::DistributionMethod::UniformDiffuse;

    products.push_back(converted_venetian);
    products.push_back(clear_3);


    Engine_Gap_Info air_gap(Gases::GasDef::Air, 0.0127);
    std::vector<Engine_Gap_Info> gaps;
    gaps.push_back(air_gap);

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());

    auto bsdf_hemisphere =
      SingleLayerOptics::CBSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);

    Glazing_System glazing_system_u(
      products, gaps, standard, 1.0, 1.0, nfrc_u_environments(), bsdf_hemisphere);
    Glazing_System glazing_system_shgc(
      products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments(), bsdf_hemisphere);

    std::cout << "Start u results for Exterior Venetian on NFRC 102 as a glazing system."
              << std::endl;
    auto u_result = glazing_system_u.u();
    EXPECT_NEAR(u_result, 3.2396918787544662, 1e-6);
    //    EXPECT_NEAR(u_result.t_sol, 0.70329763318245331, 1e-6);
    //    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.096487646903148996, 1e-6);
    //    EXPECT_NEAR(u_result.layer_solar_absorptances[1], 0.072256097417367798, 1e-6);

    auto u_layer_effective_conductivity_solid_layers =
      glazing_system_u.solid_layers_effective_conductivities(Tarcog::ISO15099::System::Uvalue);
    EXPECT_NEAR(u_layer_effective_conductivity_solid_layers[0], 1.8780638541433032, 1e-6);
    EXPECT_NEAR(u_layer_effective_conductivity_solid_layers[1], 1, 1e-6);

    auto u_layer_effective_conductivity_gap_layers =
      glazing_system_u.gap_layers_effective_conductivities(Tarcog::ISO15099::System::Uvalue);
    EXPECT_NEAR(u_layer_effective_conductivity_gap_layers[0], 0.076489623038027671, 1e-6);

    auto u_system_effective_conductivity =
      glazing_system_u.system_effective_conductivity(Tarcog::ISO15099::System::Uvalue);
    EXPECT_NEAR(u_system_effective_conductivity, 0.106426000595093, 1e-6);

    std::cout << "Start shgc results for Exterior Venetian on NFRC 102 as a glazing system."
              << std::endl;
    auto shgc_result = glazing_system_shgc.shgc();
    EXPECT_NEAR(shgc_result, 0.8415736990516316, 1e-6);
    //    EXPECT_NEAR(shgc_result.t_sol, 0.70329763318245331, 1e-6);
    //    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.096487646903148996, 1e-6);
    //    EXPECT_NEAR(shgc_result.layer_solar_absorptances[1], 0.072256097417367798, 1e-6);

    std::cout << "Start solar results for Exterior Venetian on NFRC 102 as a glazing system."
              << std::endl;
    WCE_Optical_Results solar_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_hemispherical,
                0.78968932199996555,
                1e-6);
    // EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_direct, 0.7867, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_hemispherical,
                0.093007851995787302,
                1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_hemispherical,
                0.091859351033763165,
                1e-6);
    // EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_diffuse, 0.786517, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_diffuse, 0.073785, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.front.transmittance.diffuse_diffuse, 0.708676,
    // 1e-6); EXPECT_NEAR(solar_results.system_results.back.transmittance.diffuse_diffuse,
    // 0.63794508795388927, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.front.reflectance.diffuse_diffuse, 0.138815, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.back.reflectance.diffuse_diffuse,
    // 0.14709734859263215, 1e-6);
    EXPECT_NEAR(solar_results.layer_results[0].front.absorptance.direct, 0.03060936143662869, 1e-6);
    EXPECT_NEAR(
      solar_results.layer_results[1].front.absorptance.direct, 0.086693464567618075, 1e-6);
    // EXPECT_NEAR(solar_results.layer_results[0].front.absorptance.diffuse, 0.0306, 1e-6);
    // EXPECT_NEAR(solar_results.absorptances_front.diffuse[1], 0.0867, 1e-6);

    std::cout << "Start photopic results for Exterior Venetian on NFRC 102 as a glazing system."
              << std::endl;
    WCE_Optical_Results photopic_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::PHOTOPIC);

    EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_hemispherical,
                0.85326534421545952,
                1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_direct, 0.8502, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_hemispherical,
                0.10323814248919577,
                1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_hemispherical,
                0.10467287892455968,
                1e-6);
    // EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.front.transmittance.diffuse_diffuse,
    // 0.78935221564455493, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.transmittance.diffuse_diffuse,
    // 0.78935221564455493, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.front.reflectance.diffuse_diffuse,
    // 0.13525667684831866, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.reflectance.diffuse_diffuse,
    // 0.13525667684831866, 1e-6);

    std::cout << "Start uv results for Exterior Venetian on NFRC 102 as a glazing system."
              << std::endl;
    WCE_Optical_Results tuv_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TUV);

    EXPECT_NEAR(
      tuv_results.system_results.front.transmittance.direct_hemispherical, 0.66982889175415, 1e-6);
#    if 0
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_direct, 0.58733483792234731, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_direct, 0.062256898186217748, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_direct, 0.062256898186217748, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.diffuse_diffuse, 0.51679983061465506, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.diffuse_diffuse, 0.51679983061465506, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.diffuse_diffuse, 0.11475903289790138, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.diffuse_diffuse, 0.11475903289790138, 1e-6);

    WCE_Color_Results color_results = glazing_system_u.color();

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.L, 94.666217263591847, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.a, 1.0038394903462700, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.b, 2.3170364131629428, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.G, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.B, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.X, 83.028312591606294, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Y, 86.830285384922604, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Z, 91.092529581183740, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.L, 94.666217263591847, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.a, 1.0038394903462700, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.b, 2.3170364131629428, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.G, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.B, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.X, 83.028312591606294, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Y, 86.830285384922604, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Z, 91.092529581183740, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.L, 32.685672100834950, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.a, -13.456067709025742, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.b, -9.4328475174762723, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.R, 38, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.G, 92, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.B, 101, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.X, 5.7584248621303411, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Y, 7.3931504074726693, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Z, 11.074221343637136, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.L, 32.685672100834950, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.a, -13.456067709025742, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.b, -9.4328475174762723, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.R, 38, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.G, 92, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.B, 101, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.X, 5.7584248621303411, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Y, 7.3931504074726693, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Z, 11.074221343637136, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.L, 91.205381649557779, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.a, 0.81757398116044655, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.b, 2.2108499191388242, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.G, 253, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.B, 249, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.X, 75.404098662194983, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Y, 78.936123385628918, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Z, 82.856762497727459, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.L, 91.205381649557779, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.a, 0.81757398116044655, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.b, 2.2108499191388242, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.G, 253, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.B, 249, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.X, 75.404098662194983, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Y, 78.936123385628918, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Z, 82.856762497727459, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.L, 43.540458616337347, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.a, -9.1523474616345695, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.b, -6.2165296319391494, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.R, 87, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.G, 119, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.B, 125, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.X, 11.523015097588450, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Y, 13.522673394145196, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Z, 17.554745273890433, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.L, 43.540458616337347, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.a, -9.1523474616345695, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.b, -6.2165296319391494, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.R, 87, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.G, 119, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.B, 125, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.X, 11.523015097588450, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Y, 13.522673394145196, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Z, 17.554745273890433, 1e-6);

#    endif
}
#endif

#if 1
TEST_F(TestGlazingSystem, Test_CGDB_18000_NFRC_102_glazing_system)
{
    SCOPED_TRACE("Begin Test: Exterior Perforated on NFRC 102 as a glazing system.");

    std::filesystem::path clear_3_path(test_dir);
    clear_3_path /= "products";
    clear_3_path /= "CLEAR_3.json";

    std::filesystem::path shade_path(test_dir);
    shade_path /= "products";
    shade_path /= "igsdb_12295.json";

    std::vector<std::shared_ptr<OpticsParser::ProductData>> parsed_products;
    OpticsParser::Parser parser;
    auto clear_3 = parser.parseJSONFile(clear_3_path.string());
    auto shade = parser.parseJSONFile(shade_path.string());
    parsed_products.push_back(shade);
    parsed_products.push_back(clear_3);

    auto converted_products = convert_to_solid_layers(parsed_products);

    Engine_Gap_Info air_gap(Gases::GasDef::Air, 0.0127);
    std::vector<Engine_Gap_Info> gaps;
    gaps.push_back(air_gap);

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());

    auto bsdf_hemisphere =
      SingleLayerOptics::CBSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);

    Glazing_System glazing_system_u(
      converted_products, gaps, standard, 1.0, 1.0, nfrc_u_environments(), bsdf_hemisphere);
    Glazing_System glazing_system_shgc(
      converted_products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments(), bsdf_hemisphere);

    std::cout << "Start u results Exterior Perforated on NFRC 102 as a glazing system."
              << std::endl;
    auto u_result = glazing_system_u.u();
    EXPECT_NEAR(u_result, 3.1411595607437874, 1e-6);
    //    EXPECT_NEAR(u_result.t_sol, 0.70329763318245331, 1e-6);
    //    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.096487646903148996, 1e-6);
    //    EXPECT_NEAR(u_result.layer_solar_absorptances[1], 0.072256097417367798, 1e-6);

    auto u_layer_effective_conductivity_solid_layers =
      glazing_system_u.solid_layers_effective_conductivities(Tarcog::ISO15099::System::Uvalue);
    EXPECT_NEAR(u_layer_effective_conductivity_solid_layers[0], 0.107642754912376, 1e-6);
    EXPECT_NEAR(u_layer_effective_conductivity_solid_layers[1], 1, 1e-6);

    auto u_layer_effective_conductivity_gap_layers =
      glazing_system_u.gap_layers_effective_conductivities(Tarcog::ISO15099::System::Uvalue);
    EXPECT_NEAR(u_layer_effective_conductivity_gap_layers[0], 0.0723807141184807, 1e-6);

    auto u_system_effective_conductivity =
      glazing_system_u.system_effective_conductivity(Tarcog::ISO15099::System::Uvalue);
    EXPECT_NEAR(u_system_effective_conductivity, 0.0989552810788155, 1e-6);

    std::cout << "Start shgc results Exterior Perforated on NFRC 102 as a glazing system."
              << std::endl;
    auto shgc_result = glazing_system_shgc.shgc();
    EXPECT_NEAR(shgc_result, 0.34881892160578121, 1e-6);
    //    EXPECT_NEAR(shgc_result.t_sol, 0.70329763318245331, 1e-6);
    //    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.096487646903148996, 1e-6);
    //    EXPECT_NEAR(shgc_result.layer_solar_absorptances[1], 0.072256097417367798, 1e-6);

    std::cout << "Start solar results Exterior Perforated on NFRC 102 as a glazing system."
              << std::endl;
    WCE_Optical_Results solar_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_hemispherical,
                0.315235286951065,
                1e-6);
    // EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_hemispherical, 0.7867,
    // 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_hemispherical,
                0.54342776212029142,
                1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_hemispherical,
                0.27735812445735147,
                1e-6);
    // EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_diffuse, 0.786517, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_diffuse, 0.073785, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.front.transmittance.diffuse_diffuse, 0.708676,
    // 1e-6); EXPECT_NEAR(solar_results.system_results.back.transmittance.diffuse_diffuse,
    // 0.63794508795388927, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.front.reflectance.diffuse_diffuse, 0.138815, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.back.reflectance.diffuse_diffuse,
    // 0.14709734859263215, 1e-6);
    EXPECT_NEAR(solar_results.layer_results[0].front.absorptance.direct, 0.10665923252381561, 1e-6);
    EXPECT_NEAR(solar_results.layer_results[1].front.absorptance.direct, 0.0346778891980648, 1e-6);
    // EXPECT_NEAR(solar_results.layer_results[0].front.absorptance.diffuse, 0.056829, 1e-6);
    // EXPECT_NEAR(solar_results.absorptances_front.diffuse[1], 0.095680, 1e-6);

    std::cout << "Start photopic results Exterior Perforated on NFRC 102 as a glazing system."
              << std::endl;
    WCE_Optical_Results photopic_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::PHOTOPIC);

    EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_hemispherical,
                0.340711891651154,
                1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_direct, 0.8502, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_hemispherical,
                0.559640407562256,
                1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_hemispherical,
                0.316915452480316,
                1e-6);
    // EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.front.transmittance.diffuse_diffuse,
    // 0.78935221564455493, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.transmittance.diffuse_diffuse,
    // 0.78935221564455493, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.front.reflectance.diffuse_diffuse,
    // 0.13525667684831866, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.reflectance.diffuse_diffuse,
    // 0.13525667684831866, 1e-6);

    std::cout << "Start uv results Exterior Perforated on NFRC 102 as a glazing system."
              << std::endl;
    WCE_Optical_Results tuv_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TUV);
    EXPECT_NEAR(
      tuv_results.system_results.front.transmittance.direct_hemispherical, 0.265386551618576, 1e-6);

#    if 0
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_direct, 0.58733483792234731, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_direct, 0.58733483792234731, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_direct, 0.062256898186217748, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_direct, 0.062256898186217748, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.diffuse_diffuse, 0.51679983061465506, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.diffuse_diffuse, 0.51679983061465506, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.diffuse_diffuse, 0.11475903289790138, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.diffuse_diffuse, 0.11475903289790138, 1e-6);

    WCE_Color_Results color_results = glazing_system_u.color();

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.L, 94.666217263591847, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.a, 1.0038394903462700, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.b, 2.3170364131629428, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.G, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.B, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.X, 83.028312591606294, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Y, 86.830285384922604, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Z, 91.092529581183740, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.L, 94.666217263591847, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.a, 1.0038394903462700, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.b, 2.3170364131629428, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.G, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.B, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.X, 83.028312591606294, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Y, 86.830285384922604, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Z, 91.092529581183740, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.L, 32.685672100834950, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.a, -13.456067709025742, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.b, -9.4328475174762723, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.R, 38, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.G, 92, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.B, 101, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.X, 5.7584248621303411, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Y, 7.3931504074726693, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Z, 11.074221343637136, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.L, 32.685672100834950, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.a, -13.456067709025742, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.b, -9.4328475174762723, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.R, 38, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.G, 92, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.B, 101, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.X, 5.7584248621303411, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Y, 7.3931504074726693, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Z, 11.074221343637136, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.L, 91.205381649557779, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.a, 0.81757398116044655, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.b, 2.2108499191388242, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.G, 253, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.B, 249, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.X, 75.404098662194983, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Y, 78.936123385628918, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Z, 82.856762497727459, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.L, 91.205381649557779, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.a, 0.81757398116044655, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.b, 2.2108499191388242, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.G, 253, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.B, 249, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.X, 75.404098662194983, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Y, 78.936123385628918, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Z, 82.856762497727459, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.L, 43.540458616337347, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.a, -9.1523474616345695, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.b, -6.2165296319391494, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.R, 87, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.G, 119, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.B, 125, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.X, 11.523015097588450, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Y, 13.522673394145196, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Z, 17.554745273890433, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.L, 43.540458616337347, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.a, -9.1523474616345695, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.b, -6.2165296319391494, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.R, 87, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.G, 119, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.B, 125, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.X, 11.523015097588450, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Y, 13.522673394145196, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Z, 17.554745273890433, 1e-6);
#    endif
}
#endif

#if 1
TEST_F(TestGlazingSystem, Test_User_Woven_NFRC_102_glazing_system)
{
    SCOPED_TRACE("Begin Test: Exterior user-defined woven shade on NFRC 102 as a glazing system.");

    std::filesystem::path clear_3_path(test_dir);
    clear_3_path /= "products";
    clear_3_path /= "CLEAR_3.json";


    // Use the same material as in the venetian test above for consistancy with WCE tests
    std::filesystem::path venetian_material_path(test_dir);
    venetian_material_path /= "products";
    venetian_material_path /= "igsdb_12852.json";

    std::vector<std::shared_ptr<OpticsParser::ProductData>> parsed_products;
    OpticsParser::Parser parser;
    auto clear_3 = parser.parseJSONFile(clear_3_path.string());
    auto shade_material = parser.parseJSONFile(venetian_material_path.string());
    auto shade_geometry = std::make_shared<OpticsParser::WovenGeometry>(0.002, 0.003, 0.002);
    auto shade_composition_info = std::shared_ptr<OpticsParser::CompositionInformation>(
      new OpticsParser::CompositionInformation{shade_material, shade_geometry});
    OpticsParser::ProductData shade_layer_info("User Woven Shade", "shading", "User Manufacturer");
    auto woven_shade =
      std::make_shared<OpticsParser::ComposedProductData>(shade_layer_info, shade_composition_info);

    parsed_products.push_back(woven_shade);
    parsed_products.push_back(clear_3);

    auto converted_products = convert_to_solid_layers(parsed_products);

    Engine_Gap_Info air_gap(Gases::GasDef::Air, 0.0127);
    std::vector<Engine_Gap_Info> gaps;
    gaps.push_back(air_gap);

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Optical_Standard standard = load_optical_standard(standard_path.string());

    auto bsdf_hemisphere =
      SingleLayerOptics::CBSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);

    Glazing_System glazing_system_u(
      converted_products, gaps, standard, 1.0, 1.0, nfrc_u_environments(), bsdf_hemisphere);
    Glazing_System glazing_system_shgc(
      converted_products, gaps, standard, 1.0, 1.0, nfrc_shgc_environments(), bsdf_hemisphere);

    std::cout << "Start u results exterior woven on NFRC 102 as a glazing system." << std::endl;
    auto u_result = glazing_system_u.u();
    EXPECT_NEAR(u_result, 2.9148823179439018, 1e-6);
    //    EXPECT_NEAR(u_result.t_sol, 0.70329763318245331, 1e-6);
    //    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.096487646903148996, 1e-6);
    //    EXPECT_NEAR(u_result.layer_solar_absorptances[1], 0.072256097417367798, 1e-6);

    auto u_layer_effective_conductivity_solid_layers =
      glazing_system_u.solid_layers_effective_conductivities(Tarcog::ISO15099::System::Uvalue);
    EXPECT_NEAR(u_layer_effective_conductivity_solid_layers[0], 146.74330655757495, 1e-6);
    EXPECT_NEAR(u_layer_effective_conductivity_solid_layers[1], 1, 1e-6);

    auto u_layer_effective_conductivity_gap_layers =
      glazing_system_u.gap_layers_effective_conductivities(Tarcog::ISO15099::System::Uvalue);
    EXPECT_NEAR(u_layer_effective_conductivity_gap_layers[0], 0.070327899269384583, 1e-6);

    auto u_system_effective_conductivity =
      glazing_system_u.system_effective_conductivity(Tarcog::ISO15099::System::Uvalue);
    EXPECT_NEAR(u_system_effective_conductivity, 0.10040701238466257, 1e-6);

    std::cout << "Start shgc results exterior woven on NFRC 102 as a glazing system." << std::endl;
    auto shgc_result = glazing_system_shgc.shgc();
    EXPECT_NEAR(shgc_result, 0.14718304092197346, 1e-6);
    //    EXPECT_NEAR(shgc_result.t_sol, 0.70329763318245331, 1e-6);
    //    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.096487646903148996, 1e-6);
    //    EXPECT_NEAR(shgc_result.layer_solar_absorptances[1], 0.072256097417367798, 1e-6);

    std::cout << "Start solar results Exterior Perforated on NFRC 102 as a glazing system."
              << std::endl;
    WCE_Optical_Results solar_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_hemispherical,
                0.098171536948485016,
                1e-6);
    // EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_hemispherical, 0.7867,
    // 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_hemispherical,
                0.6065899265825383,
                1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_hemispherical,
                0.50146963524036026,
                1e-6);
    // EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_diffuse, 0.786517, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_diffuse, 0.073785, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.front.transmittance.diffuse_diffuse, 0.708676,
    // 1e-6); EXPECT_NEAR(solar_results.system_results.back.transmittance.diffuse_diffuse,
    // 0.63794508795388927, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.front.reflectance.diffuse_diffuse, 0.138815, 1e-6);
    // EXPECT_NEAR(solar_results.system_results.back.reflectance.diffuse_diffuse,
    // 0.14709734859263215, 1e-6);
    EXPECT_NEAR(solar_results.layer_results[0].front.absorptance.direct, 0.28441079217097104, 1e-6);
    EXPECT_NEAR(
      solar_results.layer_results[1].front.absorptance.direct, 0.010827744298005591, 1e-6);
    // EXPECT_NEAR(solar_results.layer_results[0].front.absorptance.diffuse, 0.056829, 1e-6);
    // EXPECT_NEAR(solar_results.absorptances_front.diffuse[1], 0.095680, 1e-6);

    std::cout << "Start photopic results exterior woven on NFRC 102 as a glazing system."
              << std::endl;
    WCE_Optical_Results photopic_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::PHOTOPIC);

    EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_hemispherical,
                0.10723787565441972,
                1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_direct, 0.8502, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_hemispherical,
                0.66231650848248969,
                1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_hemispherical,
                0.63235320261454742,
                1e-6);
    // EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.front.transmittance.diffuse_diffuse,
    // 0.78935221564455493, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.transmittance.diffuse_diffuse,
    // 0.78935221564455493, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.front.reflectance.diffuse_diffuse,
    // 0.13525667684831866, 1e-6);
    // EXPECT_NEAR(photopic_results.system_results.back.reflectance.diffuse_diffuse,
    // 0.13525667684831866, 1e-6);

    std::cout << "Start uv results exterior woven on NFRC 102 as a glazing system." << std::endl;
    WCE_Optical_Results tuv_results =
      glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TUV);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_hemispherical,
                0.079820547642344905,
                1e-6);

#    if 0
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_direct, 0.58733483792234731, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_direct, 0.58733483792234731, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_direct, 0.062256898186217748, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_direct, 0.062256898186217748, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.diffuse_diffuse, 0.51679983061465506, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.diffuse_diffuse, 0.51679983061465506, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.diffuse_diffuse, 0.11475903289790138, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.diffuse_diffuse, 0.11475903289790138, 1e-6);

    WCE_Color_Results color_results = glazing_system_u.color();

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.L, 94.666217263591847, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.a, 1.0038394903462700, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.b, 2.3170364131629428, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.G, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.B, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.X, 83.028312591606294, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Y, 86.830285384922604, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Z, 91.092529581183740, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.L, 94.666217263591847, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.a, 1.0038394903462700, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.b, 2.3170364131629428, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.G, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.B, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.X, 83.028312591606294, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Y, 86.830285384922604, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Z, 91.092529581183740, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.L, 32.685672100834950, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.a, -13.456067709025742, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.b, -9.4328475174762723, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.R, 38, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.G, 92, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.B, 101, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.X, 5.7584248621303411, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Y, 7.3931504074726693, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Z, 11.074221343637136, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.L, 32.685672100834950, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.a, -13.456067709025742, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.b, -9.4328475174762723, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.R, 38, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.G, 92, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.B, 101, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.X, 5.7584248621303411, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Y, 7.3931504074726693, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Z, 11.074221343637136, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.L, 91.205381649557779, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.a, 0.81757398116044655, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.b, 2.2108499191388242, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.G, 253, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.B, 249, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.X, 75.404098662194983, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Y, 78.936123385628918, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Z, 82.856762497727459, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.L, 91.205381649557779, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.a, 0.81757398116044655, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.b, 2.2108499191388242, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.G, 253, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.B, 249, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.X, 75.404098662194983, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Y, 78.936123385628918, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Z, 82.856762497727459, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.L, 43.540458616337347, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.a, -9.1523474616345695, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.b, -6.2165296319391494, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.R, 87, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.G, 119, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.B, 125, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.X, 11.523015097588450, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Y, 13.522673394145196, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Z, 17.554745273890433, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.L, 43.540458616337347, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.a, -9.1523474616345695, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.b, -6.2165296319391494, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.R, 87, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.G, 119, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.B, 125, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.X, 11.523015097588450, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Y, 13.522673394145196, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Z, 17.554745273890433, 1e-6);
#    endif
}
#endif

#if 0

TEST_F(TestGlazingSystem, Test_NFRC_9023_glazing_system)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 9023 as a glazing system.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "9023.json";

    std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
    auto product = OpticsParser::parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Standard standard = load_optical_standard(standard_path.string());

    Glazing_System glazing_system(products, gaps, standard, 1.0, 1.0);

    Thermal_Result u_result = glazing_system_u.u();
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-6);
    EXPECT_NEAR(u_result.t_sol, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-6);

    Thermal_Result shgc_result = glazing_system_u.shgc();
    EXPECT_NEAR(shgc_result.result, 0.86063515079923925, 1e-6);
    EXPECT_NEAR(shgc_result.t_sol, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-6);

    WCE_Optical_Results photopic_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::PHOTOPIC);

    EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_direct, 0.89925998453884770, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_direct, 0.89925998453884770, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_direct, 0.082563172831176687, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_direct, 0.082563521272277285, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.transmittance.diffuse_diffuse, 0.82185034176842187, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.diffuse_diffuse, 0.82185034176842187, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.diffuse_diffuse, 0.14757793943808908, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.diffuse_diffuse, 0.14757829179791318, 1e-6);

    WCE_Optical_Results solar_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_direct, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_direct, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_direct, 0.074765048919287855, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_direct, 0.074855794767823960, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.transmittance.diffuse_diffuse, 0.75334837568243473, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.diffuse_diffuse, 0.75334837568243473, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.diffuse_diffuse, 0.13582160214429620, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.diffuse_diffuse, 0.13591473635815252, 1e-6);

    WCE_Optical_Results spf_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SPF);

    EXPECT_NEAR(spf_results.system_results.front.transmittance.direct_direct, 0.092847372531649014, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.direct_direct, 0.092847372531649014, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.direct_direct, 0.049777244859762357, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.direct_direct, 0.050726658558665656, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.transmittance.diffuse_diffuse, 0.078035374127229148, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.diffuse_diffuse, 0.078035374127229148, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.diffuse_diffuse, 0.098178562240941875, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.diffuse_diffuse, 0.099197614848102633, 1e-6);

    WCE_Optical_Results tdw_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TDW);

    EXPECT_NEAR(tdw_results.system_results.front.transmittance.direct_direct, 0.88455027707939504, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.transmittance.direct_direct, 0.88455027707939504, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.reflectance.direct_direct, 0.082076383610412737, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.reflectance.direct_direct, 0.082103867417672005, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.transmittance.diffuse_diffuse, 0.80695444628614621, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.transmittance.diffuse_diffuse, 0.80695444628614621, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.reflectance.diffuse_diffuse, 0.14632052149223410, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.reflectance.diffuse_diffuse, 0.14634816212636417, 1e-6);

    WCE_Optical_Results tkr_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TKR);

    EXPECT_NEAR(tkr_results.system_results.front.transmittance.direct_direct, 0.87316376513614979, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.transmittance.direct_direct, 0.87316376513614979, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.reflectance.direct_direct, 0.083803337959073798, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.reflectance.direct_direct, 0.083881876534500452, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.transmittance.diffuse_diffuse, 0.79627298835580007, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.transmittance.diffuse_diffuse, 0.79627298835580007, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.reflectance.diffuse_diffuse, 0.14771062629560355, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.reflectance.diffuse_diffuse, 0.14778961302433385, 1e-6);

    WCE_Optical_Results tuv_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TUV);

    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_direct, 0.71627812708040550, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_direct, 0.71627812708040550, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_direct, 0.075711443154079433, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_direct, 0.076467194319106452, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.diffuse_diffuse, 0.64123952691013475, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.diffuse_diffuse, 0.64123952691013475, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.diffuse_diffuse, 0.13353575295457998, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.diffuse_diffuse, 0.13429242840518707, 1e-6);

    WCE_Color_Results color_results = glazing_system_u.color();

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.a, -0.95083962881142847, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.b, 0.17036999848574208, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.L, 95.972303072706140, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.G, 225, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.B, 170, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.X, 113.43654324410734, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Y, 89.941031751191431, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Z, 42.099918055206423, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.a, -0.95083962881142847, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.b, 0.17036999848574208, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.L, 95.972303072706140, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.G, 225, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.B, 170, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.X, 113.43654324410734, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Y, 89.941031751191431, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Z, 42.099918055206423, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.a, -0.57014300716473620, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.b, -0.75762082784675000, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.L, 34.538795714754407, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.R, 133, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.G, 74, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.B, 54, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.X, 10.409998896409247, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Y, 8.2699083985629223, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Z, 3.9833970997487507, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.a, -0.56970898770367095, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.b, -0.75752102707072888, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.L, 34.538853599204501, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.R, 133, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.G, 74, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.B, 54, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.X, 10.410097145076767, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Y, 8.2699368143358800, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Z, 3.9833970997487507, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.a, -1.0720003464831374, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.b, 0.16624816089467753, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.L, 92.664205431775031, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.G, 217, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.B, 164, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.X, 103.57730796869151, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Y, 82.202600827001675, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Z, 38.477123654213784, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.a, -1.0720003464831374, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.b, 0.16624816089467753, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.L, 92.664205431775031, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.G, 217, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.B, 164, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.X, 103.57730796869151, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Y, 82.202600827001675, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Z, 38.477123654213784, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.a, -0.61072432920805664, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.b, -0.47392296711112891, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.L, 45.320875340223516, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.R, 174, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.G, 98, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.B, 73, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.X, 18.612365928660576, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Y, 14.772400359833146, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Z, 7.0267089564314791, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.a, -0.61042649780562730, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.b, -0.47385441662115557, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.L, 45.320915099507701, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.R, 174, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.G, 98, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.B, 73, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.X, 18.612465279823397, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Y, 14.772429094278383, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Z, 7.0267089564314791, 1e-6);
}

TEST_F(TestGlazingSystem, Test_NFRC_21467_glazing_system)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 21467 as a glazing system.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "21467.json";

    std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
    auto product = OpticsParser::parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Standard standard = load_optical_standard(standard_path.string());

    Glazing_System glazing_system(products, gaps, standard, 1.0, 1.0);

    Thermal_Result u_result = glazing_system_u.u();
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-6);
    EXPECT_NEAR(u_result.t_sol, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-6);

    Thermal_Result shgc_result = glazing_system_u.shgc();
    EXPECT_NEAR(shgc_result.result, 0.86063515079923925, 1e-6);
    EXPECT_NEAR(shgc_result.t_sol, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-6);

    WCE_Optical_Results solar_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_direct, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_direct, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_direct, 0.074765048919287855, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_direct, 0.074855794767823960, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.transmittance.diffuse_diffuse, 0.75334837568243473, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.diffuse_diffuse, 0.75334837568243473, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.diffuse_diffuse, 0.13582160214429620, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.diffuse_diffuse, 0.13591473635815252, 1e-6);

    WCE_Optical_Results photopic_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::PHOTOPIC);

    EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_direct, 0.89925998453884770, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_direct, 0.89925998453884770, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_direct, 0.082563172831176687, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_direct, 0.082563521272277285, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.transmittance.diffuse_diffuse, 0.82185034176842187, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.diffuse_diffuse, 0.82185034176842187, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.diffuse_diffuse, 0.14757793943808908, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.diffuse_diffuse, 0.14757829179791318, 1e-6);

    WCE_Optical_Results spf_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SPF);

    EXPECT_NEAR(spf_results.system_results.front.transmittance.direct_direct, 0.092847372531649014, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.direct_direct, 0.092847372531649014, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.direct_direct, 0.049777244859762357, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.direct_direct, 0.050726658558665656, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.transmittance.diffuse_diffuse, 0.078035374127229148, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.diffuse_diffuse, 0.078035374127229148, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.diffuse_diffuse, 0.098178562240941875, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.diffuse_diffuse, 0.099197614848102633, 1e-6);

    WCE_Optical_Results tdw_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TDW);

    EXPECT_NEAR(tdw_results.system_results.front.transmittance.direct_direct, 0.88455027707939504, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.transmittance.direct_direct, 0.88455027707939504, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.reflectance.direct_direct, 0.082076383610412737, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.reflectance.direct_direct, 0.082103867417672005, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.transmittance.diffuse_diffuse, 0.80695444628614621, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.transmittance.diffuse_diffuse, 0.80695444628614621, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.reflectance.diffuse_diffuse, 0.14632052149223410, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.reflectance.diffuse_diffuse, 0.14634816212636417, 1e-6);

    WCE_Optical_Results tkr_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TKR);

    EXPECT_NEAR(tkr_results.system_results.front.transmittance.direct_direct, 0.87316376513614979, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.transmittance.direct_direct, 0.87316376513614979, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.reflectance.direct_direct, 0.083803337959073798, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.reflectance.direct_direct, 0.083881876534500452, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.transmittance.diffuse_diffuse, 0.79627298835580007, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.transmittance.diffuse_diffuse, 0.79627298835580007, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.reflectance.diffuse_diffuse, 0.14771062629560355, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.reflectance.diffuse_diffuse, 0.14778961302433385, 1e-6);

    WCE_Optical_Results tuv_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TUV);

    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_direct, 0.71627812708040550, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_direct, 0.71627812708040550, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_direct, 0.075711443154079433, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_direct, 0.076467194319106452, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.diffuse_diffuse, 0.64123952691013475, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.diffuse_diffuse, 0.64123952691013475, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.diffuse_diffuse, 0.13353575295457998, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.diffuse_diffuse, 0.13429242840518707, 1e-6);

    WCE_Color_Results color_results = glazing_system_u.color();

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.a, -0.95083962881142847, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.b, 0.17036999848574208, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.L, 95.972303072706140, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.G, 225, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.B, 170, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.X, 113.43654324410734, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Y, 89.941031751191431, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Z, 42.099918055206423, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.a, -0.95083962881142847, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.b, 0.17036999848574208, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.L, 95.972303072706140, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.G, 225, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.B, 170, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.X, 113.43654324410734, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Y, 89.941031751191431, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Z, 42.099918055206423, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.a, -0.57014300716473620, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.b, -0.75762082784675000, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.L, 34.538795714754407, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.R, 133, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.G, 74, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.B, 54, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.X, 10.409998896409247, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Y, 8.2699083985629223, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Z, 3.9833970997487507, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.a, -0.56970898770367095, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.b, -0.75752102707072888, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.L, 34.538853599204501, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.R, 133, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.G, 74, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.B, 54, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.X, 10.410097145076767, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Y, 8.2699368143358800, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Z, 3.9833970997487507, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.a, -1.0720003464831374, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.b, 0.16624816089467753, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.L, 92.664205431775031, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.G, 217, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.B, 164, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.X, 103.57730796869151, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Y, 82.202600827001675, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Z, 38.477123654213784, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.a, -1.0720003464831374, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.b, 0.16624816089467753, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.L, 92.664205431775031, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.G, 217, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.B, 164, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.X, 103.57730796869151, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Y, 82.202600827001675, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Z, 38.477123654213784, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.a, -0.61072432920805664, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.b, -0.47392296711112891, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.L, 45.320875340223516, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.R, 174, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.G, 98, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.B, 73, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.X, 18.612365928660576, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Y, 14.772400359833146, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Z, 7.0267089564314791, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.a, -0.61042649780562730, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.b, -0.47385441662115557, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.L, 45.320915099507701, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.R, 174, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.G, 98, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.B, 73, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.X, 18.612465279823397, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Y, 14.772429094278383, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Z, 7.0267089564314791, 1e-6);
}

TEST_F(TestGlazingSystem, Test_NFRC_9223_glazing_system)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 9223 as a glazing system.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "9223.json";

    std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
    auto product = OpticsParser::parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Standard standard = load_optical_standard(standard_path.string());

    Glazing_System glazing_system(products, gaps, standard, 1.0, 1.0);

    Thermal_Result u_result = glazing_system_u.u();
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-6);
    EXPECT_NEAR(u_result.t_sol, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-6);

    Thermal_Result shgc_result = glazing_system_u.shgc();
    EXPECT_NEAR(shgc_result.result, 0.86063515079923925, 1e-6);
    EXPECT_NEAR(shgc_result.t_sol, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-6);

    WCE_Optical_Results solar_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_direct, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_direct, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_direct, 0.074765048919287855, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_direct, 0.074855794767823960, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.transmittance.diffuse_diffuse, 0.75334837568243473, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.diffuse_diffuse, 0.75334837568243473, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.diffuse_diffuse, 0.13582160214429620, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.diffuse_diffuse, 0.13591473635815252, 1e-6);

    WCE_Optical_Results photopic_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::PHOTOPIC);

    EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_direct, 0.89925998453884770, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_direct, 0.89925998453884770, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_direct, 0.082563172831176687, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_direct, 0.082563521272277285, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.transmittance.diffuse_diffuse, 0.82185034176842187, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.diffuse_diffuse, 0.82185034176842187, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.diffuse_diffuse, 0.14757793943808908, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.diffuse_diffuse, 0.14757829179791318, 1e-6);

    WCE_Optical_Results spf_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SPF);

    EXPECT_NEAR(spf_results.system_results.front.transmittance.direct_direct, 0.092847372531649014, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.direct_direct, 0.092847372531649014, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.direct_direct, 0.049777244859762357, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.direct_direct, 0.050726658558665656, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.transmittance.diffuse_diffuse, 0.078035374127229148, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.diffuse_diffuse, 0.078035374127229148, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.diffuse_diffuse, 0.098178562240941875, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.diffuse_diffuse, 0.099197614848102633, 1e-6);

    WCE_Optical_Results tdw_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TDW);

    EXPECT_NEAR(tdw_results.system_results.front.transmittance.direct_direct, 0.88455027707939504, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.transmittance.direct_direct, 0.88455027707939504, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.reflectance.direct_direct, 0.082076383610412737, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.reflectance.direct_direct, 0.082103867417672005, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.transmittance.diffuse_diffuse, 0.80695444628614621, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.transmittance.diffuse_diffuse, 0.80695444628614621, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.reflectance.diffuse_diffuse, 0.14632052149223410, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.reflectance.diffuse_diffuse, 0.14634816212636417, 1e-6);

    WCE_Optical_Results tkr_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TKR);

    EXPECT_NEAR(tkr_results.system_results.front.transmittance.direct_direct, 0.87316376513614979, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.transmittance.direct_direct, 0.87316376513614979, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.reflectance.direct_direct, 0.083803337959073798, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.reflectance.direct_direct, 0.083881876534500452, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.transmittance.diffuse_diffuse, 0.79627298835580007, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.transmittance.diffuse_diffuse, 0.79627298835580007, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.reflectance.diffuse_diffuse, 0.14771062629560355, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.reflectance.diffuse_diffuse, 0.14778961302433385, 1e-6);

    WCE_Optical_Results tuv_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TUV);

    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_direct, 0.71627812708040550, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_direct, 0.71627812708040550, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_direct, 0.075711443154079433, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_direct, 0.076467194319106452, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.diffuse_diffuse, 0.64123952691013475, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.diffuse_diffuse, 0.64123952691013475, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.diffuse_diffuse, 0.13353575295457998, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.diffuse_diffuse, 0.13429242840518707, 1e-6);

    WCE_Color_Results color_results = glazing_system_u.color();

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.a, -0.95083962881142847, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.b, 0.17036999848574208, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.L, 95.972303072706140, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.G, 225, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.B, 170, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.X, 113.43654324410734, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Y, 89.941031751191431, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Z, 42.099918055206423, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.a, -0.95083962881142847, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.b, 0.17036999848574208, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.L, 95.972303072706140, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.G, 225, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.B, 170, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.X, 113.43654324410734, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Y, 89.941031751191431, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Z, 42.099918055206423, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.a, -0.57014300716473620, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.b, -0.75762082784675000, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.L, 34.538795714754407, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.R, 133, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.G, 74, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.B, 54, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.X, 10.409998896409247, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Y, 8.2699083985629223, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Z, 3.9833970997487507, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.a, -0.56970898770367095, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.b, -0.75752102707072888, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.L, 34.538853599204501, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.R, 133, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.G, 74, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.B, 54, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.X, 10.410097145076767, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Y, 8.2699368143358800, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Z, 3.9833970997487507, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.a, -1.0720003464831374, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.b, 0.16624816089467753, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.L, 92.664205431775031, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.G, 217, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.B, 164, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.X, 103.57730796869151, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Y, 82.202600827001675, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Z, 38.477123654213784, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.a, -1.0720003464831374, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.b, 0.16624816089467753, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.L, 92.664205431775031, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.G, 217, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.B, 164, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.X, 103.57730796869151, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Y, 82.202600827001675, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Z, 38.477123654213784, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.a, -0.61072432920805664, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.b, -0.47392296711112891, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.L, 45.320875340223516, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.R, 174, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.G, 98, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.B, 73, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.X, 18.612365928660576, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Y, 14.772400359833146, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Z, 7.0267089564314791, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.a, -0.61042649780562730, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.b, -0.47385441662115557, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.L, 45.320915099507701, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.R, 174, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.G, 98, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.B, 73, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.X, 18.612465279823397, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Y, 14.772429094278383, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Z, 7.0267089564314791, 1e-6);
}

TEST_F(TestGlazingSystem, Test_NFRC_6046_glazing_system)
{
    SCOPED_TRACE("Begin Test: Single clear NFRC 6046 as a glazing system.");

    std::filesystem::path product_path(test_dir);
    product_path /= "products";
    product_path /= "6046.json";

    std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
    auto product = OpticsParser::parseJSONFile(product_path.string());
    products.push_back(product);

    std::vector<Gap_Data> gaps;

    std::filesystem::path standard_path(test_dir);
    standard_path /= "standards";
    standard_path /= "W5_NFRC_2003.std";
    Standard standard = load_optical_standard(standard_path.string());

    Glazing_System glazing_system(products, gaps, standard, 1.0, 1.0);

    Thermal_Result u_result = glazing_system_u.u();
    EXPECT_NEAR(u_result.result, 5.9125145552954441, 1e-6);
    EXPECT_NEAR(u_result.t_sol, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(u_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-6);

    Thermal_Result shgc_result = glazing_system_u.shgc();
    EXPECT_NEAR(shgc_result.result, 0.86063515079923925, 1e-6);
    EXPECT_NEAR(shgc_result.t_sol, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(shgc_result.layer_solar_absorptances[0], 0.091383936352422049, 1e-6);

    WCE_Optical_Results solar_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SOLAR);

    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_direct, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_direct, 0.83385101472829093, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_direct, 0.074765048919287855, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_direct, 0.074855794767823960, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.transmittance.diffuse_diffuse, 0.75334837568243473, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.transmittance.diffuse_diffuse, 0.75334837568243473, 1e-6);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.diffuse_diffuse, 0.13582160214429620, 1e-6);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.diffuse_diffuse, 0.13591473635815252, 1e-6);

    WCE_Optical_Results photopic_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::PHOTOPIC);

    EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_direct, 0.89925998453884770, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_direct, 0.89925998453884770, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_direct, 0.082563172831176687, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_direct, 0.082563521272277285, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.transmittance.diffuse_diffuse, 0.82185034176842187, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.transmittance.diffuse_diffuse, 0.82185034176842187, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.front.reflectance.diffuse_diffuse, 0.14757793943808908, 1e-6);
    EXPECT_NEAR(photopic_results.system_results.back.reflectance.diffuse_diffuse, 0.14757829179791318, 1e-6);

    WCE_Optical_Results spf_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::SPF);

    EXPECT_NEAR(spf_results.system_results.front.transmittance.direct_direct, 0.092847372531649014, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.direct_direct, 0.092847372531649014, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.direct_direct, 0.049777244859762357, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.direct_direct, 0.050726658558665656, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.transmittance.diffuse_diffuse, 0.078035374127229148, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.diffuse_diffuse, 0.078035374127229148, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.diffuse_diffuse, 0.098178562240941875, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.diffuse_diffuse, 0.099197614848102633, 1e-6);

    WCE_Optical_Results tdw_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TDW);

    EXPECT_NEAR(tdw_results.system_results.front.transmittance.direct_direct, 0.88455027707939504, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.transmittance.direct_direct, 0.88455027707939504, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.reflectance.direct_direct, 0.082076383610412737, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.reflectance.direct_direct, 0.082103867417672005, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.transmittance.diffuse_diffuse, 0.80695444628614621, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.transmittance.diffuse_diffuse, 0.80695444628614621, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.front.reflectance.diffuse_diffuse, 0.14632052149223410, 1e-6);
    EXPECT_NEAR(tdw_results.system_results.back.reflectance.diffuse_diffuse, 0.14634816212636417, 1e-6);

    WCE_Optical_Results tkr_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TKR);

    EXPECT_NEAR(tkr_results.system_results.front.transmittance.direct_direct, 0.87316376513614979, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.transmittance.direct_direct, 0.87316376513614979, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.reflectance.direct_direct, 0.083803337959073798, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.reflectance.direct_direct, 0.083881876534500452, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.transmittance.diffuse_diffuse, 0.79627298835580007, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.transmittance.diffuse_diffuse, 0.79627298835580007, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.front.reflectance.diffuse_diffuse, 0.14771062629560355, 1e-6);
    EXPECT_NEAR(tkr_results.system_results.back.reflectance.diffuse_diffuse, 0.14778961302433385, 1e-6);

    WCE_Optical_Results tuv_results = glazing_system_u.optical_method_results(Optical_Standard_Method_Type::TUV);

    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_direct, 0.71627812708040550, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_direct, 0.71627812708040550, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_direct, 0.075711443154079433, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_direct, 0.076467194319106452, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.transmittance.diffuse_diffuse, 0.64123952691013475, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.transmittance.diffuse_diffuse, 0.64123952691013475, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.front.reflectance.diffuse_diffuse, 0.13353575295457998, 1e-6);
    EXPECT_NEAR(tuv_results.system_results.back.reflectance.diffuse_diffuse, 0.13429242840518707, 1e-6);

    WCE_Color_Results color_results = glazing_system_u.color();

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.a, -0.95083962881142847, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.b, 0.17036999848574208, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.lab.L, 95.972303072706140, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.G, 225, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.rgb.B, 170, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.X, 113.43654324410734, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Y, 89.941031751191431, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_direct.trichromatic.Z, 42.099918055206423, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.a, -0.95083962881142847, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.b, 0.17036999848574208, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.lab.L, 95.972303072706140, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.G, 225, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.rgb.B, 170, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.X, 113.43654324410734, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Y, 89.941031751191431, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_direct.trichromatic.Z, 42.099918055206423, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.a, -0.57014300716473620, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.b, -0.75762082784675000, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.lab.L, 34.538795714754407, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.R, 133, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.G, 74, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.rgb.B, 54, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.X, 10.409998896409247, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Y, 8.2699083985629223, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_direct.trichromatic.Z, 3.9833970997487507, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.a, -0.56970898770367095, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.b, -0.75752102707072888, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.lab.L, 34.538853599204501, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.R, 133, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.G, 74, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.rgb.B, 54, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.X, 10.410097145076767, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Y, 8.2699368143358800, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_direct.trichromatic.Z, 3.9833970997487507, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.a, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.b, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.lab.L, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.R, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.G, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.rgb.B, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.X, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Y, 0.0, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.direct_diffuse.trichromatic.Z, 0.0, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.a, -1.0720003464831374, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.b, 0.16624816089467753, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.lab.L, 92.664205431775031, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.G, 217, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.rgb.B, 164, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.X, 103.57730796869151, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Y, 82.202600827001675, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.transmittance.diffuse_diffuse.trichromatic.Z, 38.477123654213784, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.a, -1.0720003464831374, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.b, 0.16624816089467753, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.lab.L, 92.664205431775031, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.R, 255, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.G, 217, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.rgb.B, 164, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.X, 103.57730796869151, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Y, 82.202600827001675, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.transmittance.diffuse_diffuse.trichromatic.Z, 38.477123654213784, 1e-6);

    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.a, -0.61072432920805664, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.b, -0.47392296711112891, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.lab.L, 45.320875340223516, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.R, 174, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.G, 98, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.rgb.B, 73, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.X, 18.612365928660576, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Y, 14.772400359833146, 1e-6);
    EXPECT_NEAR(color_results.system_results.front.reflectance.diffuse_diffuse.trichromatic.Z, 7.0267089564314791, 1e-6);

    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.a, -0.61042649780562730, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.b, -0.47385441662115557, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.lab.L, 45.320915099507701, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.R, 174, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.G, 98, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.rgb.B, 73, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.X, 18.612465279823397, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Y, 14.772429094278383, 1e-6);
    EXPECT_NEAR(color_results.system_results.back.reflectance.diffuse_diffuse.trichromatic.Z, 7.0267089564314791, 1e-6);
}
#endif
