#include "shade_factories.h"
#include "convert_optics_parser.h"

namespace wincalc
{
    Product_Data_Optical_Thermal
      create_venetian_blind(Venetian_Geometry const & geometry,
                            OpticsParser::ProductData const & material)
    {
        auto converted_material = convert_to_solid_layer(material);
        auto optical_data = std::make_shared<Product_Data_Optical_Venetian>(
          converted_material.optical_data, geometry);
        return Product_Data_Optical_Thermal(optical_data, converted_material.thermal_data);
    }

    Product_Data_Optical_Thermal
      create_venetian_blind(Venetian_Geometry const & geometry,
                            std::shared_ptr<Product_Data_Optical> material_optical_data,
                            std::shared_ptr<Product_Data_Thermal> material_thermal_data)
    {
        auto optical_data =
          std::make_shared<Product_Data_Optical_Venetian>(material_optical_data, geometry);
        return Product_Data_Optical_Thermal(optical_data, material_thermal_data);
    }

    Product_Data_Optical_Thermal
      create_woven_shade(Woven_Geometry const & geometry,
                         OpticsParser::ProductData const & material)
    {
        auto converted_material = convert_to_solid_layer(material);
        auto optical_data = std::make_shared<Product_Data_Optical_Woven_Shade>(
          converted_material.optical_data, geometry);
        return Product_Data_Optical_Thermal(optical_data, converted_material.thermal_data);
    }

    Product_Data_Optical_Thermal
      create_woven_shade(Woven_Geometry const & geometry,
                         std::shared_ptr<Product_Data_Optical> material_optical_data,
                         std::shared_ptr<Product_Data_Thermal> material_thermal_data)
    {
        auto optical_data =
          std::make_shared<Product_Data_Optical_Woven_Shade>(material_optical_data, geometry);
        return Product_Data_Optical_Thermal(optical_data, material_thermal_data);
    }

    Product_Data_Optical_Thermal
      create_perforated_screen(Perforated_Geometry const & geometry,
                               OpticsParser::ProductData const & material)
    {
        auto converted_material = convert_to_solid_layer(material);
        auto optical_data = std::make_shared<Product_Data_Optical_Perforated_Screen>(
          converted_material.optical_data, geometry);
        return Product_Data_Optical_Thermal(optical_data, converted_material.thermal_data);
    }

    Product_Data_Optical_Thermal
      create_perforated_screen(Perforated_Geometry const & geometry,
                               std::shared_ptr<Product_Data_Optical> material_optical_data,
                               std::shared_ptr<Product_Data_Thermal> material_thermal_data)
    {
        auto optical_data =
          std::make_shared<Product_Data_Optical_Perforated_Screen>(material_optical_data, geometry);
        return Product_Data_Optical_Thermal(optical_data, material_thermal_data);
    }
}   // namespace wincalc
