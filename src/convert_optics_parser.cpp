#include "convert_optics_parser.h"

namespace wincalc
{
    wincalc::Wavelength_Data convert(OpticsParser::WLData const & wl_data)
    {
        return wincalc::Wavelength_Data{
          wl_data.wavelength, wl_data.T, wl_data.frontR, wl_data.backR};
    }

    std::vector<wincalc::Wavelength_Data> convert(std::vector<OpticsParser::WLData> const & wl_data)
    {
        std::vector<wincalc::Wavelength_Data> converted_data;
        for(auto wl_row : wl_data)
        {
            converted_data.push_back(convert(wl_row));
        }
        return converted_data;
    }

    Product_Data_N_Band_Optical convert(OpticsParser::ProductData const & product)
    {
        std::vector<Wavelength_Data> converted_wavelengths = convert(product.measurements);
        // TODO WCE Fix this to use actual type and not always monolithic
        Product_Data_N_Band_Optical converted{FenestrationCommon::MaterialType::Monolithic,
                                              product.thickness / 1000.0,
                                              converted_wavelengths};
        return converted;
    }

    std::vector<std::shared_ptr<wincalc::Product_Data_Optical>>
      convert(std::vector<OpticsParser::ProductData> const & product)
    {
        std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> converted;
        for(auto layer : product)
        {
            converted.push_back(std::make_shared<wincalc::Product_Data_Optical>(convert(layer)));
        }

        return converted;
    }
}   // namespace wincalc
