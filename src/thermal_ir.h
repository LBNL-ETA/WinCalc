#ifndef WINCALC_THERMAL_IR_H_
#define WINCALC_THERMAL_IR_H_

#include <vector>
#include <variant>
#include <OpticsParser.hpp>
#include <windows_standards/windows_standard.h>
#include <WCEGases.hpp>
#include <WCETarcog.hpp>
#include "gap.h"
#include "optical_results.h"
#include "environmental_conditions.h"
#include "product_data.h"
#include "create_wce_objects.h"

namespace wincalc
{
    struct ThermalIRResults
    {
        double transmittance_front_diffuse_diffuse;
        double transmittance_back_diffuse_diffuse;
        double emissivity_front_hemispheric;
        double emissivity_back_hemispheric;
    };

    ThermalIRResults calc_thermal_ir(window_standards::Optical_Standard const & standard,
                                     Product_Data_Optical_Thermal const & product_data,
                                     Spectal_Data_Wavelength_Range_Method const & type =
                                       Spectal_Data_Wavelength_Range_Method::FULL,
                                     int number_visible_bands = 5,
                                     int number_solar_bands = 10);

}   // namespace wincalc
#endif
