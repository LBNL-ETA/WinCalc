#include "product_data.h"

namespace wincalc
{
    Product_Data_Optical::Product_Data_Optical(bool flipped) : Flipped_Layer(flipped)
    {}
    Product_Data_Optical::~Product_Data_Optical()
    {}


Product_Data_N_Band_Optical::Product_Data_N_Band_Optical(
      FenestrationCommon::MaterialType material_type,
      double thickness_meters,
      std::vector<Wavelength_Data> wavelength_data,
      bool flipped) :
        Product_Data_Optical(flipped),
        material_type(material_type),
        thickness_meters(thickness_meters),
        wavelength_data(wavelength_data)
    {}

Flipped_Layer::Flipped_Layer(bool flipped) : flipped(flipped)
    {}

}   // namespace wincalc