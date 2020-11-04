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

class TestNFRC9223 : public testing::Test
{
protected:
	std::shared_ptr<Glazing_System> glazing_system_u;
	std::shared_ptr<Glazing_System> glazing_system_shgc;

	virtual void SetUp()
	{
		std::filesystem::path product_path(test_dir);
		product_path /= "products";
		product_path /= "9223.json";

		std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
		auto product = OpticsParser::parseJSONFile(product_path.string());
		products.push_back(product);

		std::vector<Engine_Gap_Info> gaps;

		std::filesystem::path standard_path(test_dir);
		standard_path /= "standards";
		standard_path /= "W5_NFRC_2003.std";
		Optical_Standard standard = load_optical_standard(standard_path.string());
		glazing_system_u = std::make_shared<Glazing_System>(
			standard, products, gaps, 1.0, 1.0, nfrc_u_environments());
		glazing_system_shgc =
			std::make_shared<Glazing_System>(standard, products, gaps, 1.0, 1.0, nfrc_shgc_environments());
	}
};

TEST_F(TestNFRC9223, Test_U)
{
	auto u_result = glazing_system_u->u();
	EXPECT_NEAR(u_result, 5.9125145552954441, 1e-6);
}

TEST_F(TestNFRC9223, Test_SHGC)
{
	auto shgc_result = glazing_system_shgc->shgc();
	EXPECT_NEAR(shgc_result, 0.86063515079923925, 1e-6);
}

TEST_F(TestNFRC9223, Test_Solar)
{
	WCE_Optical_Results solar_results = glazing_system_u->optical_method_results(Optical_Standard_Method_Type::SOLAR);

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
}

TEST_F(TestNFRC9223, Test_Visible)
{
	WCE_Optical_Results photopic_results = glazing_system_u->optical_method_results(Optical_Standard_Method_Type::PHOTOPIC);

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
}

TEST_F(TestNFRC9223, Test_SPF)
{
	WCE_Optical_Results spf_results = glazing_system_u->optical_method_results(Optical_Standard_Method_Type::SPF);

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
}

TEST_F(TestNFRC9223, Test_TDW)
{
	WCE_Optical_Results tdw_results = glazing_system_u->optical_method_results(Optical_Standard_Method_Type::TDW);

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
}

TEST_F(TestNFRC9223, Test_TKR)
{
	WCE_Optical_Results tkr_results = glazing_system_u->optical_method_results(Optical_Standard_Method_Type::TKR);

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
}

TEST_F(TestNFRC9223, Test_TUV)
{
	WCE_Optical_Results tuv_results = glazing_system_u->optical_method_results(Optical_Standard_Method_Type::TUV);

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
}

TEST_F(TestNFRC9223, Test_Color)
{
    WCE_Color_Results color_results = glazing_system_u->color();

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