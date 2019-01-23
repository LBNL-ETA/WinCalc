#ifndef WINCALC_THERMAL_CALCS_H
#define WINCALC_THERMAL_CALCS_H

#include <windows_standards/standard.h>
#include <OpticsParser.hpp>

#include <WCEGases.hpp>

using Gas_Choice = Gases::GasDef;


double calc_u_iso15099(std::vector<OpticsParser::ProductData> const & layers,
              std::vector<Gas_Choice> const & gaps,
              double width,
              double height);

struct Thermal_Result
{
    double u;
    double shgc;
};

Thermal_Result calc_u_and_shgc_iso15099(std::vector<OpticsParser::ProductData> const & layers,
                 std::vector<Gas_Choice> const & gaps,
                 double width,
                 double height);

#endif
