#ifndef WINCALC_THERMAL_REPORT_H_
#define WINCALC_THERMAL_REPORT_H_

#include <string>
#include <vector>

#include <WCETarcog.hpp>

namespace wincalc
{
    struct Glazing_System;

    // Detailed thermal state of a solved glazing system (ISO 15099 center of glass).
    // All values are in SI units: temperatures in K, heat flows in W/m2, coefficients in
    // W/(m2 K), conductivities in W/(m K), lengths in m, pressures in Pa.
    //
    // Heat flow values keep the sign the engine uses and are not converted here.

    struct Thermal_Report_Environment
    {
        double air_temperature{0.0};
        double radiation_temperature{0.0};
        double infrared_irradiance{0.0};
        double heat_flow{0.0};
        double convective_heat_flow{0.0};
        double radiative_heat_flow{0.0};
        double convective_coefficient{0.0};
        double radiative_coefficient{0.0};
        // Ratios that let a system without the shade reproduce the shaded system. Both are 1
        // when no shading device faces this environment.
        double shading_emissivity_ratio{1.0};
        double shading_convective_coefficient_ratio{1.0};
    };

    struct Thermal_Report_Surface
    {
        double temperature{0.0};
        double radiosity{0.0};
        double emissivity{0.0};
    };

    struct Thermal_Report_Solid_Layer
    {
        double thickness{0.0};
        bool is_shade{false};
        Thermal_Report_Surface front;
        Thermal_Report_Surface back;
        double heat_flow{0.0};
        double absorbed_solar_radiation{0.0};
        double effective_conductivity{0.0};
    };

    struct Thermal_Report_Gap
    {
        double thickness{0.0};
        double pressure{0.0};
        double average_temperature{0.0};
        double heat_flow{0.0};
        double convective_heat_flow{0.0};
        double radiative_heat_flow{0.0};
        double ventilation_heat_gain{0.0};
        double convective_coefficient{0.0};
        double effective_conductivity{0.0};
    };

    struct Thermal_Report
    {
        // "U" or "SHGC": which of the two ISO 15099 runs the values belong to.
        std::string system_type;
        double theta{0.0};
        double phi{0.0};
        size_t number_of_iterations{0};
        double system_effective_conductivity{0.0};
        Thermal_Report_Environment outdoor;
        Thermal_Report_Environment indoor;
        // Ordered from the outdoor side to the indoor side. gaps[i] lies between
        // solid_layers[i] and solid_layers[i + 1].
        std::vector<Thermal_Report_Solid_Layer> solid_layers;
        std::vector<Thermal_Report_Gap> gaps;
    };

    // Solves the system if needed and collects its thermal state.
    Thermal_Report thermal_report(Glazing_System & glazing_system,
                                  Tarcog::ISO15099::System system_type,
                                  double theta = 0,
                                  double phi = 0);

    std::string thermal_report_to_json(Thermal_Report const & report);
    std::string thermal_report_to_xml(Thermal_Report const & report);

    // Returns 0 on success. The format follows the file extension (.json or .xml).
    int save_thermal_report(Thermal_Report const & report, std::string const & file_name);
}   // namespace wincalc

#endif
