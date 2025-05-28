#include "util.h"
#include <algorithm>
#include <iostream>
#include "convert_optics_parser.h"

namespace wincalc
{
    std::vector<std::shared_ptr<wincalc::Product_Data_Optical>>
      get_optical_layers(std::vector<wincalc::Product_Data_Optical_Thermal> const & layers,
                         std::optional<Product_Data_Optical_Thermal> non_coplanar_attachment_exterior,
                         std::optional<Product_Data_Optical_Thermal> non_coplanar_attachment_interior)
    {
        std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> optical_layers;
        if(non_coplanar_attachment_exterior)
        {
            optical_layers.push_back(non_coplanar_attachment_exterior.value().optical_data);
        }
        for(const auto& layer : layers)
        {
            optical_layers.push_back(layer.optical_data);
        }
        if (non_coplanar_attachment_interior)
        {
            optical_layers.push_back(non_coplanar_attachment_interior.value().optical_data);
        }

        return optical_layers;
    }

    std::vector<std::shared_ptr<Product_Data_Thermal>>
      get_thermal_layers(std::vector<Product_Data_Optical_Thermal> const & layers)
    {
        std::vector<std::shared_ptr<Product_Data_Thermal>> thermal_layers;
        for(const auto& layer : layers)
        {
            thermal_layers.push_back(layer.thermal_data);
        }
        return thermal_layers;
    }
#if 0
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
#endif
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

	std::vector<std::vector<double>> get_wavelengths(std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data)
	{
		std::vector<std::vector<double>> wavelengths;
		for(auto & product : product_data)
		{
			wavelengths.push_back(product->wavelengths());
		}
		return wavelengths;
	}

	template<>
	std::vector<double> get_first_val(std::vector<OpticsParser::WLData> const & wavelength_data)
	{
		std::vector<double> res;

		for(auto const & row : wavelength_data)
		{
			res.push_back(row.wavelength);
		}

		return res;
	}

    void logMsg(std::string const & msg)
    {
        std::cerr << msg << std::endl;
    }
}   // namespace wincalc