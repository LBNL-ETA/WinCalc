#pragma once

#include <memory>
#include <OpticsParser.hpp>
#include "product_data.h"

namespace wincalc
{
    wincalc::Product_Data_N_Band_Optical convert(OpticsParser::ProductData const & product);
    std::vector<std::shared_ptr<wincalc::Product_Data_Optical>>
      convert(std::vector < OpticsParser::ProductData> const & product);
}