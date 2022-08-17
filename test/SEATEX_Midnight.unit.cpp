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

class Test_SEATEX_Midnight : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path shade_path(test_dir);
        shade_path /= "products";
        shade_path /= "46016 SEATEX Midnight.xml";

        std::vector<std::shared_ptr<OpticsParser::ProductData>> parsed_products;
        auto shade = OpticsParser::parseBSDFXMLFile(shade_path.string());
        parsed_products.push_back(shade);
    
        std::vector<Engine_Gap_Info> gaps;
        
        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        auto bsdf_hemisphere =
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Full);

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, parsed_products, gaps, 1.0, 1.0, 90, nfrc_u_environments(), bsdf_hemisphere);
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, parsed_products, gaps, 1.0, 1.0, 90, nfrc_shgc_environments(), bsdf_hemisphere);
    }
};

TEST_F(Test_SEATEX_Midnight, Test_Optical)
{
	test_optical_results("SEATEX_Midnight", glazing_system_u, update_results);
}

TEST_F(Test_SEATEX_Midnight, Test_Thermal)
{
	test_thermal_results("SEATEX_Midnight", "thermal_U_Environment", glazing_system_u, update_results);
	test_thermal_results("SEATEX_Midnight", "thermal_SHGC_Environment", glazing_system_shgc, update_results);
}



