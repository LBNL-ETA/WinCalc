#ifndef WINCALC_GLAZING_SYSTEM_H_
#define WINCALC_GLAZING_SYSTEM_H_

#include <vector>
#include <variant>
#include <windows_standards/windows_standard.h>
#include <WCETarcog.hpp>
#include "optical_results.h"
#include "environmental_conditions.h"
#include "product_data.h"
#include "create_wce_objects.h"
#include "deflection_results.h"
#include "layer_cache.h"

namespace wincalc
{
    // Some calculations require window data at the moment when it was made. For example, this is
    // used to calculate deflection.
    struct Temperature_Pressure
    {
        double temperature{293.15};
        double pressure{101325};
    };

    struct Glazing_System
    {
        Glazing_System(
          window_standards::Optical_Standard const & standard,
          std::vector<Product_Data_Optical_Thermal> const & product_data,
          std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> const & gap_values =
            std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>>(),
          double width = 1.0,
          double height = 1.0,
          double tilt = 90,
          Environments const & environment = nfrc_u_environments(),
          std::optional<SingleLayerOptics::BSDFHemisphere> const & bsdf_hemisphere =
            std::optional<SingleLayerOptics::BSDFHemisphere>(),
          Spectal_Data_Wavelength_Range_Method const & type =
            Spectal_Data_Wavelength_Range_Method::FULL,
          int number_visible_bands = 5,
          int number_solar_bands = 10,
          std::optional<Product_Data_Optical_Thermal> non_coplanar_attachment_exterior =
            std::nullopt,
          std::optional<Product_Data_Optical_Thermal> non_coplanar_attachment_interior =
            std::nullopt);

        Glazing_System(
          window_standards::Optical_Standard const & standard,
          std::vector<OpticsParser::ProductData> const & product_data,
          std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> const & gap_values =
            std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>>(),
          double width = 1.0,
          double height = 1.0,
          double tilt = 90,
          Environments const & environment = nfrc_u_environments(),
          std::optional<SingleLayerOptics::BSDFHemisphere> const & bsdf_hemisphere =
            std::optional<SingleLayerOptics::BSDFHemisphere>(),
          Spectal_Data_Wavelength_Range_Method const & type =
            Spectal_Data_Wavelength_Range_Method::FULL,
          int number_visible_bands = 5,
          int number_solar_bands = 10,
          std::optional<OpticsParser::ProductData> non_coplanar_attachment_exterior = std::nullopt,
          std::optional<OpticsParser::ProductData> non_coplanar_attachment_interior = std::nullopt);

        // Constructor with vector of variants added for ease of use in python
        Glazing_System(
          window_standards::Optical_Standard const & standard,
          std::vector<std::variant<OpticsParser::ProductData, Product_Data_Optical_Thermal>> const &
            product_data,
          std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> const & gap_values =
            std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>>(),
          double width = 1.0,
          double height = 1.0,
          double tilt = 90,
          Environments const & environment = nfrc_u_environments(),
          std::optional<SingleLayerOptics::BSDFHemisphere> const & bsdf_hemisphere =
            std::optional<SingleLayerOptics::BSDFHemisphere>(),
          Spectal_Data_Wavelength_Range_Method const & type =
            Spectal_Data_Wavelength_Range_Method::FULL,
          int number_visible_bands = 5,
          int number_solar_bands = 10,
          std::optional<std::variant<OpticsParser::ProductData, Product_Data_Optical_Thermal>>
            non_coplanar_attachment_exterior = std::nullopt,
          std::optional<std::variant<OpticsParser::ProductData, Product_Data_Optical_Thermal>>
            non_coplanar_attachment_interior = std::nullopt);

        double u(double theta = 0, double phi = 0);

        double shgc(double theta = 0, double phi = 0);
        std::vector<double> layer_temperatures(Tarcog::ISO15099::System system_type,
                                               double theta = 0,
                                               double phi = 0);

        void enable_deflection(bool model);
        void set_deflection_properties(double temperature_at_construction,
                                       double pressure_at_construction);
        void set_deflection_properties(std::vector<double> const & measured_deflected_gaps);
        void set_applied_loads(std::vector<double> const & loads);
        Deflection_Results calc_deflection_properties(Tarcog::ISO15099::System system_type,
                                                      double theta = 0,
                                                      double phi = 0);

        WCE_Optical_Results optical_method_results(std::string const & method_name,
                                                   double theta = 0,
                                                   double phi = 0) const;

        WCE_Color_Results color(double theta = 0,
                                double phi = 0,
                                std::string const & tristimulus_x_method = "COLOR_TRISTIMX",
                                std::string const & tristimulus_y_method = "COLOR_TRISTIMY",
                                std::string const & tristimulus_z_method = "COLOR_TRISTIMZ") const;

