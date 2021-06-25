#ifndef WINCALC_DEFLECTION_RESULTS_H_
#define WINCALC_DEFLECTION_RESULTS_H_

#include <vector>

namespace wincalc
{
    struct Deflection_Results
    {
        std::vector<double> deflection_max;
        std::vector<double> deflection_mean;
		std::vector<double> pressure_difference;
    };
}   // namespace wincalc

#endif
