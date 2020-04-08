#include <sstream>

#include "glazing_system.h"
#include "optical_calcs.h"
#include "util.h"
#include "create_wce_objects.h"

namespace wincalc
{
#pragma warning(push)
#pragma warning(disable : 4100)
    WCE_Optical_Results Glazing_System_Optical::all_method_values(
      window_standards::Optical_Standard_Method_Type const & method_type,
      double theta,
      double phi) const
    {
        auto method = get_method(method_type);
        return calc_all(solid_layers_optical, method, theta, phi);
    }

    WCE_Color_Results Glazing_System_Optical::color(double theta, double phi) const
    {
        window_standards::Optical_Standard_Method tristim_x =
          get_method(window_standards::Optical_Standard_Method_Type::COLOR_TRISTIMX);
        window_standards::Optical_Standard_Method tristim_y =
          get_method(window_standards::Optical_Standard_Method_Type::COLOR_TRISTIMY);
        window_standards::Optical_Standard_Method tristim_z =
          get_method(window_standards::Optical_Standard_Method_Type::COLOR_TRISTIMZ);
        return calc_color(solid_layers_optical, tristim_x, tristim_y, tristim_z, theta, phi);
    }
#pragma warning(pop)

    window_standards::Optical_Standard_Method Glazing_System_Optical_Interface::get_method(
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

    Glazing_System_Thermal::Glazing_System_Thermal(
      std::vector<std::shared_ptr<wincalc::Product_Data_Thermal>> const & products,
      std::vector<Engine_Gap_Info> const & gap_layers,
      double width,
      double height,
      Environments const & environment) :
        igu(create_igu(products, gap_layers, width, height)),
        system(create_system(igu, environment))
    {}

    double Glazing_System_Thermal::u()
    {
        return system.getUValue();
    }

    double Glazing_System_Thermal::shgc(std::vector<double> const & absorptances_front,
                                        double total_solar_transmittance)
    {
        system.setAbsorptances(absorptances_front);
        return system.getSHGC(total_solar_transmittance);
    }
    std::vector<double>
      Glazing_System_Thermal::layer_temperatures(Tarcog::ISO15099::System system_type,
                                                 std::vector<double> const & absorptances_front)
    {
        system.setAbsorptances(absorptances_front);
        return system.getTemperatures(system_type);
    }
    std::vector<double> Glazing_System_Thermal::solid_layers_effective_conductivities(
      Tarcog::ISO15099::System system_type)
    {
        return system.getSolidEffectiveLayerConductivities(system_type);
    }
    std::vector<double> Glazing_System_Thermal::gap_layers_effective_conductivities(
      Tarcog::ISO15099::System system_type)
    {
        return system.getGapEffectiveLayerConductivities(system_type);
    }
    double
      Glazing_System_Thermal::system_effective_conductivity(Tarcog::ISO15099::System system_type)
    {
        return system.getEffectiveSystemConductivity(system_type);
    }
    double Glazing_System_Thermal::relative_heat_gain(double solar_transmittance)
    {
        return system.relativeHeatGain(solar_transmittance);
    }

    Glazing_System_Optical_Interface::Glazing_System_Optical_Interface(
      window_standards::Optical_Standard const & standard) :
        standard(standard)
    {}
    void Glazing_System_Optical_Interface::optical_standard(
      window_standards::Optical_Standard const & s)
    {
        standard = s;
    }
    window_standards::Optical_Standard Glazing_System_Optical_Interface::optical_standard() const
    {
        return standard;
    }
    Glazing_System_Optical::Glazing_System_Optical(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & solid_layers,
      window_standards::Optical_Standard const & standard) :
        Glazing_System_Optical_Interface(standard), solid_layers_optical(solid_layers)
    {}


    Glazing_System_Thermal_And_Optical::Glazing_System_Thermal_And_Optical(
      std::vector<Product_Data_Optical_Thermal> const & product_data,
      std::vector<Engine_Gap_Info> const & gap_values,
      window_standards::Optical_Standard const & standard,
      double width,
      double height,
      Environments const & environment) :
        Glazing_System_Optical(get_optical_layers(product_data), standard),
        Glazing_System_Thermal(
          get_thermal_layers(product_data), gap_values, width, height, environment)
    {}

    Glazing_System_Thermal_And_Optical::Glazing_System_Thermal_And_Optical(
      std::vector<std::shared_ptr<OpticsParser::ProductData>> const & product_data,
      std::vector<Engine_Gap_Info> const & gap_values,
      window_standards::Optical_Standard const & standard,
      double width,
      double height,
      Environments const & environment) :
        Glazing_System_Optical(get_optical_layers(product_data), standard),
        Glazing_System_Thermal(
          get_thermal_layers(product_data), gap_values, width, height, environment)
    {}

    double Glazing_System_Thermal_And_Optical::shgc(double theta, double phi)
    {
        auto optical_results = optical_results_needed_for_thermal_calcs(
          solid_layers_optical, optical_standard(), theta, phi);

        return Glazing_System_Thermal::shgc(optical_results.layer_solar_absorptances,
                                            optical_results.total_solar_transmittance);
    }

    std::vector<double> Glazing_System_Thermal_And_Optical::layer_temperatures(
      Tarcog::ISO15099::System system_type, double theta, double phi)
    {
        auto optical_results = optical_results_needed_for_thermal_calcs(
          solid_layers_optical, optical_standard(), theta, phi);

        return Glazing_System_Thermal::layer_temperatures(system_type,
                                                          optical_results.layer_solar_absorptances);
    }
}   // namespace wincalc
