#include "util.h"
#include "convert_optics_parser.h"

namespace wincalc
{
    std::vector<std::shared_ptr<wincalc::Product_Data_Optical>>
      get_optical_layers(std::vector<wincalc::Product_Data_Optical_Thermal> const & layers)
    {
        std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> optical_layers;

        for(auto layer : layers)
        {
            optical_layers.push_back(layer.optical_data);
        }

        return optical_layers;
    }

    std::vector<std::shared_ptr<Product_Data_Thermal>>
      get_thermal_layers(std::vector<Product_Data_Optical_Thermal> const & layers)
    {
        std::vector<std::shared_ptr<Product_Data_Thermal>> thermal_layers;
        for(auto layer : layers)
        {
            thermal_layers.push_back(layer.thermal_data);
        }
        return thermal_layers;
    }

    std::vector<std::shared_ptr<wincalc::Product_Data_Optical>>
      get_optical_layers(std::vector<std::shared_ptr<OpticsParser::ProductData>> const & layers)
    {
        std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> optical_layers;

        for(auto layer : layers)
        {
            optical_layers.push_back(
              std::make_shared<Product_Data_N_Band_Optical>(convert_optical(*layer)));
        }

        return optical_layers;
    }

    std::vector<std::shared_ptr<Product_Data_Thermal>>
      get_thermal_layers(std::vector<std::shared_ptr<OpticsParser::ProductData>> const & layers)
    {
        std::vector<std::shared_ptr<Product_Data_Thermal>> thermal_layers;
        for(auto layer : layers)
        {
            thermal_layers.push_back(
              std::make_shared<Product_Data_Thermal>(convert_thermal(*layer)));
        }
        return thermal_layers;
    }

    bool all_optical_layers_the_same(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & layers)
    {
        if(layers.size() > 1)
        {
            for(size_t i = 0; i < layers.size() - 1; ++i)
            {
                if(static_cast<bool>(
                     std::dynamic_pointer_cast<wincalc::Product_Data_N_Band_Optical>(layers[i]))
                   != static_cast<bool>(
                     std::dynamic_pointer_cast<wincalc::Product_Data_N_Band_Optical>(
                       layers[i + 1])))
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool
      all_optical_layers_the_same(std::vector<wincalc::Product_Data_Optical_Thermal> const & layers)
    {
        return all_optical_layers_the_same(get_optical_layers(layers));
    }
    
}   // namespace wincalc
