#pragma once

#include <WCETarcog.hpp>

namespace wincalc
{
    struct Environment
    {
        Environment(double air_temperature,
                    double pressure,
                    double convection_coefficient,
                    Tarcog::ISO15099::BoundaryConditionsCoeffModel coefficient_model,
                    double radiation_temperature,
                    double emissivity,
                    double air_speed = 0.0,
                    Tarcog::ISO15099::AirHorizontalDirection air_direction =
                      Tarcog::ISO15099::AirHorizontalDirection::None,
                    double direct_solar_radiation = 0.0);
        double air_temperature;
        double pressure;
        double convection_coefficient;
        Tarcog::ISO15099::BoundaryConditionsCoeffModel coefficient_model;
        double radiation_temperature;   // this is effective room temperature/effective sky
                                        // temperature in WINDOW environment
        double emissivity;
        double air_speed = 0.0;
        Tarcog::ISO15099::AirHorizontalDirection air_direction =
          Tarcog::ISO15099::AirHorizontalDirection::None;
        double direct_solar_radiation = 0.0;
    };

    struct Environments
    {
		Environments(Environment outside, Environment inside);
        Environment outside;
        Environment inside;
    };

    Environments nfrc_u_environments();
    Environments nfrc_shgc_environments();

}   // namespace wincalc
