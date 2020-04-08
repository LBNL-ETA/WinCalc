#pragma once

#include <vector>
#include <OpticsParser.hpp>
#include "product_data.h"

namespace wincalc
{
    std::vector<std::shared_ptr<wincalc::Product_Data_Optical>>
      get_optical_layers(std::vector<wincalc::Product_Data_Optical_Thermal> const & layers);

    std::vector<std::shared_ptr<Product_Data_Thermal>>
      get_thermal_layers(std::vector<Product_Data_Optical_Thermal> const & layers);

    std::vector<std::shared_ptr<wincalc::Product_Data_Optical>>
      get_optical_layers(std::vector<std::shared_ptr<OpticsParser::ProductData>> const & layers);

    std::vector<std::shared_ptr<Product_Data_Thermal>>
      get_thermal_layers(std::vector<std::shared_ptr<OpticsParser::ProductData>> const & layers);

    bool all_optical_layers_the_same(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & layers);

    bool all_optical_layers_the_same(
      std::vector<wincalc::Product_Data_Optical_Thermal> const & layers);

}   // namespace wincalc
