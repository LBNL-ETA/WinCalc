#include <sstream>

#include "glazing_system.h"
#include "optical_calcs.h"
#include "util.h"
#include "create_wce_objects.h"
#include "convert_optics_parser.h"

namespace wincalc
{
    WCE_Optical_Results Glazing_System::optical_method_results(std::string const & method_name,
                                                               double theta,
                                                               double phi) const
    {
        if(method_name == "THERMAL IR")
        {
            throw std::runtime_error(
              "THERMAL IR results should not be calculated using the generic method for optical "
              "calculations.  Valid results are only available for single layers via the "
              "calc_thermal_ir function which should be used instead.  If results using the "
              "generic calculations are absolutely required rename the THERMAL IR method in the "
              "optical standard file but care should be taken in interpreting any results "
              "calculated this way.");
        }
        std::vector<std::string> color_methods{
          "COLOR_TRISTIMX", "COLOR_TRISTIMY", "COLOR_TRISTIMZ", "CRI_X", "CRI_Y", "CRI_Z"};
        if(std::find(color_methods.begin(), color_methods.end(), method_name)
           != color_methods.end())
        {
            throw std::runtime_error(
              "Individual tristimulus color optical methods should not be calculated using the "
              "generic method for optical calculations.  The color function which takes the X, Y "
              "and Z optical methods should be used instead.  If results from individual tristulus "
              "methods are absolutely required rename the tristimuls method in the optical "
              "standard file but care should be taken in interpreting any results calculated this "
              "way.");
        }
        auto method = get_method(method_name);
        return calc_all(get_optical_layers(product_data),
                        method,
                        theta,
                        phi,
                        bsdf_hemisphere,
                        spectral_data_wavelength_range_method,
                        number_visible_bands,
                        number_solar_bands);
    }

    WCE_Color_Results Glazing_System::color(double theta,
                                            double phi,
                                            std::string const & tristimulus_x_method,
                                            std::string const & tristimulus_y_method,
                                            std::string const & tristimulus_z_method) const
    {
        window_standards::Optical_Standard_Method tristim_x = get_method(tristimulus_x_method);
        window_standards::Optical_Standard_Method tristim_y = get_method(tristimulus_y_method);
        window_standards::Optical_Standard_Method tristim_z = get_method(tristimulus_z_method);
        return calc_color(get_optical_layers(product_data),
                          tristim_x,
                          tristim_y,
                          tristim_z,
                          theta,
                          phi,
                          bsdf_hemisphere,
                          spectral_data_wavelength_range_method,
                          number_visible_bands,
                          number_solar_bands);
    }

    void Glazing_System::reset_igu()
    {
		optical_system_for_thermal_calcs = nullptr;
        current_igu = std::nullopt;
        reset_system();
    }

    void Glazing_System::reset_system()
    {
        current_system = std::nullopt;
    }

