#include "convert_optics_parser.h"

namespace wincalc
{
    wincalc::Wavelength_Data convert(OpticsParser::WLData const & wl_data)
    {
        return wincalc::Wavelength_Data{
          wl_data.wavelength, wl_data.T, wl_data.T, wl_data.frontR, wl_data.backR};
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

    Product_Data_N_Band_Optical convert_optical(OpticsParser::ProductData const & product)
    {
        std::vector<Wavelength_Data> converted_wavelengths = convert(product.measurements);
        // TODO WCE Fix this to use actual type and not always monolithic
        Product_Data_N_Band_Optical converted{FenestrationCommon::MaterialType::Monolithic,
                                              product.thickness / 1000.0,
                                              converted_wavelengths};
        return converted;
    }

    wincalc::Product_Data_Thermal convert_thermal(OpticsParser::ProductData const & product)
    {
        return wincalc::Product_Data_Thermal(*product.conductivity,
                                             product.thickness / 1000.0,
                                             product.IRTransmittance,
                                             product.IRTransmittance,
                                             product.frontEmissivity,
                                             product.backEmissivity,
                                             false);
    }

    std::vector<wincalc::Product_Data_Optical_Thermal>
      convert(std::vector<OpticsParser::ProductData> const & product)
    {
        std::vector<wincalc::Product_Data_Optical_Thermal> converted;
        for(auto layer : product)
        {
            auto optical = std::make_shared<Product_Data_N_Band_Optical>(convert_optical(layer));
            auto thermal = std::make_shared<Product_Data_Thermal>(convert_thermal(layer));
            converted.push_back(wincalc::Product_Data_Optical_Thermal{optical, thermal});
        }

        return converted;
    }
}   // namespace wincalc
