#ifndef WINCALC_OPTICAL_CALCS_H
#define WINCALC_OPTICAL_CALCS_H

#include <vector>

#include "optical_calc_params.h"
#include "optical_results.h"

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

WCE_Optical_Result calc_all(OpticsParser::ProductData const & product_data, Method const & method);
WCE_Optical_Result calc_all(std::vector<OpticsParser::ProductData> const & product_data,
                   Method const & method);

WCE_Color_Result calc_color(std::vector<OpticsParser::ProductData> const & product_data,
                              Method const & method_x,
                              Method const & method_y,
                              Method const & method_z);

#endif
