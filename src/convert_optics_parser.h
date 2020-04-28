#pragma once

#include <memory>
#include <OpticsParser.hpp>
#include "product_data.h"

namespace wincalc
{
    std::shared_ptr<Product_Data_Optical>
      convert_optical(std::shared_ptr<OpticsParser::ProductData> const & product);
    wincalc::Product_Data_Thermal
      convert_thermal(std::shared_ptr<OpticsParser::ProductData> const & product);
    std::vector<wincalc::Product_Data_Optical_Thermal>
      convert(std::vector < std::shared_ptr<OpticsParser::ProductData> > const & product);
}   // namespace wincalc
