#include <sstream>

#include "glazing_system.h"
#include "thermal_calcs.h"
#include "optical_calcs.h"
#include "util.h"

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
      std::vector<Gap_Data> const & gap_layers,
      double width,
      double height,
      Environments const & u_environment,
      Environments const & shgc_environment) :
        solid_layers_thermal(products),
        gap_layers(convert(gap_layers)),
        width(width),
        height(height),
        u_environment(u_environment),
        shgc_environment(shgc_environment)
    {}

    Glazing_System_Thermal::Glazing_System_Thermal(
      std::vector<std::shared_ptr<wincalc::Product_Data_Thermal>> const & products,
      std::vector<Engine_Gap_Info> const & gap_layers,
      double width,
      double height,
      Environments const & u_environment,
      Environments const & shgc_environment) :
        solid_layers_thermal(products),
        gap_layers(gap_layers),
        width(width),
        height(height),
        u_environment(u_environment),
        shgc_environment(shgc_environment)
    {}

#pragma warning(push)
#pragma warning(disable : 4100)
    double Glazing_System_Thermal::u(double theta, double phi) const
    {
        std::vector<double> solar_absorptances(
          solid_layers_thermal.size(), 0);   // U-value calculations do not use solar absorptances
        return calc_u(
          solid_layers_thermal, solar_absorptances, gap_layers, width, height, u_environment);
    }

    double Glazing_System_Thermal::shgc(std::vector<double> const & absorptances_front,
                                        double total_solar_transmittance,
                                        double theta,
                                        double phi) const
    {
        return calc_shgc(solid_layers_thermal,
                         absorptances_front,
                         gap_layers,
                         total_solar_transmittance,
                         width,
                         height,
                         shgc_environment);
    }
#pragma warning(pop)

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
      std::vector<Gap_Data> const & gap_values,
      window_standards::Optical_Standard const & standard,
      double width,
      double height,
      Environments const & u_environment,
      Environments const & shgc_environment) :
        Glazing_System_Optical(get_optical_layers(product_data), standard),
        Glazing_System_Thermal(get_thermal_layers(product_data),
                               gap_values,
                               width,
                               height,
                               u_environment,
                               shgc_environment)
    {}

    Glazing_System_Thermal_And_Optical::Glazing_System_Thermal_And_Optical(
      std::vector<OpticsParser::ProductData> const & product_data,
      std::vector<Gap_Data> const & gap_values,
      window_standards::Optical_Standard const & standard,
      double width,
      double height,
      Environments const & u_environment,
      Environments const & shgc_environment) :
        Glazing_System_Optical(get_optical_layers(product_data), standard),
        Glazing_System_Thermal(get_thermal_layers(product_data),
                               gap_values,
                               width,
                               height,
                               u_environment,
                               shgc_environment)
    {}

    double Glazing_System_Thermal_And_Optical::shgc(double theta, double phi) const
    {
        auto optical_results = optical_results_needed_for_thermal_calcs(
          solid_layers_optical, optical_standard(), theta, phi);

		return Glazing_System_Thermal::shgc(optical_results.layer_solar_absorptances,
                    optical_results.total_solar_transmittance,
                    theta,
                    phi);
    }


    std::vector<Product_Data_Optical_Thermal>
      Glazing_System_Thermal_And_Optical::optical_and_thermal_data() const
    {
        std::vector<Product_Data_Optical_Thermal> layers;
        for(size_t i = 0; i < solid_layers_optical.size(); ++i)
        {
            layers.push_back(
              Product_Data_Optical_Thermal{solid_layers_optical[i], solid_layers_thermal[i]});
        }
        return layers;
    }
}   // namespace wincalc
