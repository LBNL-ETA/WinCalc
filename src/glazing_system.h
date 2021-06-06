#ifndef WINCALC_GLAZING_SYSTEM_H_
#define WINCALC_GLAZING_SYSTEM_H_

#include <vector>
#include <variant>
#include <OpticsParser.hpp>
#include <windows_standards/windows_standard.h>
#include <WCEGases.hpp>
#include <WCETarcog.hpp>
#include "gap.h"
#include "optical_results.h"
#include "environmental_conditions.h"
#include "product_data.h"
#include "create_wce_objects.h"

namespace wincalc
{
    struct Glazing_System
    {
        Glazing_System(window_standards::Optical_Standard const & standard,
                       std::vector<Product_Data_Optical_Thermal> const & product_data,
                       std::vector<Engine_Gap_Info> const & gap_values,
                       double width = 1.0,
                       double height = 1.0,
                       Environments const & environment = nfrc_u_environments(),
                       std::optional<SingleLayerOptics::CBSDFHemisphere> const & bsdf_hemisphere =
                         std::optional<SingleLayerOptics::CBSDFHemisphere>(),
                       Spectal_Data_Wavelength_Range_Method const & type =
                         Spectal_Data_Wavelength_Range_Method::FULL,
                       int number_visible_bands = 5,
                       int number_solar_bands = 10);

        Glazing_System(window_standards::Optical_Standard const & standard,
                       std::vector<std::shared_ptr<OpticsParser::ProductData>> const & product_data,
                       std::vector<Engine_Gap_Info> const & gap_values,
                       double width = 1.0,
                       double height = 1.0,
                       Environments const & environment = nfrc_u_environments(),
                       std::optional<SingleLayerOptics::CBSDFHemisphere> const & bsdf_hemisphere =
                         std::optional<SingleLayerOptics::CBSDFHemisphere>(),
                       Spectal_Data_Wavelength_Range_Method const & type =
                         Spectal_Data_Wavelength_Range_Method::FULL,
                       int number_visible_bands = 5,
                       int number_solar_bands = 10);

        // Constructor with vector of variants added for ease of use in python
        Glazing_System(
          window_standards::Optical_Standard const & standard,
          std::vector<std::variant<std::shared_ptr<OpticsParser::ProductData>,
                                   Product_Data_Optical_Thermal>> const & product_data,
          std::vector<Engine_Gap_Info> const & gap_values = std::vector<Engine_Gap_Info>(),
          double width = 1.0,
          double height = 1.0,
          Environments const & environment = nfrc_u_environments(),
          std::optional<SingleLayerOptics::CBSDFHemisphere> const & bsdf_hemisphere =
            std::optional<SingleLayerOptics::CBSDFHemisphere>(),
          Spectal_Data_Wavelength_Range_Method const & type =
            Spectal_Data_Wavelength_Range_Method::FULL,
          int number_visible_bands = 5,
          int number_solar_bands = 10);

        double u(double theta = 0, double phi = 0);

        double shgc(double theta = 0, double phi = 0);
        std::vector<double> layer_temperatures(Tarcog::ISO15099::System system_type,
                                               double theta = 0,
                                               double phi = 0);

        WCE_Optical_Results
          optical_method_results(std::string const & method_name,
                                 double theta = 0,
                                 double phi = 0) const;

        WCE_Color_Results color(double theta = 0, double phi = 0) const;

        std::vector<double> solid_layers_effective_conductivities(
          Tarcog::ISO15099::System system_type, double theta = 0, double phi = 0);

        std::vector<double> gap_layers_effective_conductivities(
          Tarcog::ISO15099::System system_type, double theta = 0, double phi = 0);

        double system_effective_conductivity(Tarcog::ISO15099::System system_type,
                                             double theta = 0,
                                             double phi = 0);

        double relative_heat_gain(double theta = 0, double phi = 0);

        void optical_standard(window_standards::Optical_Standard const & s);
        window_standards::Optical_Standard optical_standard() const;

        void solid_layers(std::vector<Product_Data_Optical_Thermal> const & layers);
        std::vector<Product_Data_Optical_Thermal> solid_layers() const;

        Environments environments() const;
        void environments(Environments const & environment);

    protected:
        std::vector<Product_Data_Optical_Thermal> product_data;
        std::vector<Engine_Gap_Info> gap_values;
        window_standards::Optical_Standard standard;
        double width;
        double height;
        Environments environment;
        std::optional<SingleLayerOptics::CBSDFHemisphere> bsdf_hemisphere;
        Spectal_Data_Wavelength_Range_Method spectral_data_wavelength_range_method;
        int number_visible_bands;
        int number_solar_bands;

        Tarcog::ISO15099::CIGU & get_igu(double theta, double phi);
        Tarcog::ISO15099::CSystem & get_system(double theta, double phi);

        std::optional<Tarcog::ISO15099::CIGU> current_igu;
        std::optional<Tarcog::ISO15099::CSystem> current_system;
        double last_theta;
        double last_phi;
        void reset_system();
        void reset_igu();

        window_standards::Optical_Standard_Method
          get_method(std::string const & method_name) const;
    };
}   // namespace wincalc
#endif
