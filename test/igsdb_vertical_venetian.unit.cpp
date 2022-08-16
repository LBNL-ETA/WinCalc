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

class TestVerticalVenetianIGSDBMaterialUserGeometry : public testing::Test
{
protected:
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path venetian_material_path(test_dir);
        venetian_material_path /= "products";
        venetian_material_path /= "igsdb_14477.json";

        std::vector<
          std::variant<std::shared_ptr<OpticsParser::ProductData>, Product_Data_Optical_Thermal>>
          products;

        auto shade_material = OpticsParser::parseJSONFile(venetian_material_path.string());

        wincalc::Venetian_Geometry geometry{15, 0.020, 0.050, 0.025, false};
        auto shade = wincalc::create_venetian_blind(geometry, shade_material);

        products.push_back(shade);

        std::vector<Engine_Gap_Info> gaps;

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        auto bsdf_hemisphere =
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments(), bsdf_hemisphere);
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_shgc_environments(), bsdf_hemisphere);
    }
};

TEST_F(TestVerticalVenetianIGSDBMaterialUserGeometry, Test_Thermal)
{
    test_thermal_results("vertical_venetian_igsdb_material_14477_user_geometry",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("vertical_venetian_igsdb_material_14477_user_geometry",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}

TEST_F(TestVerticalVenetianIGSDBMaterialUserGeometry, Test_Optical)
{
    test_optical_results(
      "vertical_venetian_igsdb_material_14477_user_geometry", glazing_system_u, update_results);
}
