#ifndef WINCALC_OPTICAL_CALCS_H
#define WINCALC_OPTICAL_CALCS_H

#include <vector>

#include "optical_calc_params.h"
#include "optical_results.h"
#include "product_data.h"

namespace wincalc
{
    double calc_optical_property(std::shared_ptr<Product_Data_Optical> const & product_data,
                                 window_standards::Optical_Standard_Method const & method,
                                 Calculated_Property_Choice property_choice,
                                 Side_Choice side_choice,
                                 Scattering_Choice scattering_choice,
                                 double theta = 0,
                                 double phi = 0);

    double
      calc_optical_property(std::vector<std::shared_ptr<Product_Data_Optical>> const & product_data,
                            window_standards::Optical_Standard_Method const & method,
                            Calculated_Property_Choice property_choice,
                            Side_Choice side_choice,
                            Scattering_Choice scattering_choice,
                            double theta = 0,
                            double phi = 0);

    WCE_Optical_Results calc_all(std::shared_ptr<Product_Data_Optical> const & product_data,
                                 window_standards::Optical_Standard_Method const & method,
                                 double theta = 0,
                                 double phi = 0);
    WCE_Optical_Results
      calc_all(std::vector<std::shared_ptr<Product_Data_Optical>> const & product_data,
               window_standards::Optical_Standard_Method const & method,
               double theta = 0,
               double phi = 0);

    WCE_Color_Results
      calc_color(std::vector<std::shared_ptr<Product_Data_Optical>> const & product_data,
                 window_standards::Optical_Standard_Method const & method_x,
                 window_standards::Optical_Standard_Method const & method_y,
                 window_standards::Optical_Standard_Method const & method_z,
                 double theta = 0,
                 double phi = 0);
}   // namespace wincalc
#endif
