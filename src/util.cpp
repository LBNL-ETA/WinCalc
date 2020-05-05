#include "util.h"
#include <algorithm>
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
            optical_layers.push_back(convert_optical(layer));
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
              std::make_shared<Product_Data_Thermal>(convert_thermal(layer)));
        }
        return thermal_layers;
    }

    bool all_optical_layers_the_same(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & layers)
    {
        size_t ct_dual_band = 0;

        for(size_t i = 0; i < layers.size() - 1; ++i)
        {
            if(std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical>(layers[i]))
            {
                ct_dual_band++;
            }
        }

        return ct_dual_band == 0 || ct_dual_band == layers.size();
    }

    bool
      all_optical_layers_the_same(std::vector<wincalc::Product_Data_Optical_Thermal> const & layers)
    {
        return all_optical_layers_the_same(get_optical_layers(layers));
    }

    std::vector<std::shared_ptr<wincalc::Product_Data_Optical>>
      get_optical_layers(std::vector<std::shared_ptr<Product_Data_Optical_Thermal>> const & layers)
    {
        std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> optical_layers;
        for(auto layer : layers)
        {
            optical_layers.push_back(layer->optical_data);
        }
        return optical_layers;
    }

    std::string to_lower(std::string s)
    {
        /*
                This has issues with unicode but there will be many of those to deal
                with if this ever needs to really be internationalized.
                Until then this should be good enough
        */
#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4244)
#endif
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
#ifdef _MSC_VER
#    pragma warning(pop)
#endif
        return s;
    }

}   // namespace wincalc
