#pragma once
#include <optional>
#include <WCESpectralAveraging.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCETarcog.hpp>
#include <optical_standard.h>
#include <OpticsParser.hpp>


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
        Product_Data_Thermal(std::optional<double> conductivity,
                             double thickness_meters,
                             bool flipped = false,
                             double opening_top = 0,
                             double opening_bottom = 0,
                             double opening_left = 0,
                             double opening_right = 0,
                             std::optional<double> effective_openness = std::nullopt,
                             std::optional<double> effective_thickness = std::nullopt);

        std::optional<double> conductivity;
        double opening_top;
        double opening_bottom;
        double opening_left;
        double opening_right;
        //! effective openness and effective thickness are calculated by default and it is not
        //! necessary to assign them manually. However, if doing research work with custom shading
        //! device, these two can be calculated outside and the effect of the effective values can
        //! be monitored by the user.
        std::optional<double> effective_openness{std::nullopt};
        std::optional<double> effective_thickness{std::nullopt};
        std::optional<double> youngs_modulus;
        std::optional<double> density;
    };

    struct Product_Data_Optical : Flippable_Solid_Layer,
                                  std::enable_shared_from_this<Product_Data_Optical>
    {
        Product_Data_Optical(double thickness_meters,
                             std::optional<double> ir_transmittance_front = std::optional<double>(),
                             std::optional<double> ir_transmittance_back = std::optional<double>(),
                             std::optional<double> emissivity_front = std::optional<double>(),
                             std::optional<double> emissivity_back = std::optional<double>(),
                             double permeability_factor = 0,
                             bool flipped = false);
        virtual ~Product_Data_Optical();
        virtual std::shared_ptr<Product_Data_Optical> optical_data();
        // The optical section seems a strange place to put this since it has "thermal" in
        // the name but when thermal openness is calculated it is done using the geometry
        // of the product, e.g. for venetians it uses slat tilt angle, slat spacing, etc...
        // And those properties are also used for the optical calculations.  So this seems
        // to be analagous to other properties used in thermal calculations that are actually
        // derived from optical data, e.g. solar absorptance in SHGC calculations.
        virtual std::unique_ptr<EffectiveLayers::EffectiveLayer>
          effective_thermal_values(double width,
                                   double height,
                                   double gap_width_top,
                                   double gap_width_bottom,
                                   double gap_width_left,
                                   double gap_width_right) const;
        std::optional<double> ir_transmittance_front;
        std::optional<double> ir_transmittance_back;
        std::optional<double> emissivity_front;
        std::optional<double> emissivity_back;
        double permeability_factor;
        virtual std::vector<double> wavelengths() const = 0;
        std::optional<OpticsParser::PVPowerProperties> pv_power_properties;
    };


    struct Product_Data_Dual_Band_Optical : Product_Data_Optical
    {
        Product_Data_Dual_Band_Optical(
          double thickness_meters,
          std::optional<double> ir_transmittance_front = std::optional<double>(),
          std::optional<double> ir_transmittance_back = std::optional<double>(),
          std::optional<double> emissivity_front = std::optional<double>(),
          std::optional<double> emissivity_back = std::optional<double>(),
          double permeability_factor = 0,
          bool flipped = false) :
            Product_Data_Optical(thickness_meters,
                                 ir_transmittance_front,
                                 ir_transmittance_back,
                                 emissivity_front,
                                 emissivity_back,
                                 permeability_factor,
                                 flipped)
        {}


        virtual std::vector<double> wavelengths() const override
        {
            return {0.3, 0.32, 0.38, 0.78, 2.5};
        }
    };

    struct Product_Data_Dual_Band_Optical_Hemispheric : Product_Data_Dual_Band_Optical
    {
        Product_Data_Dual_Band_Optical_Hemispheric(
          double tf_solar,
          double tb_solar,
          double rf_solar,
          double rb_solar,
          double tf_visible,
          double tb_visible,
          double rf_visible,
          double rb_visible,
          double thickness_meteres,
          std::optional<double> ir_transmittance_front = std::optional<double>(),
          std::optional<double> ir_transmittance_back = std::optional<double>(),
          std::optional<double> emissivity_front = std::optional<double>(),
          std::optional<double> emissivity_back = std::optional<double>(),
          double permeability_factor = 0,
          bool flipped = false);

        double tf_solar;
        double tb_solar;
        double rf_solar;
        double rb_solar;
        double tf_visible;
        double tb_visible;
        double rf_visible;
        double rb_visible;
    };

    struct Product_Data_Dual_Band_Optical_BSDF : Product_Data_Dual_Band_Optical
    {
        Product_Data_Dual_Band_Optical_BSDF(
          std::vector<std::vector<double>> const & tf_solar,
          std::vector<std::vector<double>> const & tb_solar,
          std::vector<std::vector<double>> const & rf_solar,
          std::vector<std::vector<double>> const & rb_solar,
          std::vector<std::vector<double>> const & tf_visible,
          std::vector<std::vector<double>> const & tb_visible,
          std::vector<std::vector<double>> const & rf_visible,
          std::vector<std::vector<double>> const & rb_visible,
          SingleLayerOptics::BSDFHemisphere const & bsdf_hemisphere,
          double thickness_meteres,
          std::optional<double> ir_transmittance_front = std::optional<double>(),
          std::optional<double> ir_transmittance_back = std::optional<double>(),
          std::optional<double> emissivity_front = std::optional<double>(),
          std::optional<double> emissivity_back = std::optional<double>(),
          double permeability_factor = 0,
          bool flipped = false);

        SingleLayerOptics::BSDFHemisphere bsdf_hemisphere;

        std::vector<std::vector<double>> tf_solar;
        std::vector<std::vector<double>> tb_solar;
        std::vector<std::vector<double>> rf_solar;
        std::vector<std::vector<double>> rb_solar;
        std::vector<std::vector<double>> tf_visible;
        std::vector<std::vector<double>> tb_visible;
        std::vector<std::vector<double>> rf_visible;
        std::vector<std::vector<double>> rb_visible;

        std::unique_ptr<EffectiveLayers::EffectiveLayer>
          effective_thermal_values(double width,
                                   double height,
                                   double gap_width_top,
                                   double gap_width_bottom,
                                   double gap_width_left,
                                   double gap_width_right) const override;
    };

    enum class CoatedSide
    {
        FRONT,
        BACK,
        BOTH,
        NEITHER
    };

    struct Product_Data_N_Band_Optical : Product_Data_Optical
    {
        Product_Data_N_Band_Optical(
          FenestrationCommon::MaterialType material_type,
          double thickness_meteres,
          std::vector<OpticsParser::WLData> wavelength_data,
          std::optional<CoatedSide> coatedSide = std::optional<CoatedSide>(),
          std::optional<double> ir_transmittance_front = std::optional<double>(),
          std::optional<double> ir_transmittance_back = std::optional<double>(),
          std::optional<double> emissivity_front = std::optional<double>(),
          std::optional<double> emissivity_back = std::optional<double>(),
          double permeability_factor = 0,
          bool flipped = false);
        FenestrationCommon::MaterialType material_type;
        std::vector<OpticsParser::WLData> wavelength_data;
        std::optional<CoatedSide> coated_side;
        virtual std::vector<double> wavelengths() const override;
    };

    struct Product_Data_Optical_Thermal
    {
        Product_Data_Optical_Thermal() = default;
        Product_Data_Optical_Thermal(std::shared_ptr<Product_Data_Optical> optical_data,
                                     std::shared_ptr<Product_Data_Thermal> thermal_data);
        Product_Data_Optical_Thermal & operator=(Product_Data_Optical_Thermal const &) = default;
        std::shared_ptr<Product_Data_Optical> optical_data;
        std::shared_ptr<Product_Data_Thermal> thermal_data;
    };


    struct Product_Data_Optical_With_Material : Product_Data_Optical
    {
        Product_Data_Optical_With_Material(
          std::shared_ptr<Product_Data_Optical> const & material_optical_data);
        virtual ~Product_Data_Optical_With_Material() = default;
        virtual std::shared_ptr<Product_Data_Optical> optical_data() override;
        virtual std::vector<double> wavelengths() const override;

        std::shared_ptr<Product_Data_Optical> material_optical_data;
    };

    struct Product_Data_Optical_Perfectly_Diffuse : Product_Data_Optical_With_Material
    {
        Product_Data_Optical_Perfectly_Diffuse(
          std::shared_ptr<Product_Data_Optical> const & material_optical_data);
    };

    struct Venetian_Geometry
    {
        Venetian_Geometry(double slat_tilt,
                          double slat_width,
                          double slat_spacing,
                          double slat_curvature,
                          bool is_horizontal = true,
                          SingleLayerOptics::DistributionMethod distribution_method =
                            SingleLayerOptics::DistributionMethod::DirectionalDiffuse,
                          int number_slat_segments = 5);

        double slat_tilt;
        double slat_width;
        double slat_spacing;
        double slat_curvature;
        bool is_horizontal = true;
        SingleLayerOptics::DistributionMethod distribution_method =
          SingleLayerOptics::DistributionMethod::DirectionalDiffuse;
        int number_slat_segments = 5;
    };

    struct Product_Data_Optical_Venetian : Product_Data_Optical_With_Material
    {
        Product_Data_Optical_Venetian(
          std::shared_ptr<Product_Data_Optical> const & material_optical_data,
          Venetian_Geometry const & geometry);

        std::unique_ptr<EffectiveLayers::EffectiveLayer>
          effective_thermal_values(double width,
                                   double height,
                                   double gap_width_top,
                                   double gap_width_bottom,
                                   double gap_width_left,
                                   double gap_width_right) const override;

        Venetian_Geometry geometry;
    };

    struct Woven_Geometry
    {
        Woven_Geometry(double thread_diameter, double thread_spacing, double shade_thickness);

        double thread_diameter;
        double thread_spacing;
        double shade_thickness;
    };

    struct Product_Data_Optical_Woven_Shade : Product_Data_Optical_With_Material
    {
        Product_Data_Optical_Woven_Shade(
          std::shared_ptr<Product_Data_Optical> const & material_optical_data,
          Woven_Geometry const & geometry);

        std::unique_ptr<EffectiveLayers::EffectiveLayer>
          effective_thermal_values(double width,
                                   double height,
                                   double gap_width_top,
                                   double gap_width_bottom,
                                   double gap_width_left,
                                   double gap_width_right) const override;

        Woven_Geometry geometry;
    };

    struct Perforated_Geometry
    {
        enum class Type
        {
            CIRCULAR,
            RECTANGULAR,
            SQUARE
        };

        Perforated_Geometry(double spacing_x,
                            double spacing_y,
                            double dimension_x,
                            double dimension_y,
                            Perforated_Geometry::Type perforation_type);

        double spacing_x;
        double spacing_y;
        double dimension_x;
        double dimension_y;
        Perforated_Geometry::Type perforation_type;
    };

    struct Product_Data_Optical_Perforated_Screen : Product_Data_Optical_With_Material
    {
        Product_Data_Optical_Perforated_Screen(
          std::shared_ptr<Product_Data_Optical> const & material_optical_data,
          Perforated_Geometry const & geometry);

        std::unique_ptr<EffectiveLayers::EffectiveLayer>
          effective_thermal_values(double width,
                                   double height,
                                   double gap_width_top,
                                   double gap_width_bottom,
                                   double gap_width_left,
                                   double gap_width_right) const override;

        Perforated_Geometry geometry;
    };

    // etc...

}   // namespace wincalc
