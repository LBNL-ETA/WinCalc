#pragma once
#include <optional>
#include <WCESpectralAveraging.hpp>

namespace wincalc
{
    struct Flipped_Layer
    {
        Flipped_Layer(bool flipped = false);

        bool flipped;
    };

    struct Product_Data_Thermal : Flipped_Layer
    {
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

    struct Product_Data_Optical : Flipped_Layer
    {
        Product_Data_Optical(bool flipped = false);
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
        double thickness_meters;
        std::vector<Wavelength_Data> wavelength_data;
    };

    struct Product_Data_Thermal_Optical
    {
        std::shared_ptr<Product_Data_Optical> optical_data;
        std::shared_ptr<Product_Data_Thermal> thermal_data;
    };

}   // namespace wincalc
