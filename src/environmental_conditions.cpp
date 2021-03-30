#include "environmental_conditions.h"

namespace wincalc
{
    Environment::Environment(double air_temperature,
                             double pressure,
                             double convection_coefficient,
                             Tarcog::ISO15099::BoundaryConditionsCoeffModel coefficient_model,
                             double radiation_temperature,
                             double emissivity,
                             double air_speed,
                             Tarcog::ISO15099::AirHorizontalDirection air_direction,
                             double direct_solar_radiation) :
        air_temperature(air_temperature),
        pressure(pressure),
        convection_coefficient(convection_coefficient),
        coefficient_model(coefficient_model),
		radiation_temperature(radiation_temperature),
		emissivity(emissivity),
		air_speed(air_speed),
		air_direction(air_direction),
		direct_solar_radiation(direct_solar_radiation)
    {}

	Environments::Environments(Environment outside, Environment inside) : outside(outside), inside(inside)
	{}

    Environments nfrc_u_environments()
    {
        Environment inside{294.15,
                           101325.0,
                           0.0,
                           Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH,
                           294.15,
                           1.0,
                           0.0,
                           Tarcog::ISO15099::AirHorizontalDirection::None,
                           0.0};

        Environment outside{255.15,
                            101325.0,
                            26.0,
                            Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH,
                            255.15,
                            1.0,
                            5.5,
                            Tarcog::ISO15099::AirHorizontalDirection::Windward,
                            0.0};

        return Environments{outside, inside};
    }

    Environments nfrc_shgc_environments()
    {
        Environment inside{297.15,
                           101325.0,
                           0.0,
                           Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH,
                           297.15,
                           1.0,
                           0.0,
                           Tarcog::ISO15099::AirHorizontalDirection::None,
                           0.0};

        Environment outside{305.15,
                            101325.0,
                            15.0,
                            Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH,
                            305.15,
                            1.0,
                            2.75,
                            Tarcog::ISO15099::AirHorizontalDirection::Windward,
                            783};

        return Environments{outside, inside};
    }
}   // namespace wincalc
