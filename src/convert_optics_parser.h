#pragma once

#include <memory>
#include <OpticsParser.hpp>
#include "product_data.h"

namespace wincalc
{
    wincalc::Product_Data_Optical_Thermal
      convert_to_solid_layer(std::shared_ptr<OpticsParser::ProductData> const & product);
    std::vector<wincalc::Product_Data_Optical_Thermal> convert_to_solid_layers(
      std::vector<std::shared_ptr<OpticsParser::ProductData>> const & products);
}   // namespace wincalc
