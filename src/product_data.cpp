#include "product_data.h"
#include "create_wce_objects.h"

namespace wincalc
{
    Product_Data_Optical::Product_Data_Optical(double thickness_meters, bool flipped) :
        Flippable_Solid_Layer(thickness_meters, flipped)
    {}
    Product_Data_Optical::~Product_Data_Optical()
    {}

    std::shared_ptr<Product_Data_Optical> Product_Data_Optical::optical_data()
    {
        return shared_from_this();
    }


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

    Product_Data_Optical_With_Material::Product_Data_Optical_With_Material(
      std::shared_ptr<Product_Data_Optical> const & material_optical_data) :
        Product_Data_Optical(material_optical_data->thickness_meters),
        material_optical_data(material_optical_data)
    {}

    std::shared_ptr<Product_Data_Optical> Product_Data_Optical_With_Material::optical_data()
    {
        return material_optical_data;
    }
#if 0
    std::shared_ptr<SingleLayerOptics::CBSDFLayer> Product_Data_Optical_With_Material::create_layer(
      window_standards::Optical_Standard_Method const & method,
      SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands) const
    {
        auto material =
          create_material(optical_data, method, type, number_visible_bands, number_solar_bands);
        auto layer =
          SingleLayerOptics::CBSDFLayerMaker::getSpecularLayer(material, bsdf_hemisphere);
        return layer;
    }
#endif

    Product_Data_Optical_Venetian::Product_Data_Optical_Venetian(
      std::shared_ptr<Product_Data_Optical> const & optical_data,
      double slat_tilt,
      double slat_width,
      double slat_spacing,
      double slat_curvature,
      size_t number_slats) :
        Product_Data_Optical_With_Material(optical_data),
        slat_tilt(slat_tilt),
        slat_width(slat_width),
        slat_spacing(slat_spacing),
        slat_curvature(slat_curvature),
        number_slats(number_slats)
    {}

#if 0
    std::shared_ptr<SingleLayerOptics::CBSDFLayer> Product_Data_Optical_Venetian::create_layer(
      window_standards::Optical_Standard_Method const & method,
      SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands) const
    {

        auto material =
          create_material(optical_data, method, type, number_visible_bands, number_solar_bands);
        auto layer = SingleLayerOptics::CBSDFLayerMaker::getVenetianLayer(material,
                                                                          bsdf_hemisphere,
                                                                          slat_width,
                                                                          slat_spacing,
                                                                          slat_tilt,
                                                                          slat_curvature,
                                                                          number_slats);
        return layer;
    }
#endif

    Product_Data_Optical_Woven_Shade::Product_Data_Optical_Woven_Shade(
      std::shared_ptr<Product_Data_Optical> const & material_optical_data,
      double thread_diameter,
      double thread_spacing,
      double shade_thickness) :
        Product_Data_Optical_With_Material(material_optical_data),
        thread_diameter(thread_diameter),
        thread_spacing(thread_spacing),
        shade_thickness(shade_thickness)
    {}

}   // namespace wincalc