    window_standards::Optical_Standard_Method
      Glazing_System::get_method(std::string const & method_name) const
    {
        auto method_itr = standard.methods.find(method_name);
        if(method_itr == standard.methods.end())
        {
            std::stringstream err_msg;
            err_msg << "Standard " << standard.name << " does not include a " << method_name
                    << " method";
            throw std::runtime_error(err_msg.str());
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
            current_igu = create_igu(product_data, gap_values, width, height, tilt, standard);
            if(!applied_loads.empty())
            {
                current_igu.value().setAppliedLoad(applied_loads);
            }
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
        do_deflection_updates(theta, phi);
        auto & system = get_system(theta, phi);
        return system.getUValue();
    }

    double Glazing_System::shgc(double theta, double phi)
    {
        // Doing deflection updates and creating the system before calculating the optical results
        // because there are cases where the thermal system cannot be created.  E.G. genSDF XML
        // files do not have conductivity and so can be used in optical calcs but not thermal.
        // Creating the system is much less expensive than doing the optical calcs so do that first
        // to save time if there are any errors.
        do_deflection_updates(theta, phi);
        auto & system = get_system(theta, phi);

        auto absportances = get_solar_abs_front(theta, phi);
        auto solar_front_transmittance = get_solar_transmittance_front(theta, phi);

        system.setAbsorptances(absportances);
        return system.getSHGC(solar_front_transmittance);
    }

    std::vector<double> Glazing_System::layer_temperatures(Tarcog::ISO15099::System system_type,
                                                           double theta,
                                                           double phi)
    {
        // Don't do deflection updates if theta and phi are unchanged.  If this check is not
        // present then the CSystem m_solved value will get set to false causing the deflection
        // solver to go through another iteration resulting in slightly differnt temperatures
        // While these results are not incorrect they will not match the results from
        // Windows-CalcEngine unit tests.  And also if those temperates from the extra
        // iteration are used in the Stephen Morse code it will not result in the same
        // deflection values.
        //
        // The whole interaction involving do_deflection_updates needs to be refactored
        //
        // Doing deflection updates and creating the system before calculating the optical results
        // because there are cases where the thermal system cannot be created.  E.G. genSDF XML
        // files do not have conductivity and so can be used in optical calcs but not thermal.
        // Creating the system is much less expensive than doing the optical calcs so do that first
        // to save time if there are any errors.
        if(theta != last_theta || phi != last_phi)
        {
            do_deflection_updates(theta, phi);
        }
        auto & system = get_system(theta, phi);

        if(system_type == Tarcog::ISO15099::System::SHGC)
        {
			auto solar_front_absorptances = get_solar_abs_front(theta, phi);
            system.setAbsorptances(solar_front_absorptances);
        }
        return system.getTemperatures(system_type);
    }

    void Glazing_System::set_deflection_properties(double temperature_initial,
                                                   double pressure_initial)
    {
        initial_pressure = pressure_initial;
        initial_temperature = temperature_initial;
        do_deflection_updates(last_theta, last_phi);
    }

    void Glazing_System::set_applied_loads(std::vector<double> const & loads)
    {
        applied_loads = loads;
        if(current_system)
        {
            current_system.value().setAppliedLoad(applied_loads);
        }
    }

    Deflection_Results Glazing_System::calc_deflection_properties(
      Tarcog::ISO15099::System system_type, double theta, double phi)
    {
        do_deflection_updates(theta, phi);
        auto & system = get_system(theta, phi);
        auto deflection_max = system.getMaxDeflections(system_type);
        auto deflection_mean = system.getMeanDeflections(system_type);
        auto panes_load = system.getPanesLoad(system_type);
        return {deflection_max, deflection_mean, panes_load};
    }

    void Glazing_System::do_deflection_updates(double theta, double phi)
    {
        auto & system = get_system(theta, phi);
        if(model_deflection)
        {
            system.setDeflectionProperties(initial_temperature, initial_pressure);
        }
        else
        {
            system.clearDeflection();
        }
    }


    std::vector<double> Glazing_System::solid_layers_effective_conductivities(
      Tarcog::ISO15099::System system_type, double theta, double phi)
    {
        do_deflection_updates(theta, phi);
        auto & system = get_system(theta, phi);
        return system.getSolidEffectiveLayerConductivities(system_type);
    }
    std::vector<double> Glazing_System::gap_layers_effective_conductivities(
      Tarcog::ISO15099::System system_type, double theta, double phi)
    {
        do_deflection_updates(theta, phi);
        auto & system = get_system(theta, phi);
        return system.getGapEffectiveLayerConductivities(system_type);
    }
    double Glazing_System::system_effective_conductivity(Tarcog::ISO15099::System system_type,
                                                         double theta,
                                                         double phi)
    {
        do_deflection_updates(theta, phi);
        auto & system = get_system(theta, phi);
        return system.getEffectiveSystemConductivity(system_type);
    }
    double Glazing_System::relative_heat_gain(double theta, double phi)
    {
        // Doing deflection updates and creating the system before calculating the optical results
        // because there are cases where the thermal system cannot be created.  E.G. genSDF XML
        // files do not have conductivity and so can be used in optical calcs but not thermal.
        // Creating the system is much less expensive than doing the optical calcs so do that first
        // to save time if there are any errors.
        do_deflection_updates(theta, phi);
        auto & system = get_system(theta, phi);

		auto solar_front_transmittance = get_solar_transmittance_front(theta, phi);

        return system.relativeHeatGain(solar_front_transmittance);
    }

    void Glazing_System::optical_standard(window_standards::Optical_Standard const & s)
    {
        reset_igu();
        standard = s;
    }
    window_standards::Optical_Standard Glazing_System::optical_standard() const
    {
        return standard;
    }

    void Glazing_System::solid_layers(std::vector<Product_Data_Optical_Thermal> const & layers)
    {
        reset_igu();
        product_data = layers;
    }

    std::vector<Product_Data_Optical_Thermal> Glazing_System::solid_layers() const
    {
        return product_data;
    }

    void Glazing_System::sort_spectral_data()
    {
        for(auto & product : product_data)
        {
            auto nband_optical_data = std::dynamic_pointer_cast<Product_Data_N_Band_Optical>(
              product.optical_data->optical_data());
            if(nband_optical_data)
            {
                auto & measurements = nband_optical_data->wavelength_data;
                std::sort(measurements.begin(),
                          measurements.end(),
                          [](OpticsParser::WLData const & a, OpticsParser::WLData const & b) {
                              return a.wavelength < b.wavelength;
                          });
            }
        }
    }

    Glazing_System::Glazing_System(
      window_standards::Optical_Standard const & standard,
      std::vector<Product_Data_Optical_Thermal> const & product_data,
      std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> const & gap_values,
      double width,
      double height,
      double tilt,
      Environments const & environment,
      std::optional<SingleLayerOptics::BSDFHemisphere> const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & spectral_data_wavelength_range_method,
      int number_visible_bands,
      int number_solar_bands) :
        product_data(product_data),
        gap_values(gap_values),
        standard(standard),
        width(width),
        height(height),
        tilt(tilt),
        environment(environment),
        bsdf_hemisphere(bsdf_hemisphere),
        spectral_data_wavelength_range_method(spectral_data_wavelength_range_method),
        number_visible_bands(number_visible_bands),
        number_solar_bands(number_solar_bands)
    {
        sort_spectral_data();
    }

    Glazing_System::Glazing_System(
      window_standards::Optical_Standard const & standard,
      std::vector<OpticsParser::ProductData> const & product_data,
      std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> const & gap_values,
      double width,
      double height,
      double tilt,
      Environments const & environment,
      std::optional<SingleLayerOptics::BSDFHemisphere> const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & spectral_data_wavelength_range_method,
      int number_visible_bands,
      int number_solar_bands) :
        product_data(convert_to_solid_layers(product_data)),
        gap_values(gap_values),
        standard(standard),
        width(width),
        height(height),
        tilt(tilt),
        environment(environment),
        bsdf_hemisphere(bsdf_hemisphere),
        spectral_data_wavelength_range_method(spectral_data_wavelength_range_method),
        number_visible_bands(number_visible_bands),
        number_solar_bands(number_solar_bands)
    {
        sort_spectral_data();
    }

    std::vector<Product_Data_Optical_Thermal> create_solid_layers(
      std::vector<std::variant<OpticsParser::ProductData, Product_Data_Optical_Thermal>> const &
        product_data)
    {
        std::vector<Product_Data_Optical_Thermal> solid_layers;
        for(auto product : product_data)
        {
            Product_Data_Optical_Thermal * solid_layer =
              std::get_if<Product_Data_Optical_Thermal>(&product);
            if(solid_layer)
            {
                // If the variant was already holding a converted object use it
                solid_layers.push_back(*solid_layer);
            }
            else
            {
                // Otherwise the variant was holding OpticsParser::ProductData
                // Convert that and use it
                auto converted_layer =
                  convert_to_solid_layer(std::get<OpticsParser::ProductData>(product));
                solid_layers.push_back(converted_layer);
            }
        }
        return solid_layers;
    }

    Glazing_System::Glazing_System(
      window_standards::Optical_Standard const & standard,
      std::vector<std::variant<OpticsParser::ProductData, Product_Data_Optical_Thermal>> const &
        product_data,
      std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> const & gap_values,
      double width,
      double height,
      double tilt,
      Environments const & environment,
      std::optional<SingleLayerOptics::BSDFHemisphere> const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & spectral_data_wavelength_range_method,
      int number_visible_bands,
      int number_solar_bands) :
        product_data(create_solid_layers(product_data)),
        gap_values(gap_values),
        standard(standard),
        width(width),
        height(height),
        tilt(tilt),
        environment(environment),
        bsdf_hemisphere(bsdf_hemisphere),
        spectral_data_wavelength_range_method(spectral_data_wavelength_range_method),
        number_visible_bands(number_visible_bands),
        number_solar_bands(number_solar_bands)
    {
        sort_spectral_data();
    }

    Environments Glazing_System::environments() const
    {
        return environment;
    }

    void Glazing_System::environments(Environments const & environments)
    {
        environment = environments;
        reset_system();
    }

    void Glazing_System::set_width(double w)
    {
        width = w;
        if(current_system)
        {
            current_system.value().setWidth(width);
        }
    }

    void Glazing_System::set_height(double h)
    {
        height = h;
        if(current_system)
        {
            current_system.value().setHeight(height);
        }
    }

    void Glazing_System::set_tilt(double t)
    {
        tilt = t;
        if(current_system)
        {
            current_system.value().setTilt(tilt);
        }
    }

    void Glazing_System::flip_layer(size_t layer_index, bool flipped)
    {
        auto & layer = product_data.at(layer_index);
        layer.optical_data->flipped = flipped;
        layer.thermal_data->flipped = flipped;
        reset_igu();
    }

    void Glazing_System::set_spectral_data_wavelength_range(
      Spectal_Data_Wavelength_Range_Method const & type, int visible_bands, int solar_bands)
    {
        this->spectral_data_wavelength_range_method = type;
        this->number_visible_bands = visible_bands;
        this->number_solar_bands = solar_bands;
    }

    void Glazing_System::enable_deflection(bool enable)
    {
        model_deflection = enable;
        do_deflection_updates(last_theta, last_phi);
    }

    bool Glazing_System::isBSDF()
    {
        return bsdf_hemisphere.has_value();
    }


    SingleLayerOptics::IScatteringLayer & Glazing_System::get_optical_system_for_thermal_calcs()
    {
        if(optical_system_for_thermal_calcs == nullptr)
        {
            optical_system_for_thermal_calcs =
              optical_solar_results_system_needed_for_thermal_calcs(
                product_data,
                optical_standard(),
                bsdf_hemisphere,
                spectral_data_wavelength_range_method,
                number_visible_bands,
                number_solar_bands);
        }

        return *optical_system_for_thermal_calcs;
    }

    double Glazing_System::get_solar_transmittance_front(double theta, double phi)
    {
        auto & optical_system = get_optical_system_for_thermal_calcs();

        auto optical_layers = get_optical_layers(product_data);
        auto solar_method = standard.methods.at("SOLAR");
        std::vector<std::vector<double>> wavelengths = get_wavelengths(optical_layers);
        auto lambda_range = get_lambda_range(wavelengths, solar_method);

        return optical_system.getPropertySimple(lambda_range.min_lambda,
                                                lambda_range.max_lambda,
                                                FenestrationCommon::PropertySimple::T,
                                                FenestrationCommon::Side::Front,
                                                FenestrationCommon::Scattering::DirectHemispherical,
                                                theta,
                                                phi);
    }

    std::vector<double> Glazing_System::get_solar_abs_front(double theta, double phi)
    {
        auto & optical_system = get_optical_system_for_thermal_calcs();

        auto optical_layers = get_optical_layers(product_data);
        auto solar_method = standard.methods.at("SOLAR");
        std::vector<std::vector<double>> wavelengths = get_wavelengths(optical_layers);
        auto lambda_range = get_lambda_range(wavelengths, solar_method);

        return optical_system.getAbsorptanceLayersHeat(lambda_range.min_lambda,
                                                       lambda_range.max_lambda,
                                                       FenestrationCommon::Side::Front,
                                                       FenestrationCommon::ScatteringSimple::Direct,
                                                       theta,
                                                       phi);
    }


}   // namespace wincalc
