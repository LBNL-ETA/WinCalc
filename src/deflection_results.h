#ifndef WINCALC_DEFLECTION_RESULTS_H_
#define WINCALC_DEFLECTION_RESULTS_H_

#include <vector>

namespace wincalc
{
    struct Deflection_Results
    {
        std::vector<double> layer_deflection_max;
        std::vector<double> layer_deflection_mean;
        std::vector<double> panes_load;
        std::vector<double> gap_width_max;
        std::vector<double> gap_width_mean;
    };
}   // namespace wincalc

#endif
