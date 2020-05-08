#include "convert_optics_parser.h"
#include <sstream>
#include "util.h"

namespace wincalc
{
#if 0
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
#endif

    std::shared_ptr<Product_Data_Optical>
      convert_optical(std::shared_ptr<OpticsParser::ProductData> const & product)
    {
        std::shared_ptr<OpticsParser::ComposedProductData> composed_product =
          std::dynamic_pointer_cast<OpticsParser::ComposedProductData>(product);

        if(composed_product)
        {
            auto material = convert_optical(composed_product->compositionInformation->material);

            std::shared_ptr<OpticsParser::VenetianGeometry> venetian_geometry =
              std::dynamic_pointer_cast<OpticsParser::VenetianGeometry>(
                composed_product->compositionInformation->geometry);
            if(venetian_geometry)
            {
                std::shared_ptr<Product_Data_Optical> converted(
                  new Product_Data_Optical_Venetian{material,
                                                    venetian_geometry->slatTilt,
                                                    venetian_geometry->slatWidth,
                                                    venetian_geometry->slatSpacing,
                                                    venetian_geometry->slatCurvature,
                                                    venetian_geometry->numberSegments});
                return converted;
            }
            std::shared_ptr<OpticsParser::WovenGeometry> woven_geometry =
              std::dynamic_pointer_cast<OpticsParser::WovenGeometry>(
                composed_product->compositionInformation->geometry);
            if(woven_geometry)
            {
                std::shared_ptr<Product_Data_Optical> converted(
                  new Product_Data_Optical_Woven_Shade{material,
                                                       woven_geometry->threadDiameter,
                                                       woven_geometry->threadSpacing,
                                                       woven_geometry->shadeThickness});
                return converted;
            }
            std::shared_ptr<OpticsParser::PerforatedGeometry> perforated_geometry =
              std::dynamic_pointer_cast<OpticsParser::PerforatedGeometry>(
                composed_product->compositionInformation->geometry);
            if(perforated_geometry)
            {
                Product_Data_Optical_Perforated_Screen::Type perforation_type;
                auto perforation_type_str = to_lower(perforated_geometry->perforationType);
                if(perforation_type_str == "circular")
                {
                    perforation_type = Product_Data_Optical_Perforated_Screen::Type::CIRCULAR;
                }
                else if(perforation_type_str == "square")
                {
                    perforation_type = Product_Data_Optical_Perforated_Screen::Type::SQUARE;
                }
                else if(perforation_type_str == "rectangular")
                {
                    perforation_type = Product_Data_Optical_Perforated_Screen::Type::RECTANGULAR;
                }
                else
                {
                    std::stringstream msg;
                    msg << "Unsupported perforation type: " << perforated_geometry->perforationType;
                    throw std::runtime_error(msg.str());
                }
                std::shared_ptr<Product_Data_Optical> converted(
                  new Product_Data_Optical_Perforated_Screen{material,
                                                             perforated_geometry->spacingX,
                                                             perforated_geometry->spacingY,
                                                             perforated_geometry->dimensionX,
                                                             perforated_geometry->dimensionY,
                                                             perforation_type});
                return converted;
            }
            // If this point is reached then the product is either missing a geometry or
            // has a geometry that is not yet supported in WinCalc
            throw std::runtime_error("Composed product with missing or unsupported geometry");
        }
        else
        {
            auto wavelength_measured_values = product->measurements.value();
            // TODO WCE Fix this to use actual type and not always monolithic
            std::shared_ptr<Product_Data_Optical> converted(
              new Product_Data_N_Band_Optical{FenestrationCommon::MaterialType::Monolithic,
                                              product->thickness.value() / 1000.0,
                                              wavelength_measured_values,
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

    wincalc::Product_Data_Optical_Thermal
      convert_to_solid_layer(std::shared_ptr<OpticsParser::ProductData> const & product)
    {
        auto optical = convert_optical(product);
        auto thermal = std::make_shared<Product_Data_Thermal>(convert_thermal(product));
        return wincalc::Product_Data_Optical_Thermal{optical, thermal};
    }

    std::vector<wincalc::Product_Data_Optical_Thermal> convert_to_solid_layers(
      std::vector<std::shared_ptr<OpticsParser::ProductData>> const & products)
    {
        std::vector<wincalc::Product_Data_Optical_Thermal> converted;
        for(auto product : products)
        {
            converted.push_back(convert_to_solid_layer(product));
        }

        return converted;
    }
}   // namespace wincalc