        std::vector<double> solid_layers_effective_conductivities(
          Tarcog::ISO15099::System system_type, double theta = 0, double phi = 0);

        std::vector<double> gap_layers_effective_conductivities(
          Tarcog::ISO15099::System system_type, double theta = 0, double phi = 0);

        double system_effective_conductivity(Tarcog::ISO15099::System system_type,
                                             double theta = 0,
                                             double phi = 0);

        double relative_heat_gain(double theta = 0, double phi = 0);

        double heat_flow(Tarcog::ISO15099::System system_type,
                         Tarcog::ISO15099::Environment env,
                         double theta = 0,
                         double phi = 0);

        double h(Tarcog::ISO15099::System system_type,
                 Tarcog::ISO15099::Environment env,
                 double theta = 0,
                 double phi = 0);

        std::vector<double>
          radiosities(Tarcog::ISO15099::System system_type, double theta = 0, double phi = 0);


        void optical_standard(window_standards::Optical_Standard const & s);
        window_standards::Optical_Standard optical_standard() const;

        void solid_layers(std::vector<Product_Data_Optical_Thermal> const & layers);
        std::vector<Product_Data_Optical_Thermal> solid_layers() const;

        void
          gap_layers(std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> const & layers);
        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> gap_layers() const;

        Environments environments() const;
        void environments(Environments const & environment);

        void set_width(double width);
        void set_height(double height);
        void set_tilt(double tilt);
        void flip_layer(size_t layer_index, bool flipped);
        void set_spectral_data_wavelength_range(Spectal_Data_Wavelength_Range_Method const & type =
                                                  Spectal_Data_Wavelength_Range_Method::FULL,
                                                int number_visible_bands = 5,
                                                int number_solar_bands = 10);

        bool isBSDF();

        [[nodiscard]] double overallThickness() const;

        void set_solid_layer_conductivites(const std::vector<double> & conductivities);
        void set_solid_layer_conductivity(size_t index, double conductivity);

        Tarcog::ISO15099::CSystem & get_thermal_system(double theta = 0, double phi = 0);

        std::optional<Product_Data_Optical_Thermal> non_coplanar_attachment_exterior() const;
        std::optional<Product_Data_Optical_Thermal> non_coplanar_attachment_interior() const;
        void
          non_coplanar_attachment_exterior(std::optional<Product_Data_Optical_Thermal> attachment);
        void
          non_coplanar_attachment_interior(std::optional<Product_Data_Optical_Thermal> attachment);

    protected:
        std::vector<Product_Data_Optical_Thermal> product_data;
        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> gap_values;
        std::optional<Product_Data_Optical_Thermal> non_coplanar_attachment_exterior_;
        std::optional<Product_Data_Optical_Thermal> non_coplanar_attachment_interior_;
        window_standards::Optical_Standard standard;
        double width;
        double height;
        double tilt;
        Environments environment;
        std::optional<SingleLayerOptics::BSDFHemisphere> bsdf_hemisphere;
        Spectal_Data_Wavelength_Range_Method spectral_data_wavelength_range_method;
        int number_visible_bands;
        int number_solar_bands;
        bool model_deflection = false;
        std::variant<Temperature_Pressure, std::vector<double>> deflection_properties;
        std::vector<double> applied_loads;

        void do_deflection_updates(double theta, double phi);
        void do_deflection_updates(std::optional<double> theta, std::optional<double> phi);
        void do_layer_absorptance_updates(double theta, double phi);
        void do_updates_for_thermal(double theta, double phi);

        Tarcog::ISO15099::CIGU & get_igu(double theta, double phi);
        Tarcog::ISO15099::CSystem & get_system(double theta, double phi);

        std::optional<Tarcog::ISO15099::CIGU> current_igu;
        std::optional<Tarcog::ISO15099::CSystem> current_system;

        bool is_current_igu_calculated(double theta, double phi);

        std::vector<double> get_solar_abs_front(double theta, double phi);
        double get_solar_transmittance_front(double theta, double phi);
        SingleLayerOptics::IScatteringLayer & get_optical_system_for_thermal_calcs();
        std::unique_ptr<SingleLayerOptics::IScatteringLayer> optical_system_for_thermal_calcs;


        std::optional<double> last_theta = std::nullopt;
        std::optional<double> last_phi = std::nullopt;
        void reset_system();
        void reset_igu();
        void sort_spectral_data();

        window_standards::Optical_Standard_Method get_method(std::string const & method_name) const;

    private:
        std::shared_ptr<SingleLayerOptics::IScatteringLayer>
          get_or_create_layer(std::string const & method_name,
                              window_standards::Optical_Standard_Method const & method) const;

        mutable std::unordered_map<LayerCacheKey,
                                   std::shared_ptr<SingleLayerOptics::IScatteringLayer>>
          m_layer_cache;
    };
}   // namespace wincalc
#endif
