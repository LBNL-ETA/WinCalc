#ifndef WINCALC_GLAZING_SYSTEM_H_
#define WINCALC_GLAZING_SYSTEM_H_

#include <vector>
#include <OpticsParser.hpp>
#include <windows_standards/windows_standard.h>
#include "gap.h"
#include "thermal_results.h"
#include "optical_results.h"
#include "environmental_conditions.h"

namespace wincalc
{
    struct Glazing_System
    {
        Glazing_System(std::vector<OpticsParser::ProductData> const & products,
                       std::vector<Gap_Data> const & gap_values,
                       window_standards::Optical_Standard const & standard,
                       double width = 1.0,
                       double height = 1.0,
                       Environments const & u_environment = nfrc_u_environments(),
                       Environments const & shgc_environment = nfrc_shgc_environments());

        std::vector<OpticsParser::ProductData> solid_layers;
        std::vector<Gap_Data> gap_layers;
        window_standards::Optical_Standard standard;
        double width;
        double height;

        Environments u_environment;
        Environments shgc_environment;

        Thermal_Result u() const;
        Thermal_Result shgc() const;

        WCE_Simple_Result all_method_values(
          window_standards::Optical_Standard_Method_Type const & method_type) const;

        WCE_Color_Result color() const;

    protected:
        window_standards::Optical_Standard_Method
          get_method(window_standards::Optical_Standard_Method_Type const & method_type) const;
    };
}   // namespace wincalc
#endif
