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
        LOGMSG("begin Glazing_System::optical_method_results(" + method_name + ", "
               + std::to_string(theta) + ", " + std::to_string(phi) + ")");

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
        LOGMSG("before std::find");
        if(std::find(color_methods.begin(), color_methods.end(), method_name)
           != color_methods.end())
        {
            LOGMSG("in std::find");
            throw std::runtime_error(
              "Individual tristimulus color optical methods should not be calculated using the "
              "generic method for optical calculations.  The color function which takes the X, Y "
              "and Z optical methods should be used instead.  If results from individual tristulus "
              "methods are absolutely required rename the tristimuls method in the optical "
              "standard file but care should be taken in interpreting any results calculated this "
              "way.");
        }
        LOGMSG("before get_method");
        auto method = get_method(method_name);
        LOGMSG("before calc_all");
        auto result = calc_all(get_optical_layers(product_data),
                               method,
                               theta,
                               phi,
                               bsdf_hemisphere,
                               spectral_data_wavelength_range_method,
                               number_visible_bands,
                               number_solar_bands);
        LOGMSG("end Glazing_System::optical_method_results(" + method_name + ", "
               + std::to_string(theta) + ", " + std::to_string(phi) + ")");
        return result;
    }

    WCE_Color_Results Glazing_System::color(double theta,
                                            double phi,
                                            std::string const & tristimulus_x_method,
                                            std::string const & tristimulus_y_method,
                                            std::string const & tristimulus_z_method) const
    {
        LOGMSG("begin Glazing_System::color(" + std::to_string(theta) + ", " + std::to_string(phi)
               + ")");
        window_standards::Optical_Standard_Method tristim_x = get_method(tristimulus_x_method);
        LOGMSG("before get_method Y");
        window_standards::Optical_Standard_Method tristim_y = get_method(tristimulus_y_method);
        LOGMSG("before get_method Z");
        window_standards::Optical_Standard_Method tristim_z = get_method(tristimulus_z_method);
        LOGMSG("before calc_color");
        auto result = calc_color(get_optical_layers(product_data),
                                 tristim_x,
                                 tristim_y,
                                 tristim_z,
                                 theta,
                                 phi,
                                 bsdf_hemisphere,
                                 spectral_data_wavelength_range_method,
                                 number_visible_bands,
                                 number_solar_bands);
        LOGMSG("end Glazing_System::color(" + std::to_string(theta) + ", " + std::to_string(phi)
               + ")");
        return result;
    }

    void Glazing_System::reset_igu()
    {
        LOGMSG("begin Glazing_System::reset_igu");
        optical_system_for_thermal_calcs = nullptr;
        current_igu = std::nullopt;
        LOGMSG("before reset_system");
        reset_system();
        LOGMSG("end Glazing_System::reset_igu");
    }

    void Glazing_System::reset_system()
    {
        LOGMSG("begin Glazing_System::reset_system");
        current_system = std::nullopt;
        LOGMSG("end Glazing_System::reset_system");
    }

    window_standards::Optical_Standard_Method
      Glazing_System::get_method(std::string const & method_name) const
    {
        LOGMSG("begin Glazing_System::get_method(" + method_name + ")");
        auto method_itr = standard.methods.find(method_name);
        LOGMSG("before if(method_itr == standard.methods.end())");
        if(method_itr == standard.methods.end())
        {
            LOGMSG("in if(method_itr == standard.methods.end())");
            std::stringstream err_msg;
            err_msg << "Standard " << standard.name << " does not include a " << method_name
                    << " method";
            throw std::runtime_error(err_msg.str());
        }
        LOGMSG("end Glazing_System::get_method(" + method_name + ")");
        return method_itr->second;
    }

    Tarcog::ISO15099::CIGU & Glazing_System::get_igu(double theta, double phi)
    {
        LOGMSG("begin Glazing_System::get_igu(" + std::to_string(theta) + ", " + std::to_string(phi)
               + ")");
        if(current_igu.has_value() && is_current_igu_calculated(theta, phi))
        {
            LOGMSG("in if(current_igu.has_value() && is_current_igu_calculated(theta, phi))");
            return current_igu.value();
        }
        else
        {
            LOGMSG("in else branch of if(current_igu.has_value() && "
                   "is_current_igu_calculated(theta, phi))");
            current_igu = create_igu(product_data, gap_values, width, height, tilt, standard);
            LOGMSG("before if(!applied_loads.empty())");
            if(!applied_loads.empty())
            {
                LOGMSG("in if(!applied_loads.empty())");
                current_igu.value().setAppliedLoad(applied_loads);
            }
            LOGMSG("end of else branch");
            return current_igu.value();
        }
    }

    Tarcog::ISO15099::CSystem & Glazing_System::get_system(double theta, double phi)
    {
        LOGMSG("begin Glazing_System::get_system(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        if(current_system.has_value() && is_current_igu_calculated(theta, phi))
        {
            LOGMSG("in if(current_system.has_value() && is_current_igu_calculated(theta, phi))");
            return current_system.value();
        }
        else
        {
            LOGMSG("in else branch of if(current_system.has_value() && "
                   "is_current_igu_calculated(theta, phi))");
            auto & igu = get_igu(theta, phi);
            LOGMSG("before create_system");
            current_system = create_system(igu, environment);
            last_theta = theta;
            last_phi = phi;
            LOGMSG("end else branch of if(current_system.has_value() && "
                   "is_current_igu_calculated(theta, phi))");

            return current_system.value();
        }
    }

    double Glazing_System::u(double theta, double phi)
    {
        LOGMSG("begin Glazing_System::u(" + std::to_string(theta) + ", " + std::to_string(phi)
               + ")");
        do_updates_for_thermal(theta, phi);
        LOGMSG("before get_system");
        auto & system = get_system(theta, phi);
        LOGMSG("before getUValue");
        auto result = system.getUValue();
        LOGMSG("end Glazing_System::u(" + std::to_string(theta) + ", " + std::to_string(phi) + ")");
        return result;
    }

    double Glazing_System::shgc(double theta, double phi)
    {
        // Doing deflection updates and creating the system before calculating the optical results
        // because there are cases where the thermal system cannot be created.  E.G. genSDF XML
        // files do not have conductivity and so can be used in optical calcs but not thermal.
        // Creating the system is much less expensive than doing the optical calcs so do that first
        // to save time if there are any errors.

        LOGMSG("begin Glazing_System::shgc(" + std::to_string(theta) + ", " + std::to_string(phi)
               + ")");

        do_updates_for_thermal(theta, phi);
        LOGMSG("befoe get_solar_transmittance_front");
        auto solar_front_transmittance = get_solar_transmittance_front(theta, phi);
        LOGMSG("befoe get_system");
        auto & system = get_system(theta, phi);
        LOGMSG("befoe system.getSHGC");
        auto result = system.getSHGC(solar_front_transmittance);
        LOGMSG("end Glazing_System::shgc(" + std::to_string(theta) + ", " + std::to_string(phi)
               + ")");
        return result;
    }

    std::vector<double> Glazing_System::layer_temperatures(Tarcog::ISO15099::System system_type,
                                                           double theta,
                                                           double phi)
    {
        LOGMSG("begin Glazing_System::layer_temperatures(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        do_updates_for_thermal(theta, phi);
        LOGMSG("before get_system");
        auto & system = get_system(theta, phi);
        LOGMSG("before system.getTemperatures");
        auto result = system.getTemperatures(system_type);
        LOGMSG("end Glazing_System::layer_temperatures(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        return result;
    }

    void Glazing_System::set_deflection_properties(double temperature_at_construction,
                                                   double pressure_at_construction)
    {
        LOGMSG("begin Glazing_System::set_deflection_properties("
               + std::to_string(pressure_at_construction) + ", "
               + std::to_string(temperature_at_construction) + ")");
        deflection_properties =
          Temperature_Pressure{temperature_at_construction, pressure_at_construction};
        LOGMSG("before do_deflection_updates");
        do_deflection_updates(last_theta, last_phi);
        LOGMSG("end Glazing_System::set_deflection_properties("
               + std::to_string(pressure_at_construction) + ", "
               + std::to_string(temperature_at_construction) + ")");
    }

    void
      Glazing_System::set_deflection_properties(const std::vector<double> & measured_deflected_gaps)
    {
        LOGMSG("begin Glazing_System::set_deflection_properties");
        deflection_properties = measured_deflected_gaps;
        do_deflection_updates(last_theta, last_phi);
        LOGMSG("end Glazing_System::set_deflection_properties");
    }

    void Glazing_System::set_applied_loads(std::vector<double> const & loads)
    {
        LOGMSG("begin Glazing_System::set_applied_loads");
        applied_loads = loads;
        if(current_system)
        {
            LOGMSG("in if(current_system)");
            current_system.value().setAppliedLoad(applied_loads);
        }
        LOGMSG("end Glazing_System::set_applied_loads");
    }

    Deflection_Results Glazing_System::calc_deflection_properties(
      Tarcog::ISO15099::System system_type, double theta, double phi)
    {
        LOGMSG("begin Glazing_System::calc_deflection_properties(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        do_updates_for_thermal(theta, phi);
        LOGMSG("before get_system");
        auto & system = get_system(theta, phi);
        LOGMSG("before getMaxLayerDeflections");
        const auto layer_deflection_max = system.getMaxLayerDeflections(system_type);
        LOGMSG("before getMeanLayerDeflections");
        const auto layer_deflection_mean = system.getMeanLayerDeflections(system_type);
        LOGMSG("before getPanesLoad");
        const auto panes_load = system.getPanesLoad(system_type);
        LOGMSG("before getMaxGapWidth");
        const auto gap_deflection_max = system.getMaxGapWidth(system_type);
        LOGMSG("before getMeanGapWidth");
        const auto gap_deflection_mean = system.getMeanGapWidth(system_type);
        auto result = Deflection_Results{layer_deflection_max,
                                         layer_deflection_mean,
                                         panes_load,
                                         gap_deflection_max,
                                         gap_deflection_mean};
        LOGMSG("end Glazing_System::calc_deflection_properties(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        return result;
    }

    void Glazing_System::do_deflection_updates(double theta, double phi)
    {
        LOGMSG("begin Glazing_System::do_deflection_updates(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        auto & system = get_system(theta, phi);
        if(model_deflection)
        {
            LOGMSG("in if(model_deflection)");
            if(auto state_ptr = std::get_if<Temperature_Pressure>(&deflection_properties))
            {
                system.setDeflectionProperties(state_ptr->temperature, state_ptr->pressure);
            }
            else if(auto vector_ptr = std::get_if<std::vector<double>>(&deflection_properties))
            {
                system.setDeflectionProperties(*vector_ptr);
            }
        }
        else
        {
            LOGMSG("in else calling system.clearDeflection();");
            system.clearDeflection();
        }
        LOGMSG("end Glazing_System::do_deflection_updates(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
    }

    void Glazing_System::do_deflection_updates(std::optional<double> theta,
                                               std::optional<double> phi)
    {
        if(theta.has_value() && phi.has_value())
        {
            do_deflection_updates(theta.value(), phi.value());
        }
    }

    void Glazing_System::do_layer_absorptance_updates(double theta, double phi)
    {
        LOGMSG("begin Glazing_System::do_layer_absorptance_updates(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        auto & system = get_system(theta, phi);
        LOGMSG("before get_solar_abs_front");
        auto solar_front_absorptances = get_solar_abs_front(theta, phi);
        LOGMSG("before system.setAbsorptances");
        system.setAbsorptances(solar_front_absorptances);
        LOGMSG("end Glazing_System::do_layer_absorptance_updates(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
    }

    void Glazing_System::do_updates_for_thermal(double theta, double phi)
    {
        LOGMSG("begin Glazing_System::do_updates_for_thermal(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        do_deflection_updates(theta, phi);
        LOGMSG("before do_layer_absorptance_updates");
        do_layer_absorptance_updates(theta, phi);
        LOGMSG("end Glazing_System::do_updates_for_thermal(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
    }

    std::vector<double> Glazing_System::solid_layers_effective_conductivities(
      Tarcog::ISO15099::System system_type, double theta, double phi)
    {
        LOGMSG("begin Glazing_System::solid_layers_effective_conductivities("
               + std::to_string(theta) + ", " + std::to_string(phi) + ")");
        do_updates_for_thermal(theta, phi);
        LOGMSG("before get_system");
        auto & system = get_system(theta, phi);
        LOGMSG("before getSolidEffectiveLayerConductivities");
        auto result = system.getSolidEffectiveLayerConductivities(system_type);
        LOGMSG("end Glazing_System::solid_layers_effective_conductivities(" + std::to_string(theta)
               + ", " + std::to_string(phi) + ")");
        return result;
    }
    std::vector<double> Glazing_System::gap_layers_effective_conductivities(
      Tarcog::ISO15099::System system_type, double theta, double phi)
    {
        do_updates_for_thermal(theta, phi);
        auto & system = get_system(theta, phi);
        return system.getGapEffectiveLayerConductivities(system_type);
    }
    double Glazing_System::system_effective_conductivity(Tarcog::ISO15099::System system_type,
                                                         double theta,
                                                         double phi)
    {
        do_updates_for_thermal(theta, phi);
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
        do_updates_for_thermal(theta, phi);
        auto & system = get_system(theta, phi);

        auto solar_front_transmittance = get_solar_transmittance_front(theta, phi);

        return system.relativeHeatGain(solar_front_transmittance);
    }

    void Glazing_System::optical_standard(window_standards::Optical_Standard const & s)
    {
        LOGMSG("begin Glazing_System::optical_standard");
        reset_igu();
        standard = s;
        LOGMSG("end Glazing_System::optical_standard");
    }
    window_standards::Optical_Standard Glazing_System::optical_standard() const
    {
        return standard;
    }

    void Glazing_System::solid_layers(std::vector<Product_Data_Optical_Thermal> const & layers)
    {
        LOGMSG("begin Glazing_System::solid_layers");
        reset_igu();
        product_data = layers;
        LOGMSG("end Glazing_System::solid_layers");
    }

    std::vector<Product_Data_Optical_Thermal> Glazing_System::solid_layers() const
    {
        return product_data;
    }

    void Glazing_System::gap_layers(
      std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> const & layers)
    {
        LOGMSG("begin Glazing_System::gap_layers");
        reset_igu();
        gap_values = layers;
        LOGMSG("end Glazing_System::gap_layers");
    }

    std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> Glazing_System::gap_layers() const
    {
        return gap_values;
    }

    void Glazing_System::sort_spectral_data()
    {
        LOGMSG("begin Glazing_System::sort_spectral_data");
        for(auto & product : product_data)
        {
            LOGMSG("in for(auto & product : product_data)");
            auto nband_optical_data = std::dynamic_pointer_cast<Product_Data_N_Band_Optical>(
              product.optical_data->optical_data());
            if(nband_optical_data)
            {
                LOGMSG("in if(nband_optical_data)");
                auto & measurements = nband_optical_data->wavelength_data;
                std::sort(measurements.begin(),
                          measurements.end(),
                          [](OpticsParser::WLData const & a, OpticsParser::WLData const & b) {
                              return a.wavelength < b.wavelength;
                          });
            }
        }
        LOGMSG("end Glazing_System::sort_spectral_data");
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
        LOGMSG("begin create_solid_layers");
        std::vector<Product_Data_Optical_Thermal> solid_layers;
        for(auto product : product_data)
        {
            LOGMSG("in for(auto product : product_data)");
            Product_Data_Optical_Thermal * solid_layer =
              std::get_if<Product_Data_Optical_Thermal>(&product);
            if(solid_layer)
            {
                LOGMSG("in if(solid_layer)");
                // If the variant was already holding a converted object use it
                solid_layers.push_back(*solid_layer);
            }
            else
            {
                LOGMSG("in else");
                // Otherwise the variant was holding OpticsParser::ProductData
                // Convert that and use it
                auto converted_layer =
                  convert_to_solid_layer(std::get<OpticsParser::ProductData>(product));
                LOGMSG("before solid_layers.push_back");
                solid_layers.push_back(converted_layer);
            }
        }
        LOGMSG("end create_solid_layers");
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
        LOGMSG("begin Glazing_System::environments");
        environment = environments;
        reset_system();
        LOGMSG("end Glazing_System::environments");
    }

    void Glazing_System::set_width(double w)
    {
        LOGMSG("begin Glazing_System::set_width(" + std::to_string(w) + ")");
        width = w;
        if(current_system)
        {
            LOGMSG("in if(current_system)");
            current_system.value().setWidth(width);
        }
        LOGMSG("end Glazing_System::set_width(" + std::to_string(w) + ")");
    }

    void Glazing_System::set_height(double h)
    {
        LOGMSG("begin Glazing_System::set_height(" + std::to_string(h) + ")");
        height = h;
        if(current_system)
        {
            LOGMSG("in if(current_system)");
            current_system.value().setHeight(height);
        }
        LOGMSG("end Glazing_System::set_height(" + std::to_string(h) + ")");
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
        LOGMSG("begin Glazing_System::set_spectral_data_wavelength_range");
        if(this->spectral_data_wavelength_range_method != type
           || this->number_visible_bands != visible_bands
           || this->number_solar_bands != solar_bands)
        {
            LOGMSG("in if(this->spectral_data_wavelength_range_method etc...");
            this->spectral_data_wavelength_range_method = type;
            this->number_visible_bands = visible_bands;
            this->number_solar_bands = solar_bands;
            this->optical_system_for_thermal_calcs = nullptr;
        }
        LOGMSG("end Glazing_System::set_spectral_data_wavelength_range");
    }

    void Glazing_System::enable_deflection(bool enable)
    {
        model_deflection = enable;

        // If last theta and phi are not set then set them to 0.0 for the deflection updates
        if(!last_theta.has_value())
        {
            last_theta = 0.0;
        }
        if(!last_phi.has_value())
        {
            last_phi = 0.0;
        }

        do_deflection_updates(last_theta, last_phi);
    }

    bool Glazing_System::isBSDF()
    {
        return bsdf_hemisphere.has_value();
    }

    double Glazing_System::overallThickness() const
    {
        LOGMSG("begin Glazing_System::overallThickness");
        if(current_igu.has_value())
        {
            LOGMSG("in if(current_igu.has_value()) returning "
                   + std::to_string(current_igu.value().getThickness()));
            return current_igu.value().getThickness();
        }
        LOGMSG("not in if(current_igu.has_value()) returning 0.0");
        return 0.0;
    }

    void Glazing_System::set_solid_layer_conductivites(const std::vector<double> & conductivities)
    {
        if(current_igu)
        {
            current_igu->setSolidLayerConductivities(conductivities);
        }

        if(current_system)
        {
            current_system->setSolidLayerConductivities(conductivities);
        }

        if(conductivities.size() != product_data.size())
        {
            throw std::runtime_error("Size of conductivities does not match the "
                                     "number of product_data layers.");
        }

        for(size_t i = 0; i < conductivities.size(); i++)
        {
            product_data[i].thermal_data->conductivity = conductivities[i];
        }
    }

    void Glazing_System::set_solid_layer_conductivity(size_t index, double conductivity)
    {
        if(current_igu)
        {
            current_igu->setSolidLayerConductivity(index, conductivity);
        }

        if(current_system)
        {
            current_system->setSolidLayerConductivity(index, conductivity);
        }

        if(index >= product_data.size())
        {
            throw std::runtime_error(
              "Index out of bounds: The provided index exceeds the number of available layers.");
        }

        product_data[index].thermal_data->conductivity = conductivity;
    }

    Tarcog::ISO15099::CSystem & Glazing_System::get_thermal_system(double theta, double phi)
    {
        do_updates_for_thermal(theta, phi);
        return get_system(theta, phi);
    }

    SingleLayerOptics::IScatteringLayer & Glazing_System::get_optical_system_for_thermal_calcs()
    {
        LOGMSG("begin Glazing_System::get_optical_system_for_thermal_calcs");
        if(optical_system_for_thermal_calcs == nullptr)
        {
            LOGMSG("in if(optical_system_for_thermal_calcs == nullptr)");
            optical_system_for_thermal_calcs =
              optical_solar_results_system_needed_for_thermal_calcs(
                product_data,
                optical_standard(),
                bsdf_hemisphere,
                spectral_data_wavelength_range_method,
                number_visible_bands,
                number_solar_bands);
        }

        LOGMSG("end Glazing_System::get_optical_system_for_thermal_calcs");
        return *optical_system_for_thermal_calcs;
    }

    double Glazing_System::get_solar_transmittance_front(double theta, double phi)
    {
        LOGMSG("begin Glazing_System::get_solar_transmittance_front(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        auto & optical_system = get_optical_system_for_thermal_calcs();
        LOGMSG("before get_optical_layers");
        auto optical_layers = get_optical_layers(product_data);
        LOGMSG("before standard.methods.at(SOLAR)");
        auto solar_method = standard.methods.at("SOLAR");
        LOGMSG("before get_wavelengths");
        std::vector<std::vector<double>> wavelengths = get_wavelengths(optical_layers);
        LOGMSG("before get_lambda_range");
        auto lambda_range = get_lambda_range(wavelengths, solar_method);
        LOGMSG("before optical_system.getPropertySimple");
        auto result =
          optical_system.getPropertySimple(lambda_range.min_lambda,
                                           lambda_range.max_lambda,
                                           FenestrationCommon::PropertySimple::T,
                                           FenestrationCommon::Side::Front,
                                           FenestrationCommon::Scattering::DirectHemispherical,
                                           theta,
                                           phi);
        LOGMSG("end Glazing_System::get_solar_transmittance_front(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        return result;
    }

    bool Glazing_System::is_current_igu_calculated(double theta, double phi)
    {
        LOGMSG("begin Glazing_System::is_current_igu_calculated(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        bool result = last_theta.has_value()
                      && FenestrationCommon::isEqual(theta, last_theta.value())
                      && last_phi.has_value() && FenestrationCommon::isEqual(phi, last_phi.value());
        LOGMSG("begin Glazing_System::is_current_igu_calculated(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")" + " returning " + std::to_string(result));
        return result;
    }

    std::vector<double> Glazing_System::get_solar_abs_front(double theta, double phi)
    {
        LOGMSG("begin Glazing_System::get_solar_abs_front(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        auto & optical_system = get_optical_system_for_thermal_calcs();
        LOGMSG("before get_optical_layers");
        auto optical_layers = get_optical_layers(product_data);
        LOGMSG("before standard.methods.at(SOLAR)");
        auto solar_method = standard.methods.at("SOLAR");
        LOGMSG("before get_wavelengths");
        std::vector<std::vector<double>> wavelengths = get_wavelengths(optical_layers);
        LOGMSG("before get_lambda_range");
        auto lambda_range = get_lambda_range(wavelengths, solar_method);
        LOGMSG("before optical_system.getAbsorptanceLayersHeat");
        auto result =
          optical_system.getAbsorptanceLayersHeat(lambda_range.min_lambda,
                                                  lambda_range.max_lambda,
                                                  FenestrationCommon::Side::Front,
                                                  FenestrationCommon::ScatteringSimple::Direct,
                                                  theta,
                                                  phi);

        LOGMSG("end Glazing_System::get_solar_abs_front(" + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        return result;
    }

    double Glazing_System::heat_flow(Tarcog::ISO15099::System system_type,
                                     Tarcog::ISO15099::Environment env,
                                     double theta,
                                     double phi)
    {
        do_updates_for_thermal(theta, phi);
        auto & system = get_system(theta, phi);
        return system.getHeatFlow(system_type, env);
    }

    double Glazing_System::h(Tarcog::ISO15099::System system_type,
                             Tarcog::ISO15099::Environment env,
                             double theta,
                             double phi)
    {
        do_updates_for_thermal(theta, phi);
        auto & system = get_system(theta, phi);
        return system.getH(system_type, env);
    }

    std::vector<double>
      Glazing_System::radiosities(Tarcog::ISO15099::System system_type, double theta, double phi)
    {
        do_updates_for_thermal(theta, phi);
        auto & system = get_system(theta, phi);
        return system.getRadiosities(system_type);
    }

}   // namespace wincalc
