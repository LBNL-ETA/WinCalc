#ifndef WINCALC_OPTICAL_CALCS_H
#define WINCALC_OPTICAL_CALCS_H

#include <vector>

#include "optical_calc_params.h"
#include "optical_results.h"
#include "product_data.h"
#include "create_wce_objects.h"

namespace wincalc
{
    struct Layer_Optical_IR_Results_Needed_For_Thermal_Calcs
    {
        double transmittance_front;
        double transmittance_back;
        double absorptance_front;
        double absorptance_back;
    };

    std::vector<Layer_Optical_IR_Results_Needed_For_Thermal_Calcs>
      optical_ir_results_needed_for_thermal_calcs(
        std::vector<Product_Data_Optical_Thermal> const & product_data,
        window_standards::Optical_Standard const & standard);

#if 0
    struct Optical_Solar_Results_Needed_For_Thermal_Calcs
    {
        double total_solar_transmittance;
        std::vector<double> layer_solar_absorptances;
    };

    Optical_Solar_Results_Needed_For_Thermal_Calcs optical_solar_results_needed_for_thermal_calcs(
      std::vector<Product_Data_Optical_Thermal> const & product_data,
      window_standards::Optical_Standard const & standard,
      double theta = 0,
      double phi = 0,
      std::optional<SingleLayerOptics::BSDFHemisphere> bsdf_hemisphere =
        std::optional<SingleLayerOptics::BSDFHemisphere>(),
      Spectal_Data_Wavelength_Range_Method const & type =
        Spectal_Data_Wavelength_Range_Method::FULL,
      int number_visible_bands = 5,
      int number_solar_bands = 10);
#endif

	std::unique_ptr<SingleLayerOptics::IScatteringLayer>
		optical_solar_results_system_needed_for_thermal_calcs(
			std::vector<Product_Data_Optical_Thermal> const & product_data,
            std::optional<Product_Data_Optical_Thermal> const& non_coplanar_attachment_exterior,
            std::optional<Product_Data_Optical_Thermal> const& non_coplanar_attachment_interior,
			window_standards::Optical_Standard const & standard,
			std::optional<SingleLayerOptics::BSDFHemisphere> bsdf_hemisphere =
			std::optional<SingleLayerOptics::BSDFHemisphere>(),
			Spectal_Data_Wavelength_Range_Method const & type =
			Spectal_Data_Wavelength_Range_Method::FULL,
			int number_visible_bands = 5,
			int number_solar_bands = 10);

    double
      calc_optical_property(std::vector<std::shared_ptr<Product_Data_Optical>> const & product_data,
                            window_standards::Optical_Standard_Method const & method,
                            Calculated_Property_Choice property_choice,
                            Side_Choice side_choice,
                            Scattering_Choice scattering_choice,
                            double theta = 0,
                            double phi = 0,
                            std::optional<SingleLayerOptics::BSDFHemisphere> bsdf_hemisphere =
                              std::optional<SingleLayerOptics::BSDFHemisphere>(),
                            Spectal_Data_Wavelength_Range_Method const & type =
                              Spectal_Data_Wavelength_Range_Method::FULL,
                            int number_visible_bands = 5,
                            int number_solar_bands = 10);

    WCE_Optical_Results
      calc_all(std::vector<std::shared_ptr<Product_Data_Optical>> const & product_data,
               window_standards::Optical_Standard_Method const & method,
               double theta = 0,
               double phi = 0,
               std::optional<SingleLayerOptics::BSDFHemisphere> bsdf_hemisphere =
                 std::optional<SingleLayerOptics::BSDFHemisphere>(),
               Spectal_Data_Wavelength_Range_Method const & type =
                 Spectal_Data_Wavelength_Range_Method::FULL,
               int number_visible_bands = 5,
               int number_solar_bands = 10);

    WCE_Optical_Results calc_all(std::shared_ptr<SingleLayerOptics::IScatteringLayer> system,
                              double min_lambda,
                              double max_lambda,
                              double theta,
                              double phi);

    WCE_Color_Results
      calc_color(std::vector<std::shared_ptr<Product_Data_Optical>> const & product_data,
                 window_standards::Optical_Standard_Method const & method_x,
                 window_standards::Optical_Standard_Method const & method_y,
                 window_standards::Optical_Standard_Method const & method_z,
                 double theta = 0,
                 double phi = 0,
                 std::optional<SingleLayerOptics::BSDFHemisphere> bsdf_hemisphere =
                   std::optional<SingleLayerOptics::BSDFHemisphere>(),
                 Spectal_Data_Wavelength_Range_Method const & type =
                   Spectal_Data_Wavelength_Range_Method::FULL,
                 int number_visible_bands = 5,
                 int number_solar_bands = 10);
}   // namespace wincalc
#endif
