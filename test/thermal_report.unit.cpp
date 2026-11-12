#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include <gtest/gtest.h>

#include "wincalc/wincalc.h"
#include "paths.h"

using namespace wincalc;
using namespace window_standards;

namespace
{
    constexpr double tolerance{1e-6};

    std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> air_gaps(size_t count)
    {
        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> gaps;
        for(size_t index = 0; index < count; ++index)
        {
            gaps.push_back(std::make_shared<Tarcog::ISO15099::CIGUGapLayer>(
              0.0127, Gases::DefaultPressure, Gases::CGas({{1.0, Gases::GasDef::Air}})));
        }
        return gaps;
    }

    Glazing_System clear_glazing_system(size_t layer_count, Environments const & environments)
    {
        const auto products_dir = std::filesystem::path(test_dir) / "products";
        const auto standards_dir = std::filesystem::path(test_dir) / "standards";

        OpticsParser::Parser parser;
        const auto clear_3 = parser.parseJSONFile((products_dir / "CLEAR_3.json").string());
        const std::vector<OpticsParser::ProductData> products(layer_count, clear_3);
        const auto standard =
          load_optical_standard((standards_dir / "W5_NFRC_2003.std").string());

        return Glazing_System(
          standard, products, air_gaps(layer_count - 1), 1.0, 1.0, 90, environments);
    }
}   // namespace

TEST(Thermal_Report, Triple_Clear_U_Matches_Glazing_System_Results)
{
    using Tarcog::ISO15099::Environment;
    using Tarcog::ISO15099::System;

    auto glazing_system = clear_glazing_system(3, nfrc_u_environments());

    const auto report = thermal_report(glazing_system, System::Uvalue);

    EXPECT_EQ(report.system_type, "U");
    ASSERT_EQ(report.solid_layers.size(), 3u);
    ASSERT_EQ(report.gaps.size(), 2u);

    const auto temperatures = glazing_system.layer_temperatures(System::Uvalue);
    const auto radiosities = glazing_system.radiosities(System::Uvalue);
    const auto solid_keff = glazing_system.solid_layers_effective_conductivities(System::Uvalue);
    const auto gap_keff = glazing_system.gap_layers_effective_conductivities(System::Uvalue);
    for(size_t index = 0; index < report.solid_layers.size(); ++index)
    {
        const auto & layer = report.solid_layers[index];
        EXPECT_NEAR(layer.front.temperature, temperatures[2 * index], tolerance);
        EXPECT_NEAR(layer.back.temperature, temperatures[2 * index + 1], tolerance);
        EXPECT_NEAR(layer.front.radiosity, radiosities[2 * index], tolerance);
        EXPECT_NEAR(layer.back.radiosity, radiosities[2 * index + 1], tolerance);
        EXPECT_NEAR(layer.effective_conductivity, solid_keff[index], tolerance);
        EXPECT_FALSE(layer.is_shade);
    }
    for(size_t index = 0; index < report.gaps.size(); ++index)
    {
        const auto & gap = report.gaps[index];
        EXPECT_NEAR(gap.effective_conductivity, gap_keff[index], tolerance);
        EXPECT_NEAR(gap.heat_flow, gap.convective_heat_flow + gap.radiative_heat_flow, tolerance);
        EXPECT_NEAR(gap.thickness, 0.0127, tolerance);
    }

    EXPECT_NEAR(report.system_effective_conductivity,
                glazing_system.system_effective_conductivity(System::Uvalue),
                tolerance);
    EXPECT_NEAR(
      report.indoor.heat_flow, glazing_system.heat_flow(System::Uvalue, Environment::Indoor), tolerance);
    EXPECT_NEAR(report.outdoor.heat_flow,
                glazing_system.heat_flow(System::Uvalue, Environment::Outdoor),
                tolerance);
    EXPECT_NEAR(report.indoor.convective_coefficient + report.indoor.radiative_coefficient,
                glazing_system.h(System::Uvalue, Environment::Indoor),
                tolerance);
    EXPECT_NEAR(report.outdoor.convective_coefficient + report.outdoor.radiative_coefficient,
                glazing_system.h(System::Uvalue, Environment::Outdoor),
                tolerance);
    EXPECT_NEAR(report.indoor.air_temperature, 294.15, tolerance);
    EXPECT_NEAR(report.outdoor.air_temperature, 255.15, tolerance);
}

