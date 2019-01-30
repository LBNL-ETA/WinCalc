#ifndef WINCALC_THERMAL_CALCS_H
#define WINCALC_THERMAL_CALCS_H

#include <standard.h>
#include <OpticsProductData.hpp>

#include <WCEGases.hpp>

using Gas_Choice = Gases::GasDef;

struct Gap_Info
{
    Gas_Choice gas;
    double thickness;
};

double calc_u_iso15099(std::vector<OpticsParser::ProductData> const & layers,
                                        std::vector<Gap_Info> const & gaps,
                                        double width,
                                        double height,
                                        Standard const & standard);

double calc_shgc_iso15099(std::vector<OpticsParser::ProductData> const & layers,
                          std::vector<Gap_Info> const & gaps,
                          double width,
                          double height,
                          Standard const & standard);

#endif
