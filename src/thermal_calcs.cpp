
#include <algorithm>
#include <iostream>
#include <sstream>

#include "thermal_calcs.h"
/*
#include "FenestrationCommon.hpp"
#include "WCESingleLayerOptics.hpp"
#include "WCESpectralAveraging.hpp"
*/

double calc_u_iso15099(std::vector<OpticsParser::ProductData> const & layers,
                       std::vector<Gas_Choice> const & gaps,
                       double width,
                       double height)
{
    return 0.0;
}

Thermal_Result calc_u_and_shgc_iso15099(std::vector<OpticsParser::ProductData> const & layers,
                                        std::vector<Gas_Choice> const & gaps,
                                        double width,
                                        double height)
{
    return Thermal_Result();
}
