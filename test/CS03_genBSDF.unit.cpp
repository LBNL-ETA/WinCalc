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


class TestCS03genBSDF : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path product_path(test_dir);
        product_path /= "products";
        product_path /= "CS03_genBSDF.xml";
        
		std::vector<std::shared_ptr<OpticsParser::ProductData>> products;
		auto shade = OpticsParser::parseBSDFXMLFile(product_path.string());
		products.push_back(shade);

		std::vector<Engine_Gap_Info> gaps;

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        auto bsdf_hemisphere =
          SingleLayerOptics::CBSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Full);

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments(), bsdf_hemisphere);
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_shgc_environments(), bsdf_hemisphere);
    }
};

TEST_F(TestCS03genBSDF, Test_Thermal)
{
	// The genBSDF XML files do not have conductivity and so cannot generate thermal results
	EXPECT_THROW(glazing_system_u->gap_layers_effective_conductivities(Tarcog::ISO15099::System::Uvalue), std::runtime_error);
	EXPECT_THROW(glazing_system_u->gap_layers_effective_conductivities(Tarcog::ISO15099::System::SHGC), std::runtime_error);
	EXPECT_THROW(glazing_system_u->layer_temperatures(Tarcog::ISO15099::System::Uvalue), std::runtime_error);
	EXPECT_THROW(glazing_system_u->layer_temperatures(Tarcog::ISO15099::System::SHGC), std::runtime_error);
	EXPECT_THROW(glazing_system_u->relative_heat_gain(), std::runtime_error);
	EXPECT_THROW(glazing_system_u->shgc(), std::runtime_error);
	EXPECT_THROW(glazing_system_u->u(), std::runtime_error);
	EXPECT_THROW(glazing_system_u->solid_layers_effective_conductivities(Tarcog::ISO15099::System::Uvalue), std::runtime_error);
	EXPECT_THROW(glazing_system_u->solid_layers_effective_conductivities(Tarcog::ISO15099::System::SHGC), std::runtime_error);
	EXPECT_THROW(glazing_system_u->system_effective_conductivity(Tarcog::ISO15099::System::Uvalue), std::runtime_error);
	EXPECT_THROW(glazing_system_u->system_effective_conductivity(Tarcog::ISO15099::System::SHGC), std::runtime_error);

	
}

TEST_F(TestCS03genBSDF, Test_Optical)
{
    test_optical_results("CS03_genBSDF", glazing_system_u, update_results);
}