TEST(Thermal_Report, Steady_State_Heat_Flow_Is_The_Same_Through_Every_Layer_For_U)
{
    auto glazing_system = clear_glazing_system(3, nfrc_u_environments());

    const auto report = thermal_report(glazing_system, Tarcog::ISO15099::System::Uvalue);

    const auto expected = report.indoor.heat_flow;
    constexpr double balance_tolerance{1e-3};
    EXPECT_NEAR(report.outdoor.heat_flow, expected, balance_tolerance);
    for(const auto & layer : report.solid_layers)
    {
        EXPECT_NEAR(layer.heat_flow, expected, balance_tolerance);
        EXPECT_NEAR(layer.absorbed_solar_radiation, 0.0, tolerance);
    }
    for(const auto & gap : report.gaps)
    {
        EXPECT_NEAR(gap.heat_flow, expected, balance_tolerance);
        EXPECT_NEAR(gap.ventilation_heat_gain, 0.0, tolerance);
    }
}

TEST(Thermal_Report, SHGC_Run_Reports_Absorbed_Solar_Radiation)
{
    auto glazing_system = clear_glazing_system(2, nfrc_shgc_environments());

    const auto report = thermal_report(glazing_system, Tarcog::ISO15099::System::SHGC);

    EXPECT_EQ(report.system_type, "SHGC");
    ASSERT_EQ(report.solid_layers.size(), 2u);
    for(const auto & layer : report.solid_layers)
    {
        EXPECT_GT(layer.absorbed_solar_radiation, 0.0);
    }
}

TEST(Thermal_Report, Single_Layer_Has_No_Gaps)
{
    auto glazing_system = clear_glazing_system(1, nfrc_u_environments());

    const auto report = thermal_report(glazing_system, Tarcog::ISO15099::System::Uvalue);

    EXPECT_EQ(report.solid_layers.size(), 1u);
    EXPECT_TRUE(report.gaps.empty());
    EXPECT_FALSE(thermal_report_to_json(report).empty());
    EXPECT_FALSE(thermal_report_to_xml(report).empty());
}

TEST(Thermal_Report, Json_And_Xml_Carry_The_Structure)
{
    auto glazing_system = clear_glazing_system(3, nfrc_u_environments());
    const auto report = thermal_report(glazing_system, Tarcog::ISO15099::System::Uvalue);

    const auto json = thermal_report_to_json(report);
    const auto xml = thermal_report_to_xml(report);

    for(const auto * name : {"ThermalReport", "Outdoor", "Indoor", "SolidLayer", "Gap",
                             "FrontSurface", "ConvectiveHeatFlow", "EffectiveConductivity"})
    {
        EXPECT_NE(json.find(name), std::string::npos) << name;
        EXPECT_NE(xml.find(name), std::string::npos) << name;
    }
}

TEST(Thermal_Report, Saves_Json_And_Xml_Files_By_Extension)
{
    auto glazing_system = clear_glazing_system(3, nfrc_u_environments());
    const auto report = thermal_report(glazing_system, Tarcog::ISO15099::System::Uvalue);
    const auto directory = std::filesystem::temp_directory_path() / "wincalc_thermal_report";
    std::filesystem::create_directories(directory);

    const auto json_file = directory / "triple_clear_U.json";
    const auto xml_file = directory / "triple_clear_U.xml";

    EXPECT_EQ(save_thermal_report(report, json_file.string()), 0);
    EXPECT_EQ(save_thermal_report(report, xml_file.string()), 0);
    EXPECT_GT(std::filesystem::file_size(json_file), 0u);
    EXPECT_GT(std::filesystem::file_size(xml_file), 0u);
    EXPECT_NE(save_thermal_report(report, (directory / "triple_clear_U.txt").string()), 0);
}
