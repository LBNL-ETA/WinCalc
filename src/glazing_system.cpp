#include <sstream>

#include "glazing_system.h"
#include "thermal_calcs_from_measured_data_source.h"
#include "optical_calcs.h"

namespace wincalc
{
#if 0
    Glazing_System::Glazing_System(std::vector<OpticsParser::ProductData> const & solid_layers,
                                   std::vector<Gap_Data> const & gap_layers,
                                   window_standards::Optical_Standard const & standard,
                                   double width,
                                   double height,
                                   Environments const & u_environment,
                                   Environments const & shgc_environment) :
        solid_layers(solid_layers),
        gap_layers(gap_layers),
        standard(standard),
        width(width),
        height(height),
        u_environment(u_environment),
        shgc_environment(shgc_environment)
    {}

    Thermal_Result Glazing_System::u() const
    {
        return calc_u(solid_layers, gap_layers, standard, width, height, u_environment);
    }

    Thermal_Result Glazing_System::shgc() const
    {
        return calc_shgc(solid_layers, gap_layers, standard, width, height, shgc_environment);
    }


    WCE_Simple_Result Glazing_System::all_method_values(
      window_standards::Optical_Standard_Method_Type const & method_type) const
    {
        auto method = get_method(method_type);
        return calc_all(solid_layers, method);
    }

    WCE_Color_Result Glazing_System::color() const
    {
        window_standards::Optical_Standard_Method tristim_x =
          get_method(window_standards::Optical_Standard_Method_Type::COLOR_TRISTIMX);
        window_standards::Optical_Standard_Method tristim_y =
          get_method(window_standards::Optical_Standard_Method_Type::COLOR_TRISTIMY);
        window_standards::Optical_Standard_Method tristim_z =
          get_method(window_standards::Optical_Standard_Method_Type::COLOR_TRISTIMZ);
        return calc_color(solid_layers, tristim_x, tristim_y, tristim_z);
    }
#endif

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
        products(products),
        gap_layers(gap_layers),
        width(width),
        height(height),
        u_environment(u_environment),
        shgc_environment(shgc_environment)
    {}

    Thermal_Result Glazing_System_Thermal::u(double theta, double phi) const
    {
        return Thermal_Result();
    }

    Thermal_Result Glazing_System_Thermal::shgc(std::vector<double> const & absorptances_front,
                                                double theta,
                                                double phi) const
    {
        return Thermal_Result();
    }

    Thermal_Result Glazing_System_Thermal::shgc(double theta, double phi) const
    {
        std::vector<double> absorptances(products.size(), 0);
        return shgc(absorptances, theta, phi);
    };

    Glazing_System_Optical_Interface::Glazing_System_Optical_Interface(
      window_standards::Optical_Standard const & standard) :
        standard(standard)
    {}
    void Glazing_System_Optical_Interface::optical_standard(
      window_standards::Optical_Standard const & s)
    {
        standard = s;
    }
    window_standards::Optical_Standard Glazing_System_Optical_Interface::optical_standard()
    {
        return standard;
    }
    Glazing_System_Optical::Glazing_System_Optical(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & solid_layers,
      window_standards::Optical_Standard const & standard) :
        Glazing_System_Optical_Interface(standard),
        solid_layers(solid_layers)
    {}

    std::vector<std::shared_ptr<Product_Data_Optical>>
      get_optical_data(std::vector<Product_Data_Thermal_Optical> const & product_data)
    {
        std::vector<std::shared_ptr<Product_Data_Optical>> data;
        for(auto layer : product_data)
        {
            data.push_back(layer.optical_data);
        }
        return data;
    }

    std::vector<std::shared_ptr<Product_Data_Thermal>>
      get_thermal_data(std::vector<Product_Data_Thermal_Optical> const & product_data)
    {
        std::vector<std::shared_ptr<Product_Data_Thermal>> data;
        for(auto layer : product_data)
        {
            data.push_back(layer.thermal_data);
        }
        return data;
    }

    Glazing_System_Thermal_And_Optical::Glazing_System_Thermal_And_Optical(
      std::vector<Product_Data_Thermal_Optical> const & product_data,
      std::vector<Gap_Data> const & gap_values,
      window_standards::Optical_Standard const & standard,
      double width,
      double height,
      Environments const & u_environment,
      Environments const & shgc_environment) :
        Glazing_System_Optical(get_optical_data(product_data), standard),
        Glazing_System_Thermal(get_thermal_data(product_data),
                               gap_values,
                               width,
                               height,
                               u_environment,
                               shgc_environment)
    {}

    Thermal_Result Glazing_System_Thermal_And_Optical::shgc(double theta, double phi) const
    {
        std::vector<double> calculated_absorptances;
        return Glazing_System_Thermal::shgc(calculated_absorptances, theta, phi);
    }
}   // namespace wincalc
