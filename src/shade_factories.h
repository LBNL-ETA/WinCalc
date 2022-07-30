#pragma once

#include <OpticsParser.hpp>
#include "product_data.h"

namespace wincalc
{
    Product_Data_Optical_Thermal
      create_venetian_blind(Venetian_Geometry const & geometry,
                            std::shared_ptr<OpticsParser::ProductData> const & material);
    Product_Data_Optical_Thermal
      create_venetian_blind(Venetian_Geometry const & geometry,
                            std::shared_ptr<Product_Data_Optical> material_optical_data,
                            std::shared_ptr<Product_Data_Thermal> material_thermal_data);

	Product_Data_Optical_Thermal
		create_woven_shade(Woven_Geometry const & geometry,
			std::shared_ptr<OpticsParser::ProductData> const & material);
	Product_Data_Optical_Thermal
		create_woven_shade(Woven_Geometry const & geometry,
			std::shared_ptr<Product_Data_Optical> material_optical_data,
			std::shared_ptr<Product_Data_Thermal> material_thermal_data);

	Product_Data_Optical_Thermal
		create_perforated_screen(Perforated_Geometry const & geometry,
			std::shared_ptr<OpticsParser::ProductData> const & material);
	Product_Data_Optical_Thermal
		create_perforated_screen(Perforated_Geometry const & geometry,
			std::shared_ptr<Product_Data_Optical> material_optical_data,
			std::shared_ptr<Product_Data_Thermal> material_thermal_data);

}   // namespace wincalc
