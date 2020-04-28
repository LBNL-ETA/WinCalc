#include "convert_optics_parser.h"
#include <sstream>

namespace wincalc
{
    wincalc::Wavelength_Data convert(OpticsParser::WLData const & wl_data)
    {
        return wincalc::Wavelength_Data{wl_data.wavelength,
                                        wl_data.directComponent.tf,
                                        wl_data.directComponent.tf,
                                        wl_data.directComponent.rf,
                                        wl_data.directComponent.rb};
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

    std::shared_ptr<Product_Data_Optical>
      convert_optical(std::shared_ptr<OpticsParser::ProductData> const & product)
    {
        std::shared_ptr<OpticsParser::ComposedProductData> composed_product =
          std::dynamic_pointer_cast<OpticsParser::ComposedProductData>(product);

        if(composed_product)
        {
            std::vector<Wavelength_Data> converted_wavelengths =
              convert(composed_product->compositionInformation->material->measurements.value());
            auto material = convert_optical(composed_product->compositionInformation->material);

            if(_stricmp(composed_product->subtype.value().c_str(), "venetian") == 0)
            {
                std::shared_ptr<OpticsParser::VenetianGeometry> geometry =
                  std::dynamic_pointer_cast<OpticsParser::VenetianGeometry>(
                    composed_product->compositionInformation->geometry);
                std::shared_ptr<Product_Data_Optical> converted(
                  new Product_Data_Optical_Venetian{material,
                                                    geometry->slatTilt,   
                                                    geometry->slatWidth,
                                                    geometry->slatSpacing,
                                                    geometry->slatCurvature,
                                                    geometry->numberSegments});
                return converted;
            }
            else if(_stricmp(composed_product->subtype.value().c_str(), "woven") == 0)
            {
                std::shared_ptr<OpticsParser::WovenGeometry> geometry =
                  std::dynamic_pointer_cast<OpticsParser::WovenGeometry>(
                    composed_product->compositionInformation->geometry);
                std::shared_ptr<Product_Data_Optical> converted(
                  new Product_Data_Optical_Woven_Shade{material,
                                                       geometry->threadDiameter,
                                                       geometry->threadSpacing,
                                                       geometry->shadeThickness});
                return converted;
            }
            else if(_stricmp(composed_product->subtype.value().c_str(), "perforated-screen") == 0)
            {
                std::shared_ptr<OpticsParser::PerforatedGeometry> geometry =
                  std::dynamic_pointer_cast<OpticsParser::PerforatedGeometry>(
                    composed_product->compositionInformation->geometry);
                Product_Data_Optical_Perforated_Screen::Type perforation_type;
                if(_stricmp(geometry->perforationType.c_str(), "Circular") == 0)
                {
                    perforation_type = Product_Data_Optical_Perforated_Screen::Type::CIRCULAR;
                }
                else if(_stricmp(geometry->perforationType.c_str(), "Square") == 0)
                {
                    perforation_type = Product_Data_Optical_Perforated_Screen::Type::SQUARE;
                }
                else if(_stricmp(geometry->perforationType.c_str(), "Rectangular") == 0)
                {
                    perforation_type = Product_Data_Optical_Perforated_Screen::Type::RECTANGULAR;
                }
                else
                {
                    std::stringstream msg;
                    msg << "Unsupported perforation type: " << geometry->perforationType;
                    throw std::runtime_error(msg.str());
                }
                std::shared_ptr<Product_Data_Optical> converted(
                  new Product_Data_Optical_Perforated_Screen{material,
                                                             geometry->spacingX,
                                                             geometry->spacingY,
                                                             geometry->dimensionX,
                                                             geometry->dimensionY,
                                                             perforation_type});
                return converted;
            }
            else
            {
                throw std::runtime_error("Unsupported subtype");
            }
        }
        else
        {
            std::vector<Wavelength_Data> converted_wavelengths =
              convert(product->measurements.value());
            // TODO WCE Fix this to use actual type and not always monolithic
            std::shared_ptr<Product_Data_Optical> converted(
              new Product_Data_N_Band_Optical{FenestrationCommon::MaterialType::Monolithic,
                                              product->thickness.value() / 1000.0,
                                              converted_wavelengths,
                                              product->IRTransmittance.value(),
                                              product->IRTransmittance.value(),
                                              product->frontEmissivity.value(),
                                              product->backEmissivity.value()});
            return converted;
        }
    }


    wincalc::Product_Data_Thermal
      convert_thermal(std::shared_ptr<OpticsParser::ProductData> const & product)
    {
        std::shared_ptr<OpticsParser::ComposedProductData> composed_product =
          std::dynamic_pointer_cast<OpticsParser::ComposedProductData>(product);

        std::shared_ptr<OpticsParser::ProductData> data = product;
        if(composed_product)
        {
            data = composed_product->compositionInformation->material;
        }

        return wincalc::Product_Data_Thermal(
          data->conductivity.value(), data->thickness.value() / 1000.0, false);
    }

    std::vector<wincalc::Product_Data_Optical_Thermal>
      convert(std::vector<std::shared_ptr<OpticsParser::ProductData>> const & product)
    {
        std::vector<wincalc::Product_Data_Optical_Thermal> converted;
        for(auto layer : product)
        {
            auto optical = convert_optical(layer);
            auto thermal = std::make_shared<Product_Data_Thermal>(convert_thermal(layer));
            converted.push_back(wincalc::Product_Data_Optical_Thermal{optical, thermal});
        }

        return converted;
    }
}   // namespace wincalc
