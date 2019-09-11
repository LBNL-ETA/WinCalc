#pragma once
#include <optional>
#include <WCESpectralAveraging.hpp>

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

    struct Product_Data_Optical : Flippable_Solid_Layer
    {
        Product_Data_Optical(double thickness_meters, bool flipped = false);
        virtual ~Product_Data_Optical();
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

}   // namespace wincalc
