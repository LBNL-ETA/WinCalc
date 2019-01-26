#ifndef WINCALC_OPTICAL_CALCS_H
#define WINCALC_OPTICAL_CALCS_H

#include <vector>

#include <windows_standards/method.h>
#include <OpticsProductData.hpp>

#include <WCECommon.hpp>
#include <WCEGases.hpp>

#include "wincalc/results.h"

using Side_Choice = FenestrationCommon::Side;
using Calculated_Property_Choice = FenestrationCommon::PropertySimple;
using Scattering_Choice = FenestrationCommon::Scattering;


double calc_optical_property(OpticsParser::ProductData const & product_data,
                             Method const & method,
                             Calculated_Property_Choice property_choice,
                             Side_Choice side_choice,
                             Scattering_Choice scattering_choice);

double calc_optical_property(std::vector<OpticsParser::ProductData> const & product_data,
                             Method const & method,
                             Calculated_Property_Choice property_choice,
                             Side_Choice side_choice,
                             Scattering_Choice scattering_choice);

WCEResult calc_all(OpticsParser::ProductData const & product_data, Method const & method);
WCEResult calc_all(std::vector<OpticsParser::ProductData> const & product_data,
                   Method const & method);

WCEColorResult calc_color(OpticsParser::ProductData const & product_data,
                              Method const & method_x,
                              Method const & method_y,
                              Method const & method_z);

#endif
