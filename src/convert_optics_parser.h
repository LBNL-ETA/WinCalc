#pragma once

#include <memory>
#include <OpticsParser.hpp>
#include "product_data.h"

namespace wincalc
{
    wincalc::Product_Data_N_Band_Optical convert_optical(OpticsParser::ProductData const & product);
    wincalc::Product_Data_Thermal convert_thermal(OpticsParser::ProductData const & product);
    std::vector<wincalc::Product_Data_Optical_Thermal>
      convert(std::vector < OpticsParser::ProductData> const & product);
}