#ifndef WINCALC_THERMAL_CALCS_DISK_H_
#define WINCALC_THERMAL_CALCS_DISK_H_

#include <string>
#include <vector>

#include <windows_standards/windows_standard.h>
#include <OpticsParser.hpp>

#include "thermal_results.h"
#include "gap.h"
#include "environmental_conditions.h"

namespace wincalc
{
    Thermal_Result calc_u(std::vector<OpticsParser::ProductData> const & products,
                          std::vector<Gap_Data> const & gap_values,
                          Standard const & standard,
                          double width,
                          double height,
                          Environments const & environments);

    Thermal_Result calc_shgc(std::vector<OpticsParser::ProductData> const & products,
                             std::vector<Gap_Data> const & gap_values,
                             Standard const & standard,
                             double width,
                             double height,
                             Environments const & environments);
}   // namespace wincalc
#endif
