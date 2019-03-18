#ifndef WINCALC_THERMAL_CALCS_DISK_H_
#define WINCALC_THERMAL_CALCS_DISK_H_

#include <string>
#include <vector>

#include <windows_standards/windows_standard.h>
#include <OpticsParser.hpp>

#include "thermal_results.h"

enum class Gas_Type
{
    AIR,
    ARGON,
    KRYPTON,
    XENON
};

struct Gap_Data
{
    Gas_Type gas;
    double thickness;
};

Thermal_Result calc_u(std::vector<OpticsParser::ProductData> const & products,
                      std::vector<Gap_Data> const & gap_values,
                      Standard const & standard,
                      double height,
                      double width);

Thermal_Result calc_shgc(std::vector<OpticsParser::ProductData> const & products,
                         std::vector<Gap_Data> const & gap_values,
                         Standard const & standard,
                         double height,
                         double width);

#endif
