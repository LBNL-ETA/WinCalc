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

class TestCGDB18100 : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
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

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, nfrc_u_environments());
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, nfrc_shgc_environments());
    }
};

TEST_F(TestCGDB18100, Test_U)
{
	auto u_result = glazing_system_u->u();
	EXPECT_NEAR(u_result, 3.6258264794653012, 1e-6);
}

TEST_F(TestCGDB18100, Test_SHGC)
{
	auto shgc_result = glazing_system_shgc->shgc();
	EXPECT_NEAR(shgc_result, 0.72886709074334710, 1e-6);
}

TEST_F(TestCGDB18100, Test_Solar)
{
	WCE_Optical_Results solar_results =
		glazing_system_u->optical_method_results(Optical_Standard_Method_Type::SOLAR);

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
}

TEST_F(TestCGDB18100, Test_Visible)
{
	WCE_Optical_Results photopic_results =
		glazing_system_u->optical_method_results(Optical_Standard_Method_Type::PHOTOPIC);

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
}

TEST_F(TestCGDB18100, Test_SPF)
{
	WCE_Optical_Results spf_results =
		glazing_system_u->optical_method_results(Optical_Standard_Method_Type::SPF);

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
}

TEST_F(TestCGDB18100, Test_TDW)
{
	WCE_Optical_Results tdw_results =
		glazing_system_u->optical_method_results(Optical_Standard_Method_Type::TDW);

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
}

TEST_F(TestCGDB18100, Test_TKR)
{
	WCE_Optical_Results tkr_results =
		glazing_system_u->optical_method_results(Optical_Standard_Method_Type::TKR);

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
}

TEST_F(TestCGDB18100, Test_TUV)
{
	WCE_Optical_Results tuv_results =
		glazing_system_u->optical_method_results(Optical_Standard_Method_Type::TUV);

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
}

TEST_F(TestCGDB18100, Test_Color)
{
    WCE_Color_Results color_results = glazing_system_u->color();

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
