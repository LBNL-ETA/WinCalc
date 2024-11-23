#pragma once

#include <OpticsParser.hpp>
#include "product_data.h"

namespace wincalc
{
    Product_Data_Optical_Thermal create_venetian_blind(Venetian_Geometry const & geometry,
                                                       OpticsParser::ProductData const & material);
    Product_Data_Optical_Thermal
      create_venetian_blind(Venetian_Geometry const & geometry,
                            std::shared_ptr<Product_Data_Optical> material_optical_data,
                            std::shared_ptr<Product_Data_Thermal> material_thermal_data);

    Product_Data_Optical_Thermal create_woven_shade(Woven_Geometry const & geometry,
                                                    OpticsParser::ProductData const & material);
    Product_Data_Optical_Thermal
      create_woven_shade(Woven_Geometry const & geometry,
                         std::shared_ptr<Product_Data_Optical> material_optical_data,
                         std::shared_ptr<Product_Data_Thermal> material_thermal_data);

    Product_Data_Optical_Thermal
      create_perforated_screen(Perforated_Geometry const & geometry,
                               OpticsParser::ProductData const & material);
    Product_Data_Optical_Thermal
      create_perforated_screen(Perforated_Geometry const & geometry,
                               std::shared_ptr<Product_Data_Optical> material_optical_data,
                               std::shared_ptr<Product_Data_Thermal> material_thermal_data);

    Product_Data_Optical_Thermal
      create_homogeneous_diffusing_shade(std::shared_ptr<Product_Data_Optical> material_optical_data,
      std::shared_ptr<Product_Data_Thermal> material_thermal_data,
      double permeability_factor = 0);

    Product_Data_Optical_Thermal
      create_homogeneous_diffusing_shade(OpticsParser::ProductData const & material,
                                         double permeability_factor = 0);

    Product_Data_Optical_Thermal create_perfectly_diffusing_shade(
      std::shared_ptr<Product_Data_Optical> material_optical_data,
      std::shared_ptr<Product_Data_Thermal> material_thermal_data,
      double permeability_factor = 0);

    Product_Data_Optical_Thermal
      create_perfectly_diffusing_shade(OpticsParser::ProductData const & material,
                                         double permeability_factor = 0);

}   // namespace wincalc
