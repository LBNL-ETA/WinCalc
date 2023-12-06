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

class TestNFRC102NFRC102Vacuum : public testing::Test
{
protected:
    //std::shared_ptr<CIGUGapLayer> pillar_gap;
    std::shared_ptr<Glazing_System> glazing_system_u;
    std::shared_ptr<Glazing_System> glazing_system_shgc;

    virtual void SetUp()
    {
        std::filesystem::path clear_3_path(test_dir);
        clear_3_path /= "products";
        clear_3_path /= "CLEAR_3.json";

        std::vector<OpticsParser::ProductData> products;
        OpticsParser::Parser parser;
        auto clear_3 = parser.parseJSONFile(clear_3_path.string());
        products.push_back(clear_3);
        products.push_back(clear_3);

        // Add support pillars
        const auto pillarHeight = 0.0001;
        const auto gapPressure = 0.1333;
        const auto pillarConductivity = 999.0;
        const auto pillarArea = 0.03 * 0.03;
        const auto pillarRadius = 0.0002;

        Tarcog::ISO15099::CylindricalPillar pillar{
          pillarHeight, pillarConductivity, pillarArea, pillarRadius};

        auto pillar_gap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> gaps;
        gaps.push_back(pillar_gap);

        std::filesystem::path standard_path(test_dir);
        standard_path /= "standards";
        standard_path /= "W5_NFRC_2003.std";
        Optical_Standard standard = load_optical_standard(standard_path.string());

        glazing_system_u = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_u_environments());
        glazing_system_shgc = std::make_shared<Glazing_System>(
          standard, products, gaps, 1.0, 1.0, 90, nfrc_shgc_environments());
    }
};

TEST_F(TestNFRC102NFRC102Vacuum, Test_Thermal_Cylindrical)
{
    test_thermal_results("NFRC_102_NFRC_102_vacuum/cylindrical",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("NFRC_102_NFRC_102_vacuum/cylindrical",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}

TEST_F(TestNFRC102NFRC102Vacuum, Test_Thermal_Spherical)
{
    // Add support pillars
    const auto pillarHeight = 0.0001;
    const auto gapPressure = 0.1333;
    const auto pillarConductivity = 999.0;
    const auto pillarArea = 0.03 * 0.03;
    const auto radiusOfContact = 0.02e-3;

    Tarcog::ISO15099::SphericalPillar pillar{
      pillarHeight, pillarConductivity, pillarArea, radiusOfContact};

    auto pillarGap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

    glazing_system_u->gap_layers({pillarGap});
    glazing_system_shgc->gap_layers({pillarGap});

    test_thermal_results("NFRC_102_NFRC_102_vacuum/spherical",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("NFRC_102_NFRC_102_vacuum/spherical",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}


TEST_F(TestNFRC102NFRC102Vacuum, Test_Optical)
{
    test_optical_results("NFRC_102_NFRC_102_vacuum", glazing_system_u, update_results);
}
