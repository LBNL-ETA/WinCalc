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

class TestNFRC102_102_2011SA1_CustomOpenings : public testing::Test
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
        bsdf_path /= "2011-SA1.XML";

        std::vector<std::variant<OpticsParser::ProductData, Product_Data_Optical_Thermal>> products;
        auto clear_3 = OpticsParser::parseJSONFile(clear_3_path.string());
        products.push_back(clear_3);
        products.push_back(clear_3);

        auto bsdf = OpticsParser::parseBSDFXMLFile(bsdf_path.string());

        // Venetian data
        double slat_thickness = 0.0029;
        double slat_width = .0508;

        double slat_spacing = .0432;

        double slat_curvature = .0;

        double theta = 0;
        double D1 = 0.016;
        double D2 = -0.63;
        double D3 = 0.53;
        double D4 = 0.043;

        bsdf.conductivity = 0.12;
        bsdf.thickness =
          D4 * (slat_width * std::cos(theta) + slat_thickness * abs(std::sin(theta)));

        double system_height = 1;
        double system_width = 1;

        double system_area = system_width * system_height;

        double d_surf =
          (1
           - slat_width * slat_thickness
               / ((slat_width * std::cos(theta) + slat_thickness * abs(std::sin(theta)))
                  * (slat_spacing + slat_thickness)));
        double Ah =
          system_width * system_height * D1
          * std::pow(((d_surf / (system_width * system_height)) * std::pow(std::cos(theta), D2)),
                     D3);

        auto bsdf_shade = convert_to_solid_layer(bsdf);
        bsdf_shade.thermal_data->effective_front_thermal_openness_area = Ah;
        bsdf_shade.thermal_data->permeability_factor = d_surf;

        bsdf_shade.optical_data->emissivity_front = 0.518577;
        bsdf_shade.optical_data->emissivity_back = 0.518577;
        bsdf_shade.optical_data->ir_transmittance_front = 0.42419052;
        bsdf_shade.optical_data->ir_transmittance_back = 0.42419052;

        std::dynamic_pointer_cast<Product_Data_Dual_Band_Optical_BSDF>(bsdf_shade.optical_data)
          ->user_defined_effective_values = true;

        products.push_back(bsdf_shade);

        double gap_thickness = 0.0127;
        double gap_shade_thickness = 0.0381;
        double gap_pressure = Gases::DefaultPressure;
        auto air_gap = std::make_shared<Tarcog::ISO15099::CIGUGapLayer>(
          gap_thickness, gap_pressure, Gases::CGas({{1.0, Gases::GasDef::Air}}));
        auto shade_gap = std::make_shared<Tarcog::ISO15099::CIGUGapLayer>(
          gap_shade_thickness, gap_pressure, Gases::CGas({{1.0, Gases::GasDef::Air}}));
        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> gaps;
        gaps.push_back(air_gap);
        gaps.push_back(shade_gap);

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        auto bsdf_hemisphere =
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Full);

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments(), bsdf_hemisphere);
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_shgc_environments(), bsdf_hemisphere);
    }
};

TEST_F(TestNFRC102_102_2011SA1_CustomOpenings, Test_Thermal)
{
    test_thermal_results("NFRC_102_102_2011SA1_CustomOpenings",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("NFRC_102_102_2011SA1_CustomOpenings",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}

TEST_F(TestNFRC102_102_2011SA1_CustomOpenings, Test_Optical)
{
    test_optical_results("NFRC_102_102_2011SA1_CustomOpenings", glazing_system_u, update_results);
}
