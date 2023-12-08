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
    // std::shared_ptr<CIGUGapLayer> pillar_gap;
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

TEST_F(TestNFRC102NFRC102Vacuum, Test_Thermal_Rectangular)
{
    const auto pillarLength = 0.1e-3;
    const auto pillarWidth = 0.1e-3;
    const auto pillarHeight = 0.1e-3;
    const auto gapPressure = 0.1333;
    const auto pillarConductivity = 999.0;
    const auto pillarArea = 0.03 * 0.03;

    Tarcog::ISO15099::RectangularPillar pillar{
      pillarHeight, pillarConductivity, pillarArea, pillarLength, pillarWidth};

    auto pillarGap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

    glazing_system_u->gap_layers({pillarGap});
    glazing_system_shgc->gap_layers({pillarGap});

    test_thermal_results("NFRC_102_NFRC_102_vacuum/rectangular",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("NFRC_102_NFRC_102_vacuum/rectangular",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}


TEST_F(TestNFRC102NFRC102Vacuum, Test_Thermal_Triangular)
{
    const auto pillarLength = 0.1e-3;
    const auto pillarHeight = 0.1e-3;
    const auto gapPressure = 0.1333;
    const auto pillarConductivity = 999.0;
    const auto pillarArea = 0.03 * 0.03;

    Tarcog::ISO15099::PolygonalPillar pillar{pillarHeight,
                                             pillarConductivity,
                                             pillarArea,
                                             pillarLength,
                                             Tarcog::ISO15099::PolygonType::Triangle};

    auto pillarGap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

    glazing_system_u->gap_layers({pillarGap});
    glazing_system_shgc->gap_layers({pillarGap});

    test_thermal_results("NFRC_102_NFRC_102_vacuum/triangular",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("NFRC_102_NFRC_102_vacuum/triangular",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}


TEST_F(TestNFRC102NFRC102Vacuum, Test_Thermal_Pentagon)
{
    const auto pillarLength = 0.1e-3;
    const auto pillarHeight = 0.1e-3;
    const auto gapPressure = 0.1333;
    const auto pillarConductivity = 999.0;
    const auto pillarArea = 0.03 * 0.03;

    Tarcog::ISO15099::PolygonalPillar pillar{pillarHeight,
                                             pillarConductivity,
                                             pillarArea,
                                             pillarLength,
                                             Tarcog::ISO15099::PolygonType::Pentagon};

    auto pillarGap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

    glazing_system_u->gap_layers({pillarGap});
    glazing_system_shgc->gap_layers({pillarGap});

    test_thermal_results("NFRC_102_NFRC_102_vacuum/pentagon",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("NFRC_102_NFRC_102_vacuum/pentagon",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}

TEST_F(TestNFRC102NFRC102Vacuum, Test_Thermal_Hexagon)
{
    const auto pillarLength = 0.1e-3;
    const auto pillarHeight = 0.1e-3;
    const auto gapPressure = 0.1333;
    const auto pillarConductivity = 999.0;
    const auto pillarArea = 0.03 * 0.03;

    Tarcog::ISO15099::PolygonalPillar pillar{pillarHeight,
                                             pillarConductivity,
                                             pillarArea,
                                             pillarLength,
                                             Tarcog::ISO15099::PolygonType::Hexagon};

    auto pillarGap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

    glazing_system_u->gap_layers({pillarGap});
    glazing_system_shgc->gap_layers({pillarGap});

    test_thermal_results("NFRC_102_NFRC_102_vacuum/hexagon",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("NFRC_102_NFRC_102_vacuum/hexagon",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}

TEST_F(TestNFRC102NFRC102Vacuum, Test_Thermal_LinearBearing)
{
    const auto pillarLength = 0.1e-3;
    const auto pillarWidth = 0.1e-3;
    const auto pillarHeight = 0.1e-3;
    const auto gapPressure = 0.1333;
    const auto pillarConductivity = 999.0;
    const auto pillarArea = 0.03 * 0.03;

    Tarcog::ISO15099::LinearBearingPillar pillar{
      pillarHeight, pillarConductivity, pillarArea, pillarLength, pillarWidth};

    auto pillarGap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

    glazing_system_u->gap_layers({pillarGap});
    glazing_system_shgc->gap_layers({pillarGap});

    test_thermal_results("NFRC_102_NFRC_102_vacuum/linear_bearing",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("NFRC_102_NFRC_102_vacuum/linear_bearing",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}


TEST_F(TestNFRC102NFRC102Vacuum, Test_Thermal_TruncatedCone)
{
    const auto pillarRadius1 = 0.1e-3;
    const auto pillarRadius2 = 0.2e-3;
    const auto pillarHeight = 0.1e-3;
    const auto gapPressure = 0.1333;
    const auto pillarConductivity = 999.0;
    const auto pillarArea = 0.03 * 0.03;

    const Tarcog::ISO15099::TruncatedConePillar pillar{
      pillarHeight, pillarConductivity, pillarArea, pillarRadius1, pillarRadius2};

    auto pillarGap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

    glazing_system_u->gap_layers({pillarGap});
    glazing_system_shgc->gap_layers({pillarGap});

    test_thermal_results("NFRC_102_NFRC_102_vacuum/truncated_cone",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("NFRC_102_NFRC_102_vacuum/truncated_cone",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}


TEST_F(TestNFRC102NFRC102Vacuum, Test_Thermal_AnnulusCylinder)
{
    const auto innerRadius = 0.1e-3;
    const auto outerRadius = 0.2e-3;
    const auto pillarHeight = 0.1e-3;
    const auto gapPressure = 0.1333;
    const auto pillarConductivity = 999.0;
    const auto pillarArea = 0.03 * 0.03;

    Tarcog::ISO15099::AnnulusCylinderPillar pillar{
      pillarHeight, pillarConductivity, pillarArea, innerRadius, outerRadius};

    auto pillarGap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

    glazing_system_u->gap_layers({pillarGap});
    glazing_system_shgc->gap_layers({pillarGap});

    test_thermal_results("NFRC_102_NFRC_102_vacuum/annulus_cylinder",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("NFRC_102_NFRC_102_vacuum/annulus_cylinder",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}

TEST_F(TestNFRC102NFRC102Vacuum, Test_Thermal_Measured)
{
    Tarcog::ISO15099::Glass glass1{0.005, 1.0, 0.9};
    Tarcog::ISO15099::Glass glass2{0.005, 1.0, 0.8};
    Tarcog::ISO15099::PillarMeasurement pillar{0.012, 0.5, 300, 350, glass1, glass2};

    auto pillarGap = Tarcog::ISO15099::Layers::createPillar(pillar);

    glazing_system_u->gap_layers({pillarGap});
    glazing_system_shgc->gap_layers({pillarGap});

    test_thermal_results("NFRC_102_NFRC_102_vacuum/measured",
                         "thermal_U_Environment",
                         glazing_system_u,
                         update_results);
    test_thermal_results("NFRC_102_NFRC_102_vacuum/measured",
                         "thermal_SHGC_Environment",
                         glazing_system_shgc,
                         update_results);
}


TEST_F(TestNFRC102NFRC102Vacuum, Test_Optical)
{
    test_optical_results("NFRC_102_NFRC_102_vacuum", glazing_system_u, update_results);
}
