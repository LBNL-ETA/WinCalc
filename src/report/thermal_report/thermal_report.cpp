#include "thermal_report.h"

#include <memory>
#include <ranges>
#include <stdexcept>

#include "../../glazing_system.h"

namespace wincalc
{
    namespace
    {
        using Tarcog::ISO15099::CBaseLayer;
        using Tarcog::ISO15099::CEnvironment;
        using Tarcog::ISO15099::CIGUGapLayer;
        using Tarcog::ISO15099::CIGUSolidLayer;
        using Tarcog::ISO15099::CSystem;
        using FenestrationCommon::Side;

        std::string system_type_name(Tarcog::ISO15099::System system_type)
        {
            return system_type == Tarcog::ISO15099::System::Uvalue ? "U" : "SHGC";
        }

        std::shared_ptr<CEnvironment> as_environment(std::shared_ptr<CBaseLayer> const & layer)
        {
            const auto environment = std::dynamic_pointer_cast<CEnvironment>(layer);
            if(environment == nullptr)
            {
                throw std::runtime_error(
                  "Thermal report: the glazing system is not connected to its environments.");
            }
            return environment;
        }

        Thermal_Report_Environment environment_report(CEnvironment & engine_environment,
                                                      Environment const & input,
                                                      Tarcog::ISO15099::ShadingModifier const & modifier)
        {
            return Thermal_Report_Environment{
              .air_temperature = engine_environment.getAirTemperature(),
              .radiation_temperature = input.radiation_temperature,
              .infrared_irradiance = engine_environment.getEnvironmentIR(),
              .heat_flow = engine_environment.getHeatFlow(),
              .convective_heat_flow = engine_environment.getConvectionConductionFlow(),
              .radiative_heat_flow = engine_environment.getRadiationFlow(),
              .convective_coefficient = engine_environment.getHc(),
              .radiative_coefficient = engine_environment.getHr(),
              .shading_emissivity_ratio = modifier.emissivityRatio,
              .shading_convective_coefficient_ratio = modifier.hcRatio};
        }

        Thermal_Report_Surface surface_report(CIGUSolidLayer const & layer, Side side)
        {
            return Thermal_Report_Surface{.temperature = layer.surfaceTemperature(side),
                                          .radiosity = layer.J(side),
                                          .emissivity = layer.getSurface(side)->getEmissivity()};
        }

        Thermal_Report_Solid_Layer solid_layer_report(CIGUSolidLayer & layer)
        {
            return Thermal_Report_Solid_Layer{
              .thickness = layer.getThickness(),
              .is_shade = layer.isShadeLayer(),
              .front = surface_report(layer, Side::Front),
              .back = surface_report(layer, Side::Back),
              .heat_flow = layer.getHeatFlow(),
              .absorbed_solar_radiation = layer.getGainFlow(),
              .effective_conductivity = layer.getEffectiveThermalConductivity()};
        }

        Thermal_Report_Gap gap_report(CIGUGapLayer & gap)
        {
            return Thermal_Report_Gap{
              .thickness = gap.getThickness(),
              .pressure = gap.getPressure(),
              .average_temperature = gap.averageLayerTemperature(),
              .heat_flow = gap.getHeatFlow(),
              .convective_heat_flow = gap.getConvectionConductionFlow(),
              .radiative_heat_flow = gap.getRadiationFlow(),
              .ventilation_heat_gain = gap.getGainFlow(),
              .convective_coefficient = gap.getConductionConvectionCoefficient(),
              .effective_conductivity = gap.getEffectiveThermalConductivity()};
        }

        std::vector<Thermal_Report_Solid_Layer>
          solid_layer_reports(std::vector<std::shared_ptr<CIGUSolidLayer>> const & layers)
        {
            auto reports = layers | std::views::transform([](auto const & layer) {
                               return solid_layer_report(*layer);
                           });
            return {reports.begin(), reports.end()};
        }

        std::vector<Thermal_Report_Gap>
          gap_reports(std::vector<std::shared_ptr<CIGUGapLayer>> const & gaps)
        {
            auto reports =
              gaps | std::views::transform([](auto const & gap) { return gap_report(*gap); });
            return {reports.begin(), reports.end()};
        }
    }   // namespace

    Thermal_Report thermal_report(Glazing_System & glazing_system,
                                  Tarcog::ISO15099::System system_type,
                                  double theta,
                                  double phi)
    {
        using Tarcog::ISO15099::Environment;

        auto & system = glazing_system.get_thermal_system(theta, phi);
        // Asking for a solved quantity first makes sure the layers below hold the solution.
        const auto effective_conductivity = system.getEffectiveSystemConductivity(system_type);

        const auto solid_layers = system.getSolidLayers(system_type);
        if(solid_layers.empty())
        {
            throw std::runtime_error("Thermal report: the glazing system has no solid layers.");
        }
        const auto outdoor = as_environment(solid_layers.front()->getPreviousLayer());
        const auto indoor = as_environment(solid_layers.back()->getNextLayer());
        const auto inputs = glazing_system.environments();

        return Thermal_Report{
          .system_type = system_type_name(system_type),
          .theta = theta,
          .phi = phi,
          .number_of_iterations = system.getNumberOfIterations(system_type),
          .system_effective_conductivity = effective_conductivity,
          .outdoor = environment_report(
            *outdoor, inputs.outside, system.getShadingModifier(system_type, Environment::Outdoor)),
          .indoor = environment_report(
            *indoor, inputs.inside, system.getShadingModifier(system_type, Environment::Indoor)),
          .solid_layers = solid_layer_reports(solid_layers),
          .gaps = gap_reports(system.getGapLayers(system_type))};
    }
}   // namespace wincalc
