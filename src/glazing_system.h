#ifndef WINCALC_GLAZING_SYSTEM_H_
#define WINCALC_GLAZING_SYSTEM_H_

#include <vector>
#include <OpticsParser.hpp>
#include <windows_standards/windows_standard.h>
#include "gap.h"
#include "thermal_results.h"
#include "optical_results.h"

struct Glazing_System
{
    Glazing_System(std::vector<OpticsParser::ProductData> const & products,
                   std::vector<Gap_Data> const & gap_values,
                   Standard const & standard,
                   double width = 1.0,
                   double height = 1.0);

    std::vector<OpticsParser::ProductData> solid_layers;
    std::vector<Gap_Data> gap_layers;
    Standard standard;
    double width;
    double height;    

    Thermal_Result u() const;
    Thermal_Result shgc() const;

    WCE_Simple_Result all_method_values(Method_Type const & method_type) const;

    WCE_Color_Result color() const;

protected:
    Method get_method(Method_Type const & method_type) const;
};

#endif
