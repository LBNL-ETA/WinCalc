#include <sstream>

#include "glazing_system.h"
#include "optical_calcs.h"
#include "util.h"
#include "create_wce_objects.h"
#include "convert_optics_parser.h"

namespace wincalc
{
    WCE_Optical_Results Glazing_System::optical_method_results(
      window_standards::Optical_Standard_Method_Type const & method_type,
      double theta,
      double phi) const
    {
        auto method = get_method(method_type);
        return calc_all(get_optical_layers(product_data), method, theta, phi, bsdf_hemisphere);
    }

    WCE_Color_Results Glazing_System::color(double theta, double phi) const
    {
        window_standards::Optical_Standard_Method tristim_x =
          get_method(window_standards::Optical_Standard_Method_Type::COLOR_TRISTIMX);
        window_standards::Optical_Standard_Method tristim_y =
          get_method(window_standards::Optical_Standard_Method_Type::COLOR_TRISTIMY);
        window_standards::Optical_Standard_Method tristim_z =
          get_method(window_standards::Optical_Standard_Method_Type::COLOR_TRISTIMZ);
        return calc_color(get_optical_layers(product_data),
                          tristim_x,
                          tristim_y,
                          tristim_z,
                          theta,
                          phi,
                          bsdf_hemisphere);
    }

    void Glazing_System::reset_system()
    {
        current_igu = std::nullopt;
        current_system = std::nullopt;
    }

    window_standards::Optical_Standard_Method Glazing_System::get_method(
      window_standards::Optical_Standard_Method_Type const & method_type) const
    {
        auto method_itr = standard.methods.find(method_type);
        if(method_itr == standard.methods.end())
        {
            std::map<window_standards::Optical_Standard_Method_Type, std::string> method_names =
              window_standards::method_type_to_name();
            auto method_name = method_names.find(method_type);
            if(method_name != method_names.end())
            {
                std::stringstream err_msg;
                err_msg << "Standard " << standard.name << " does not include a "
                        << method_name->second << " method";
                throw std::runtime_error(err_msg.str());
            }
            else
            {
                std::stringstream err_msg;
                err_msg
                  << "Unknown method type: "
                  << static_cast<
                       std::underlying_type<window_standards::Optical_Standard_Method_Type>::type>(
                       method_type);
                throw std::runtime_error(err_msg.str());
            }
        }
        return method_itr->second;
    }

    Tarcog::ISO15099::CIGU & Glazing_System::get_igu(double theta, double phi)
    {
        if(current_igu.has_value() && theta == last_theta && phi == last_phi)
        {
            return current_igu.value();
        }
        else
        {
            current_igu = create_igu(product_data,
                                     gap_values,
                                     width,
                                     height,
                                     standard,
                                     theta,
                                     phi,
                                     bsdf_hemisphere,
                                     spectral_data_wavelength_range_method,
                                     number_visible_bands,
                                     number_solar_bands);
            return current_igu.value();
        }
    }

    Tarcog::ISO15099::CSystem & Glazing_System::get_system(double theta, double phi)
    {
        if(current_system.has_value() && theta == last_theta && phi == last_phi)
        {
            return current_system.value();
        }
        else
        {
            auto & igu = get_igu(theta, phi);
            current_system = create_system(igu, environment);
            last_theta = theta;
            last_phi = phi;
            return current_system.value();
        }
    }

    double Glazing_System::u(double theta, double phi)
    {
        auto & system = get_system(theta, phi);
        return system.getUValue();
    }

    double Glazing_System::shgc(double theta, double phi)
    {
        auto optical_results = optical_solar_results_needed_for_thermal_calcs(
          product_data, optical_standard(), theta, phi, bsdf_hemisphere);

        auto & system = get_system(theta, phi);

        system.setAbsorptances(optical_results.layer_solar_absorptances);
        return system.getSHGC(optical_results.total_solar_transmittance);


        // return shgc(optical_results.layer_solar_absorptances,
        //            optical_results.total_solar_transmittance);
    }

