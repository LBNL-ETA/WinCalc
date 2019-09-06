#ifndef WINCALC_THERMAL_CALCS_H
#define WINCALC_THERMAL_CALCS_H

#include <windows_standards/windows_standard.h>
#include <OpticsProductData.hpp>

#include "thermal_results.h"

#include "create_wce_objects.h"   // For Engine_Gap_Info, better to just expose engine gap enum
#include "environmental_conditions.h"

namespace wincalc
{
    Thermal_Result calc_u_iso15099(std::vector<OpticsParser::ProductData> const & layers,
                                   std::vector<Engine_Gap_Info> const & gaps,
                                   double width,
                                   double height,
                                   window_standards::Optical_Standard const & standard,
                                   Environments const & environments);

    Thermal_Result calc_shgc_iso15099(std::vector<OpticsParser::ProductData> const & layers,
                                      std::vector<Engine_Gap_Info> const & gaps,
                                      double width,
                                      double height,
                                      window_standards::Optical_Standard const & standard,
                                      Environments const & environments);

}   // namespace wincalc
#endif
