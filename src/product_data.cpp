#include "product_data.h"

namespace wincalc
{
    Product_Data_Optical::Product_Data_Optical(double thickness_meters, bool flipped) :
        Flippable_Solid_Layer(thickness_meters, flipped)
    {}
    Product_Data_Optical::~Product_Data_Optical()
    {}


    Product_Data_N_Band_Optical::Product_Data_N_Band_Optical(
      FenestrationCommon::MaterialType material_type,
      double thickness_meters,
      std::vector<Wavelength_Data> wavelength_data,
      bool flipped) :
        Product_Data_Optical(thickness_meters, flipped),
        material_type(material_type),
        wavelength_data(wavelength_data)
    {}

    Flippable_Solid_Layer::Flippable_Solid_Layer(double thickness_meters, bool flipped) :
        thickness_meters(thickness_meters), flipped(flipped)
    {}

    Product_Data_Thermal::Product_Data_Thermal(double conductivity,
                                               double thickness_meters,
                                               std::optional<double> ir_transmittance_front,
                                               std::optional<double> ir_transmittance_back,
                                               std::optional<double> emissivity_front,
                                               std::optional<double> emissivity_back,
                                               bool flipped) :
        Flippable_Solid_Layer(thickness_meters, flipped),
        conductivity(conductivity),
        ir_transmittance_front(ir_transmittance_front),
        ir_transmittance_back(ir_transmittance_back),
        emissivity_front(emissivity_front),
        emissivity_back(emissivity_back)
    {}

}   // namespace wincalc
