#ifndef WINCALC_THERMAL_CALCS_H
#define WINCALC_THERMAL_CALCS_H

#include <windows_standards/windows_standard.h>
#include <OpticsProductData.hpp>

#include <WCEGases.hpp>

#include "thermal_results.h"

using Gas_Choice = Gases::GasDef;

struct Gap_Info
{
    Gas_Choice gas;
    double thickness;
};

Thermal_Result calc_u_iso15099(std::vector<OpticsParser::ProductData> const & layers,
                       std::vector<Gap_Info> const & gaps,
                       double width,
                       double height,
                       Standard const & standard);

Thermal_Result calc_shgc_iso15099(std::vector<OpticsParser::ProductData> const & layers,
                          std::vector<Gap_Info> const & gaps,
                          double width,
                          double height,
                          Standard const & standard);

#endif
