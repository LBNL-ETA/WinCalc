#pragma once

namespace wincalc
{
    enum class BoundaryConditionsCoeffModel
    {
        CalculateH,
        HPrescribed,
        HcPrescribed
    };

	struct Environment
	{
            double air_tempareature;
            double pressure;
            double convection_coefficient;
            BoundaryConditionsCoeffModel convection_model;
            double radiation_temperature;	// this is effective room temperature/effective sky temperature in WINDOW environment
            double emissivity;
            double air_speed;
	};
	
    struct Outside_Environment : public Environment
    {        
        double direct_solar_radiation;    
    };

}   // namespace wincalc
