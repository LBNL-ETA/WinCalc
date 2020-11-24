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

class TestNFRC1022011SA1 : public testing::Test
{
protected:
	std::shared_ptr<Glazing_System> glazing_system_u;
	std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path clear_3_path(test_dir);
        clear_3_path /= "products";
        clear_3_path /= "CLEAR_3.json";

        std::filesystem::path bsdf_path(test_dir);
        bsdf_path /= "products";
        bsdf_path /=
          "2011-SA1_same_solar_and_visible_only_normal.XML";   //"2011-SA1_same_solar_and_visible.XML";

        std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
        auto clear_3 = OpticsParser::parseJSONFile(clear_3_path.string());
        products.push_back(clear_3);

        auto bsdf = OpticsParser::parseBSDFXMLFile(bsdf_path.string());
        products.push_back(bsdf);

        Engine_Gap_Info air_gap(Gases::GasDef::Air, 0.0127);
        std::vector<Engine_Gap_Info> gaps;
        gaps.push_back(air_gap);

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        auto bsdf_hemisphere =
          SingleLayerOptics::CBSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Small);

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, nfrc_u_environments(), bsdf_hemisphere);
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, nfrc_shgc_environments(), bsdf_hemisphere);
    }
};

TEST_F(TestNFRC1022011SA1, Test_U)
{
    auto u_result = glazing_system_u->u();
    EXPECT_NEAR(u_result, 3.13679051, 1e-6);

    auto u_layer_temperatues_u =
      glazing_system_u->layer_temperatures(Tarcog::ISO15099::System::Uvalue);

    EXPECT_NEAR(u_layer_temperatues_u[0], 263.01621462125490, 1e-6);
    EXPECT_NEAR(u_layer_temperatues_u[1], 263.71904705715122, 1e-6);

    auto u_layer_temperatues_shgc =
      glazing_system_u->layer_temperatures(Tarcog::ISO15099::System::SHGC);

    EXPECT_NEAR(u_layer_temperatues_shgc[0], 263.01621462383559, 1e-6);
    EXPECT_NEAR(u_layer_temperatues_shgc[1], 263.71904705531443, 1e-6);
}

TEST_F(TestNFRC1022011SA1, Test_SHGC)
{
    auto shgc_result = glazing_system_shgc->shgc();
    EXPECT_NEAR(shgc_result, 0.37270656, 1e-6);

    auto shgc_layer_temperatues_u =
      glazing_system_shgc->layer_temperatures(Tarcog::ISO15099::System::Uvalue);

    EXPECT_NEAR(shgc_layer_temperatues_u[0], 303.05518474277812, 1e-6);
    EXPECT_NEAR(shgc_layer_temperatues_u[1], 302.92520195300676, 1e-6);

    auto shgc_layer_temperatues_shgc =
      glazing_system_shgc->layer_temperatures(Tarcog::ISO15099::System::SHGC);

    EXPECT_NEAR(shgc_layer_temperatues_shgc[0], 305.53865133878355, 1e-6);
    EXPECT_NEAR(shgc_layer_temperatues_shgc[1], 305.45379473432968, 1e-6);
    EXPECT_NEAR(shgc_layer_temperatues_shgc[2], 305.53865133878355, 1e-6);
    EXPECT_NEAR(shgc_layer_temperatues_shgc[3], 305.45379473432968, 1e-6);
}

TEST_F(TestNFRC1022011SA1, Test_Solar)
{
    WCE_Optical_Results solar_results =
      glazing_system_u->optical_method_results(Optical_Standard_Method_Type::SOLAR);
    EXPECT_NEAR(
      solar_results.system_results.front.transmittance.direct_hemispherical, 0.1024, 1e-4);
    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_direct, 0.1024, 1e-4);
    EXPECT_NEAR(solar_results.system_results.front.transmittance.direct_diffuse, 0.1024, 1e-4);
    EXPECT_NEAR(solar_results.system_results.front.reflectance.direct_hemispherical, 0.3894, 1e-4);
    EXPECT_NEAR(solar_results.system_results.back.reflectance.direct_hemispherical, 0.5166, 1e-4);
    EXPECT_NEAR(solar_results.layer_results[0].front.absorptance.direct, 0.5166, 1e-4);
    EXPECT_NEAR(solar_results.layer_results[1].front.absorptance.direct, 0.5166, 1e-4);
    EXPECT_NEAR(solar_results.layer_results[0].front.absorptance.diffuse, 0.5166, 1e-4);
    EXPECT_NEAR(solar_results.layer_results[1].front.absorptance.diffuse, 0.5166, 1e-4);

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
      glazing_system_u->optical_method_results(Optical_Standard_Method_Type::SOLAR, 50);
    EXPECT_NEAR(solar_results_50_degrees.system_results.front.transmittance.direct_direct,
                0.79665121300654163,
                1e-6);
}

TEST_F(TestNFRC1022011SA1, Test_Visible)
{
    WCE_Optical_Results photopic_results =
      glazing_system_u->optical_method_results(Optical_Standard_Method_Type::PHOTOPIC);
    EXPECT_NEAR(
      photopic_results.system_results.front.transmittance.direct_hemispherical, 0.11304305, 1e-6);

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
}

TEST_F(TestNFRC1022011SA1, Test_SPF)
{
    WCE_Optical_Results spf_results =
      glazing_system_u->optical_method_results(Optical_Standard_Method_Type::SPF);

    EXPECT_NEAR(
      spf_results.system_results.front.transmittance.direct_direct, 0.092848281215754655, 1e-6);
    EXPECT_NEAR(
      spf_results.system_results.back.transmittance.direct_direct, 0.092848281215754655, 1e-6);
    EXPECT_NEAR(
      spf_results.system_results.front.reflectance.direct_direct, 0.049777244859762357, 1e-6);
    EXPECT_NEAR(
      spf_results.system_results.back.reflectance.direct_direct, 0.050726658558665656, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.transmittance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.front.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(spf_results.system_results.back.reflectance.direct_diffuse, 0.0, 1e-6);
    EXPECT_NEAR(
      spf_results.system_results.front.transmittance.diffuse_diffuse, 0.077907596000480486, 1e-6);
    EXPECT_NEAR(
      spf_results.system_results.back.transmittance.diffuse_diffuse, 0.077907596000480486, 1e-6);
    EXPECT_NEAR(
      spf_results.system_results.front.reflectance.diffuse_diffuse, 0.098173459998706705, 1e-6);
    EXPECT_NEAR(
      spf_results.system_results.back.reflectance.diffuse_diffuse, 0.099192441334194997, 1e-6);
}

TEST_F(TestNFRC1022011SA1, Test_TDW)
{
    WCE_Optical_Results tdw_results =
      glazing_system_u->optical_method_results(Optical_Standard_Method_Type::TDW);

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
}

TEST_F(TestNFRC1022011SA1, Test_TKR)
{
    WCE_Optical_Results tkr_results =
      glazing_system_u->optical_method_results(Optical_Standard_Method_Type::TKR);

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
}

TEST_F(TestNFRC1022011SA1, Test_TUV)
{
    WCE_Optical_Results tuv_results =
      glazing_system_u->optical_method_results(Optical_Standard_Method_Type::TUV);

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
}

TEST_F(TestNFRC1022011SA1, Test_Color)
{
    WCE_Color_Results color_results = glazing_system_u->color();

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
