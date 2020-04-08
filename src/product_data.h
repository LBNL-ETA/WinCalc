#pragma once
#include <optional>
#include <WCESpectralAveraging.hpp>
#include <WCESingleLayerOptics.hpp>
#include <optical_standard.h>


namespace wincalc
{
    struct Flippable_Solid_Layer
    {
        Flippable_Solid_Layer(double thickness_meters, bool flipped = false);
        double thickness_meters;
        bool flipped;
    };

    struct Product_Data_Thermal : Flippable_Solid_Layer
    {
        Product_Data_Thermal(double conductivity,
                             double thickness_meters,
                             std::optional<double> ir_transmittance_front = std::optional<double>(),
                             std::optional<double> ir_transmittance_back = std::optional<double>(),
                             std::optional<double> emissivity_front = std::optional<double>(),
                             std::optional<double> emissivity_back = std::optional<double>(),
                             bool flipped = false);
        double conductivity;
        std::optional<double> ir_transmittance_front;
        std::optional<double> ir_transmittance_back;
        std::optional<double> emissivity_front;
        std::optional<double> emissivity_back;
    };

    struct Product_With_Openings_Data_Thermal : Product_Data_Thermal
    {
        double a_top;
        double a_bottom;
        double a_left;
        double a_right;
        double a_front;
    };


    struct Product_Data_Optical : Flippable_Solid_Layer,
                                  std::enable_shared_from_this<Product_Data_Optical>
    {
        Product_Data_Optical(double thickness_meters, bool flipped = false);
        virtual ~Product_Data_Optical();
        virtual std::shared_ptr<Product_Data_Optical> optical_data();
    };

    struct Product_Data_Dual_Band_Optical : Product_Data_Optical
    {
        double tf_solar;
        double tb_solar;
        double rf_solar;
        double rb_solar;

        double tf_visible;
        double tb_visible;
        double rf_visible;
        double rb_visible;
    };


    struct Wavelength_Data
    {
        double wavelength;
        double transmittance_front;
        double transmittance_back;
        double reflectance_front;
        double reflectance_back;
    };


    struct Product_Data_N_Band_Optical : Product_Data_Optical
    {
        Product_Data_N_Band_Optical(FenestrationCommon::MaterialType material_type,
                                    double thickness_meteres,
                                    std::vector<Wavelength_Data> wavelength_data,
                                    bool flipped = false);
        FenestrationCommon::MaterialType material_type;
        std::vector<Wavelength_Data> wavelength_data;
    };

    struct Product_Data_Optical_Thermal
    {
        std::shared_ptr<Product_Data_Optical> optical_data;
        std::shared_ptr<Product_Data_Thermal> thermal_data;
    };


    struct Product_Data_Optical_With_Material : Product_Data_Optical
    {
        Product_Data_Optical_With_Material(
          std::shared_ptr<Product_Data_Optical> const & material_optical_data);
        virtual ~Product_Data_Optical_With_Material() = default;
        virtual std::shared_ptr<Product_Data_Optical> optical_data() override;
#if 0
        virtual std::shared_ptr<SingleLayerOptics::CBSDFLayer>
          create_layer(window_standards::Optical_Standard_Method const & method,
                       SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
                       Spectal_Data_Wavelength_Range_Method const & type =
                         Spectal_Data_Wavelength_Range_Method::FULL,
                       int number_visible_bands = 5,
                       int number_solar_bands = 10) const;
#endif
        std::shared_ptr<Product_Data_Optical> material_optical_data;
    };

    struct Product_Data_Optical_Venetian : Product_Data_Optical_With_Material
    {
        Product_Data_Optical_Venetian(
          std::shared_ptr<Product_Data_Optical> const & material_optical_data,
          double slat_tilt,
          double slat_width,
          double slat_spacing,
          double slat_curvature,
          size_t number_slats);
#if 0
        virtual std::shared_ptr<SingleLayerOptics::CBSDFLayer>
          create_layer(window_standards::Optical_Standard_Method const & method,
                       SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
                       Spectal_Data_Wavelength_Range_Method const & type =
                         Spectal_Data_Wavelength_Range_Method::FULL,
                       int number_visible_bands = 5,
                       int number_solar_bands = 10) const;
#endif
        double slat_width;
        double slat_spacing;
        double slat_curvature;
        double slat_tilt;
        size_t number_slats;
    };

    struct Product_Data_Optical_Woven_Shade : Product_Data_Optical_With_Material
    {
        Product_Data_Optical_Woven_Shade(
          std::shared_ptr<Product_Data_Optical> const & material_optical_data,
          double thread_diameter,
          double thread_spacing,
          double shade_thickness);

        double thread_diameter;
        double thread_spacing;
        double shade_thickness;
    };

    // etc...

}   // namespace wincalc
