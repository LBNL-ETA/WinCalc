#pragma once

#include <WCETarcog.hpp>

namespace wincalc
{
    struct Environment
    {
        double air_tempareature;
        double pressure;
        double convection_coefficient;
        Tarcog::ISO15099::BoundaryConditionsCoeffModel coefficient_model;
        double radiation_temperature;   // this is effective room temperature/effective sky
                                        // temperature in WINDOW environment
        double emissivity;
        double air_speed;
        double direct_solar_radiation;
    };

    struct Environments
    {
        Environment outside;
        Environment inside;
    };

}   // namespace wincalc
