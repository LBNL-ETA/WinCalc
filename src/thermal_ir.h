#ifndef WINCALC_THERMAL_IR_H_
#define WINCALC_THERMAL_IR_H_

#include <windows_standards/windows_standard.h>
#include "optical_results.h"
#include "product_data.h"

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
                                     Product_Data_Optical_Thermal const & product_data);

}   // namespace wincalc
#endif
