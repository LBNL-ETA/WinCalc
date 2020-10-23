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
      get_optical_layers(std::vector<std::shared_ptr<Product_Data_Optical_Thermal>> const & layers);

    bool all_optical_layers_the_same(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & layers);

    bool all_optical_layers_the_same(
      std::vector<wincalc::Product_Data_Optical_Thermal> const & layers);

    std::string to_lower(std::string s);

    template<typename T>
    std::vector<double> get_first_val(std::vector<T> const & vec)
    {
        // returns a vector of the first of each tuple in the input vector
        std::vector<double> res;
        for(T const & val : vec)
        {
            res.push_back(std::get<0>(val));
        }
        return res;
    }

    template<>
	std::vector<double> get_first_val(std::vector<OpticsParser::WLData> const & wavelength_data);

    std::vector<std::vector<double>> get_wavelengths(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data);
}   // namespace wincalc
