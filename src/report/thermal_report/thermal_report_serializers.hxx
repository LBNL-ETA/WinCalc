#pragma once

#include <fileParse/Common.hxx>
#include <fileParse/Vector.hxx>

#include "thermal_report.h"

// FileParse serializers for the thermal report. One definition serves both XML and JSON.
// The node names here are the file format; thermal_report.xsd describes the same structure.

namespace wincalc
{
    template<typename NodeAdapter>
    inline NodeAdapter & operator<<(NodeAdapter & node, Thermal_Report_Environment const & environment)
    {
        node << FileParse::Child{"AirTemperature", environment.air_temperature};
        node << FileParse::Child{"RadiationTemperature", environment.radiation_temperature};
        node << FileParse::Child{"InfraredIrradiance", environment.infrared_irradiance};
        node << FileParse::Child{"HeatFlow", environment.heat_flow};
        node << FileParse::Child{"ConvectiveHeatFlow", environment.convective_heat_flow};
        node << FileParse::Child{"RadiativeHeatFlow", environment.radiative_heat_flow};
        node << FileParse::Child{"ConvectiveCoefficient", environment.convective_coefficient};
        node << FileParse::Child{"RadiativeCoefficient", environment.radiative_coefficient};
        node << FileParse::Child{"ShadingEmissivityRatio", environment.shading_emissivity_ratio};
        node << FileParse::Child{"ShadingConvectiveCoefficientRatio",
                                 environment.shading_convective_coefficient_ratio};

        return node;
    }

    template<typename NodeAdapter>
    inline const NodeAdapter & operator>>(const NodeAdapter & node, Thermal_Report_Environment & environment)
    {
        node >> FileParse::Child{"AirTemperature", environment.air_temperature};
        node >> FileParse::Child{"RadiationTemperature", environment.radiation_temperature};
        node >> FileParse::Child{"InfraredIrradiance", environment.infrared_irradiance};
        node >> FileParse::Child{"HeatFlow", environment.heat_flow};
        node >> FileParse::Child{"ConvectiveHeatFlow", environment.convective_heat_flow};
        node >> FileParse::Child{"RadiativeHeatFlow", environment.radiative_heat_flow};
        node >> FileParse::Child{"ConvectiveCoefficient", environment.convective_coefficient};
        node >> FileParse::Child{"RadiativeCoefficient", environment.radiative_coefficient};
        node >> FileParse::Child{"ShadingEmissivityRatio", environment.shading_emissivity_ratio};
        node >> FileParse::Child{"ShadingConvectiveCoefficientRatio",
                                 environment.shading_convective_coefficient_ratio};

        return node;
    }

    template<typename NodeAdapter>
    inline NodeAdapter & operator<<(NodeAdapter & node, Thermal_Report_Surface const & surface)
    {
        node << FileParse::Child{"Temperature", surface.temperature};
        node << FileParse::Child{"Radiosity", surface.radiosity};
        node << FileParse::Child{"Emissivity", surface.emissivity};

        return node;
    }

    template<typename NodeAdapter>
    inline const NodeAdapter & operator>>(const NodeAdapter & node, Thermal_Report_Surface & surface)
    {
        node >> FileParse::Child{"Temperature", surface.temperature};
        node >> FileParse::Child{"Radiosity", surface.radiosity};
        node >> FileParse::Child{"Emissivity", surface.emissivity};

        return node;
    }

    template<typename NodeAdapter>
    inline NodeAdapter & operator<<(NodeAdapter & node, Thermal_Report_Solid_Layer const & layer)
    {
        node << FileParse::Child{"Thickness", layer.thickness};
        node << FileParse::Child{"IsShade", layer.is_shade};
        node << FileParse::Child{"FrontSurface", layer.front};
        node << FileParse::Child{"BackSurface", layer.back};
        node << FileParse::Child{"HeatFlow", layer.heat_flow};
        node << FileParse::Child{"AbsorbedSolarRadiation", layer.absorbed_solar_radiation};
        node << FileParse::Child{"EffectiveConductivity", layer.effective_conductivity};

        return node;
    }

