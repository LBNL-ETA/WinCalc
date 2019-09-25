#ifndef WINCALC_THERMAL_CALCS_H
#define WINCALC_THERMAL_CALCS_H
#if 0

#    include <windows_standards/windows_standard.h>
#    include <OpticsProductData.hpp>

#    include "create_wce_objects.h"   // For Engine_Gap_Info, better to just expose engine gap enum
#    include "environmental_conditions.h"


namespace wincalc
{
    double
      calc_u_iso15099(std::vector<std::shared_ptr<wincalc::Product_Data_Thermal>> const & products,
                      std::vector<double> const & layer_solar_absorptances,
                      std::vector<Engine_Gap_Info> const & gaps,
                      double width,
                      double height,
                      Environments const & environments);

    double calc_shgc_iso15099(
      std::vector<std::shared_ptr<wincalc::Product_Data_Thermal>> const & products,
      std::vector<double> const & layer_solar_absorptances,
      std::vector<Engine_Gap_Info> const & gaps,
      double solar_transmittance,
      double width,
      double height,
      Environments const & environments);

    double calc_u(std::vector<std::shared_ptr<wincalc::Product_Data_Thermal>> const & products,
                  std::vector<double> const & layer_solar_absorptances,
                  std::vector<Engine_Gap_Info> const & gap_values,
                  double width,
                  double height,
                  Environments const & environments);

    double calc_shgc(std::vector<std::shared_ptr<wincalc::Product_Data_Thermal>> const & products,
                     std::vector<double> const & layer_solar_absorptances,
                     std::vector<Engine_Gap_Info> const & gap_values,
                     double solar_transmittance,
                     double width,
                     double height,
                     Environments const & environments);

    Thermal_Result calc_u(std::vector<wincalc::Product_Data_Optical_Thermal> const & products,
                          std::vector<Engine_Gap_Info> const & gap_values,
                          window_standards::Optical_Standard const & standard,
                          double width,
                          double height,
                          Environments const & environments,
                          double theta = 0,
                          double phi = 0);

    Thermal_Result calc_shgc(std::vector<wincalc::Product_Data_Optical_Thermal> const & products,
                             std::vector<Engine_Gap_Info> const & gap_values,
                             window_standards::Optical_Standard const & standard,
                             double width,
                             double height,
                             Environments const & environments,
                             double theta = 0,
                             double phi = 0);

    Thermal_Result calc_u(std::vector<OpticsParser::ProductData> const & products,
                          std::vector<Engine_Gap_Info> const & gap_values,
                          window_standards::Optical_Standard const & standard,
                          double width,
                          double height,
                          Environments const & environments);

    Thermal_Result calc_shgc(std::vector<OpticsParser::ProductData> const & products,
                             std::vector<Engine_Gap_Info> const & gap_values,
                             window_standards::Optical_Standard const & standard,
                             double width,
                             double height,
                             Environments const & environments);

}   // namespace wincalc
#endif
#endif