    std::vector<double> Glazing_System::layer_temperatures(Tarcog::ISO15099::System system_type,
                                                           double theta,
                                                           double phi)
    {
        auto optical_results = optical_solar_results_needed_for_thermal_calcs(
          product_data, optical_standard(), theta, phi, bsdf_hemisphere);
        auto & system = get_system(theta, phi);
        system.setAbsorptances(optical_results.layer_solar_absorptances);
        return system.getTemperatures(system_type);
    }
    std::vector<double> Glazing_System::solid_layers_effective_conductivities(
      Tarcog::ISO15099::System system_type, double theta, double phi)
    {
        auto & system = get_system(theta, phi);
        return system.getSolidEffectiveLayerConductivities(system_type);
    }
    std::vector<double> Glazing_System::gap_layers_effective_conductivities(
      Tarcog::ISO15099::System system_type, double theta, double phi)
    {
        auto & system = get_system(theta, phi);
        return system.getGapEffectiveLayerConductivities(system_type);
    }
    double Glazing_System::system_effective_conductivity(Tarcog::ISO15099::System system_type,
                                                         double theta,
                                                         double phi)
    {
        auto & system = get_system(theta, phi);
        return system.getEffectiveSystemConductivity(system_type);
    }
    double Glazing_System::relative_heat_gain(double theta, double phi)
    {
        auto optical_results = optical_solar_results_needed_for_thermal_calcs(
          product_data, optical_standard(), theta, phi, bsdf_hemisphere);
        auto & system = get_system(theta, phi);
        return system.relativeHeatGain(optical_results.total_solar_transmittance);
    }

    void Glazing_System::optical_standard(window_standards::Optical_Standard const & s)
    {
        reset_system();
        standard = s;
    }
    window_standards::Optical_Standard Glazing_System::optical_standard() const
    {
        return standard;
    }

    void Glazing_System::solid_layers(std::vector<Product_Data_Optical_Thermal> const & layers)
    {
        reset_system();
        product_data = layers;
    }

    std::vector<Product_Data_Optical_Thermal> Glazing_System::solid_layers() const
    {
        return product_data;
    }

    Glazing_System::Glazing_System(
      std::vector<Product_Data_Optical_Thermal> const & product_data,
      std::vector<Engine_Gap_Info> const & gap_values,
      window_standards::Optical_Standard const & standard,
      double width,
      double height,
      Environments const & environment,
      std::optional<SingleLayerOptics::CBSDFHemisphere> const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & spectral_data_wavelength_range_method,
      int number_visible_bands,
      int number_solar_bands) :
        product_data(product_data),
        gap_values(gap_values),
        standard(standard),
        width(width),
        height(height),
        environment(environment),
        bsdf_hemisphere(bsdf_hemisphere),
        spectral_data_wavelength_range_method(spectral_data_wavelength_range_method),
        number_visible_bands(number_visible_bands),
        number_solar_bands(number_solar_bands)
    {}

    Glazing_System::Glazing_System(
      std::vector<std::shared_ptr<OpticsParser::ProductData>> const & product_data,
      std::vector<Engine_Gap_Info> const & gap_values,
      window_standards::Optical_Standard const & standard,
      double width,
      double height,
      Environments const & environment,
      std::optional<SingleLayerOptics::CBSDFHemisphere> const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & spectral_data_wavelength_range_method,
      int number_visible_bands,
      int number_solar_bands) :
        product_data(convert_to_solid_layers(product_data)),
        gap_values(gap_values),
        standard(standard),
        width(width),
        height(height),
        environment(environment),
        bsdf_hemisphere(bsdf_hemisphere),
        spectral_data_wavelength_range_method(spectral_data_wavelength_range_method),
        number_visible_bands(number_visible_bands),
        number_solar_bands(number_solar_bands)
    {}
}   // namespace wincalc