    template<typename NodeAdapter>
    inline const NodeAdapter & operator>>(const NodeAdapter & node, Thermal_Report_Solid_Layer & layer)
    {
        node >> FileParse::Child{"Thickness", layer.thickness};
        node >> FileParse::Child{"IsShade", layer.is_shade};
        node >> FileParse::Child{"FrontSurface", layer.front};
        node >> FileParse::Child{"BackSurface", layer.back};
        node >> FileParse::Child{"HeatFlow", layer.heat_flow};
        node >> FileParse::Child{"AbsorbedSolarRadiation", layer.absorbed_solar_radiation};
        node >> FileParse::Child{"EffectiveConductivity", layer.effective_conductivity};

        return node;
    }

    template<typename NodeAdapter>
    inline NodeAdapter & operator<<(NodeAdapter & node, Thermal_Report_Gap const & gap)
    {
        node << FileParse::Child{"Thickness", gap.thickness};
        node << FileParse::Child{"Pressure", gap.pressure};
        node << FileParse::Child{"AverageTemperature", gap.average_temperature};
        node << FileParse::Child{"HeatFlow", gap.heat_flow};
        node << FileParse::Child{"ConvectiveHeatFlow", gap.convective_heat_flow};
        node << FileParse::Child{"RadiativeHeatFlow", gap.radiative_heat_flow};
        node << FileParse::Child{"VentilationHeatGain", gap.ventilation_heat_gain};
        node << FileParse::Child{"ConvectiveCoefficient", gap.convective_coefficient};
        node << FileParse::Child{"EffectiveConductivity", gap.effective_conductivity};

        return node;
    }

    template<typename NodeAdapter>
    inline const NodeAdapter & operator>>(const NodeAdapter & node, Thermal_Report_Gap & gap)
    {
        node >> FileParse::Child{"Thickness", gap.thickness};
        node >> FileParse::Child{"Pressure", gap.pressure};
        node >> FileParse::Child{"AverageTemperature", gap.average_temperature};
        node >> FileParse::Child{"HeatFlow", gap.heat_flow};
        node >> FileParse::Child{"ConvectiveHeatFlow", gap.convective_heat_flow};
        node >> FileParse::Child{"RadiativeHeatFlow", gap.radiative_heat_flow};
        node >> FileParse::Child{"VentilationHeatGain", gap.ventilation_heat_gain};
        node >> FileParse::Child{"ConvectiveCoefficient", gap.convective_coefficient};
        node >> FileParse::Child{"EffectiveConductivity", gap.effective_conductivity};

        return node;
    }

    template<typename NodeAdapter>
    inline NodeAdapter & operator<<(NodeAdapter & node, Thermal_Report const & report)
    {
        node << FileParse::Child{"SystemType", report.system_type};
        node << FileParse::Child{"Theta", report.theta};
        node << FileParse::Child{"Phi", report.phi};
        node << FileParse::Child{"NumberOfIterations", report.number_of_iterations};
        node << FileParse::Child{"SystemEffectiveConductivity", report.system_effective_conductivity};
        node << FileParse::Child{"Outdoor", report.outdoor};
        node << FileParse::Child{{"SolidLayers", "SolidLayer"}, report.solid_layers};
        node << FileParse::Child{{"Gaps", "Gap"}, report.gaps};
        node << FileParse::Child{"Indoor", report.indoor};

        return node;
    }

    template<typename NodeAdapter>
    inline const NodeAdapter & operator>>(const NodeAdapter & node, Thermal_Report & report)
    {
        node >> FileParse::Child{"SystemType", report.system_type};
        node >> FileParse::Child{"Theta", report.theta};
        node >> FileParse::Child{"Phi", report.phi};
        node >> FileParse::Child{"NumberOfIterations", report.number_of_iterations};
        node >> FileParse::Child{"SystemEffectiveConductivity", report.system_effective_conductivity};
        node >> FileParse::Child{"Outdoor", report.outdoor};
        node >> FileParse::Child{{"SolidLayers", "SolidLayer"}, report.solid_layers};
        node >> FileParse::Child{{"Gaps", "Gap"}, report.gaps};
        node >> FileParse::Child{"Indoor", report.indoor};

        return node;
    }
}   // namespace wincalc
