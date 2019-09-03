#ifndef WINCALC_THERMAL_RESULT_H
#define WINCALC_THERMAL_RESULT_H

#include <vector>

namespace wincalc
{
    struct Thermal_Result
    {
        double result;
        double t_sol;
        std::vector<double> layer_solar_absorptances;
    };
}   // namespace wincalc
#endif
