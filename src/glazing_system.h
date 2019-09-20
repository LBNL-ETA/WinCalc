#ifndef WINCALC_GLAZING_SYSTEM_H_
#define WINCALC_GLAZING_SYSTEM_H_

#include <vector>
#include <OpticsParser.hpp>
#include <windows_standards/windows_standard.h>
#include <WCEGases.hpp>
#include "gap.h"
#include "thermal_results.h"
#include "optical_results.h"
#include "environmental_conditions.h"
#include "product_data.h"

namespace wincalc
{
    struct Glazing_System_Optical_Interface
    {
        Glazing_System_Optical_Interface(window_standards::Optical_Standard const & standard);

        virtual WCE_Optical_Results
          all_method_values(window_standards::Optical_Standard_Method_Type const & method_type,
                            double theta = 0,
                            double phi = 0) const = 0;

        virtual WCE_Color_Results color(double theta = 0, double phi = 0) const = 0;

        void optical_standard(window_standards::Optical_Standard const & s);
        window_standards::Optical_Standard optical_standard();

    protected:
        window_standards::Optical_Standard standard;

        window_standards::Optical_Standard_Method
          get_method(window_standards::Optical_Standard_Method_Type const & method_type) const;
    };

    struct Glazing_System_Thermal_Interface
    {
        virtual Thermal_Result u(double theta = 0, double phi = 0) const = 0;
        virtual Thermal_Result shgc(std::vector<double> const & absorptances,
                                    double theta = 0,
                                    double phi = 0) const = 0;
        virtual Thermal_Result shgc(double theta = 0, double phi = 0) const = 0;
    };

    struct Glazing_System_Optical_BSDF_Interface : Glazing_System_Optical_Interface
    {
#if 0
        /* override the calls in Glazing_System_Interface*/
        virtual square_matrix matrix_method_results(
          window_standards::Optical_Standard_Method_Type const & method_type) const = 0;
#endif
        /* may have the same square matrix results for color in the future*/
    };

    struct Glazing_System_Optical : Glazing_System_Optical_Interface
    {
        Glazing_System_Optical(
          std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & solid_layers,
          window_standards::Optical_Standard const & standard);

        WCE_Optical_Results
          all_method_values(window_standards::Optical_Standard_Method_Type const & method_type,
                            double theta = 0,
                            double phi = 0) const override;

        WCE_Color_Results color(double theta = 0, double phi = 0) const override;

    protected:
        std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> solid_layers_optical;
    };

    struct Glazing_System_Thermal : Glazing_System_Thermal_Interface
    {
        Glazing_System_Thermal(
          std::vector<std::shared_ptr<wincalc::Product_Data_Thermal>> const & products,
          std::vector<Gap_Data> const & gap_layers,
          double width = 1.0,
          double height = 1.0,
          Environments const & u_environment = nfrc_u_environments(),
          Environments const & shgc_environment = nfrc_shgc_environments());

		Glazing_System_Thermal(
          std::vector<std::shared_ptr<wincalc::Product_Data_Thermal>> const & products,
          std::vector<Engine_Gap_Info> const & gap_layers,
          double width = 1.0,
          double height = 1.0,
          Environments const & u_environment = nfrc_u_environments(),
          Environments const & shgc_environment = nfrc_shgc_environments());

        std::vector<Engine_Gap_Info> gap_layers;
        std::vector<std::shared_ptr<wincalc::Product_Data_Thermal>> solid_layers_thermal;
        double width;
        double height;

        Environments u_environment;
        Environments shgc_environment;

        Thermal_Result u(double theta, double phi) const;
        Thermal_Result shgc(std::vector<double> const & absorptances_front,
                            double theta = 0,
                            double phi = 0) const override;
        Thermal_Result shgc(double theta, double phi) const override;
    };

    struct Glazing_System_Thermal_And_Optical : Glazing_System_Thermal, Glazing_System_Optical
    {
        Glazing_System_Thermal_And_Optical(
          std::vector<Product_Data_Optical_Thermal> const & product_data,
          std::vector<Gap_Data> const & gap_values,
          window_standards::Optical_Standard const & standard,
          double width = 1.0,
          double height = 1.0,
          Environments const & u_environment = nfrc_u_environments(),
          Environments const & shgc_environment = nfrc_shgc_environments());

        Glazing_System_Thermal_And_Optical(
          std::vector<OpticsParser::ProductData> const & product_data,
          std::vector<Gap_Data> const & gap_values,
          window_standards::Optical_Standard const & standard,
          double width = 1.0,
          double height = 1.0,
          Environments const & u_environment = nfrc_u_environments(),
          Environments const & shgc_environment = nfrc_shgc_environments());

        Thermal_Result shgc(double theta = 0, double phi = 0) const;
        Thermal_Result u(double theta = 0, double phi = 0) const;

    protected:
        std::vector<Product_Data_Optical_Thermal> optical_and_thermal_data() const;
    };
}   // namespace wincalc
#endif
