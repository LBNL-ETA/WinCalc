#include <sstream>
#include "product_data.h"
#include "create_wce_objects.h"
#include "util.h"

namespace wincalc
{
    Product_Data_Optical::Product_Data_Optical(double thickness_meters,
                                               std::optional<double> ir_transmittance_front,
                                               std::optional<double> ir_transmittance_back,
                                               std::optional<double> emissivity_front,
                                               std::optional<double> emissivity_back,
                                               bool flipped) :
        Flippable_Solid_Layer(thickness_meters, flipped),
        ir_transmittance_front(ir_transmittance_front),
        ir_transmittance_back(ir_transmittance_back),
        emissivity_front(emissivity_front),
        emissivity_back(emissivity_back)
    {}

    Product_Data_Optical::~Product_Data_Optical()
    {}

    std::shared_ptr<Product_Data_Optical> Product_Data_Optical::optical_data()
    {
        return shared_from_this();
    }

    std::unique_ptr<EffectiveLayers::EffectiveLayer>
      Product_Data_Optical::effective_thermal_values(double width,
                                                     double height,
                                                     double gap_width_top,
                                                     double gap_width_bottom,
                                                     double gap_width_left,
                                                     double gap_width_right,
                                                     double,
                                                     double permeability_factor) const
    {
        EffectiveLayers::ShadeOpenness openness{
          gap_width_left, gap_width_right, gap_width_top, gap_width_bottom};

        return std::make_unique<EffectiveLayers::EffectiveLayerCommon>(
          width, height, thickness_meters, permeability_factor, openness);
    }


    Product_Data_N_Band_Optical::Product_Data_N_Band_Optical(
      FenestrationCommon::MaterialType material_type,
      double thickness_meters,
      std::vector<OpticsParser::WLData> wavelength_data,
      std::optional<CoatedSide> coated_side,
      std::optional<double> ir_transmittance_front,
      std::optional<double> ir_transmittance_back,
      std::optional<double> emissivity_front,
      std::optional<double> emissivity_back,
      bool flipped) :
        Product_Data_Optical(thickness_meters,
                             ir_transmittance_front,
                             ir_transmittance_back,
                             emissivity_front,
                             emissivity_back,
                             flipped),
        material_type(material_type),
        wavelength_data(wavelength_data),
        coated_side(coated_side)
    {
        std::sort(this->wavelength_data.begin(),
                  this->wavelength_data.end(),
                  [](OpticsParser::WLData const & v1, OpticsParser::WLData const & v2) {
                      return v1.wavelength < v2.wavelength;
                  });
    }

    std::vector<double> Product_Data_N_Band_Optical::wavelengths() const
    {
        return get_first_val(wavelength_data);
    }

    Flippable_Solid_Layer::Flippable_Solid_Layer(double thickness_meters, bool flipped) :
        thickness_meters(thickness_meters), flipped(flipped)
    {}

    Product_Data_Thermal::Product_Data_Thermal(std::optional<double> conductivity,
                                               double thickness_meters,
                                               bool flipped,
                                               double opening_top,
                                               double opening_bottom,
                                               double opening_left,
                                               double opening_right,
                                               double effective_front_thermal_openness_area,
                                               double permeability_factor,
                                               std::optional<double> youngs_modulus,
                                               std::optional<double> density) :
        Flippable_Solid_Layer(thickness_meters, flipped),
        conductivity(conductivity),
        opening_top(opening_top),
        opening_bottom(opening_bottom),
        opening_left(opening_left),
        opening_right(opening_right),
        effective_front_thermal_openness_area(effective_front_thermal_openness_area),
        permeability_factor(permeability_factor),
        youngs_modulus(youngs_modulus),
        density(density)
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
    std::vector<double> Product_Data_Optical_With_Material::wavelengths() const
    {
        return material_optical_data->wavelengths();
    }

    Product_Data_Optical_Perfectly_Diffuse::Product_Data_Optical_Perfectly_Diffuse(
      const std::shared_ptr<Product_Data_Optical> & material_data) :
        Product_Data_Optical_With_Material(material_data)
    {}

    Product_Data_Optical_Uniform_Diffuse::Product_Data_Optical_Uniform_Diffuse(
      const std::shared_ptr<Product_Data_Optical> & material_data) :
        Product_Data_Optical_With_Material(material_data)
    {}

    Product_Data_Optical_Venetian::Product_Data_Optical_Venetian(
      std::shared_ptr<Product_Data_Optical> const & optical_data,
      Venetian_Geometry const & geometry) :
        Product_Data_Optical_With_Material(optical_data), geometry(geometry)
    {}

    std::unique_ptr<EffectiveLayers::EffectiveLayer>
      Product_Data_Optical_Venetian::effective_thermal_values(
        double width,
        double height,
        double gap_width_top,
        double gap_width_bottom,
        double gap_width_left,
        double gap_width_right,
        double,
        double) const   // Front openness is automatically calculated by the model
    {
        // TODO: make sure units match
        FenestrationCommon::Venetian::Geometry wce_geometry{
          geometry.slat_width, geometry.slat_spacing, geometry.slat_tilt, geometry.slat_curvature};

        // double front_openness_venetian =
        //   ThermalPermeability::Venetian::permeabilityFactor(material_optical_data->thickness_meters,
        //   wce_geometry);

        EffectiveLayers::ShadeOpenness openness{
          gap_width_left, gap_width_right, gap_width_top, gap_width_bottom};

        return std::make_unique<EffectiveLayers::EffectiveHorizontalVenetian>(
          width, height, material_optical_data->thickness_meters, wce_geometry, openness);
    }

    Product_Data_Optical_Woven_Shade::Product_Data_Optical_Woven_Shade(
      std::shared_ptr<Product_Data_Optical> const & material_optical_data,
      Woven_Geometry const & geometry) :
        Product_Data_Optical_With_Material(material_optical_data), geometry(geometry)
    {}

    std::unique_ptr<EffectiveLayers::EffectiveLayer>
      Product_Data_Optical_Woven_Shade::effective_thermal_values(
        double width,
        double height,
        double gap_width_top,
        double gap_width_bottom,
        double gap_width_left,
        double gap_width_right,
        double,
        double) const   // calculated automatically by the code
    {
        FenestrationCommon::Woven::Geometry wce_geometry{geometry.thread_diameter,
                                                         geometry.thread_spacing};
        EffectiveLayers::ShadeOpenness openness{
          gap_width_left, gap_width_right, gap_width_top, gap_width_bottom};

        return std::make_unique<EffectiveLayers::EffectiveLayerWoven>(
          width, height, geometry.shade_thickness, wce_geometry, openness);
    }

    Product_Data_Optical_Thermal::Product_Data_Optical_Thermal(
      std::shared_ptr<Product_Data_Optical> optical_data,
      std::shared_ptr<Product_Data_Thermal> thermal_data) :
        optical_data(optical_data), thermal_data(thermal_data)
    {}

    Product_Data_Optical_Perforated_Screen::Product_Data_Optical_Perforated_Screen(
      std::shared_ptr<Product_Data_Optical> const & material_optical_data,
      Perforated_Geometry const & geometry) :
        Product_Data_Optical_With_Material(material_optical_data), geometry(geometry)
    {}

    std::unique_ptr<EffectiveLayers::EffectiveLayer>
      Product_Data_Optical_Perforated_Screen::effective_thermal_values(
        double width,
        double height,
        double gap_width_top,
        double gap_width_bottom,
        double gap_width_left,
        double gap_width_right,
        double,
        double) const   // Calculated automatically by the code
    {
        std::map<wincalc::Perforated_Geometry::Type, FenestrationCommon::Perforated::Type> wce_map{
          {wincalc::Perforated_Geometry::Type::CIRCULAR,
           FenestrationCommon::Perforated::Type::Circular},
          {wincalc::Perforated_Geometry::Type::SQUARE,
           FenestrationCommon::Perforated::Type::Square},
          {wincalc::Perforated_Geometry::Type::RECTANGULAR,
           FenestrationCommon::Perforated::Type::Rectangular}};

        FenestrationCommon::Perforated::Geometry wce_geometry{wce_map[geometry.perforation_type],
                                                              geometry.spacing_x,
                                                              geometry.spacing_y,
                                                              geometry.dimension_x,
                                                              geometry.dimension_y};


        EffectiveLayers::ShadeOpenness openness{
          gap_width_left, gap_width_right, gap_width_top, gap_width_bottom};

        return std::make_unique<EffectiveLayers::EffectiveLayerPerforated>(
          width, height, material_optical_data->thickness_meters, wce_geometry, openness);
    }

    Product_Data_Dual_Band_Optical_BSDF::Product_Data_Dual_Band_Optical_BSDF(
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
      std::optional<double> ir_transmittance_front,
      std::optional<double> ir_transmittance_back,
      std::optional<double> emissivity_front,
      std::optional<double> emissivity_back,
      bool flipped,
      bool user_defined_effective_values) :
        Product_Data_Dual_Band_Optical(thickness_meteres,
                                       ir_transmittance_front,
                                       ir_transmittance_back,
                                       emissivity_front,
                                       emissivity_back,
                                       flipped),
        bsdf_hemisphere(bsdf_hemisphere),
        tf_solar(tf_solar),
        tb_solar(tb_solar),
        rf_solar(rf_solar),
        rb_solar(rb_solar),
        tf_visible(tf_visible),
        tb_visible(tb_visible),
        rf_visible(rf_visible),
        rb_visible(rb_visible),
        user_defined_effective_values(user_defined_effective_values)
    {}

    std::unique_ptr<EffectiveLayers::EffectiveLayer>
      Product_Data_Dual_Band_Optical_BSDF::effective_thermal_values(
        double width,
        double height,
        double gap_width_top,
        double gap_width_bottom,
        double gap_width_left,
        double gap_width_right,
        double effective_thermal_front_openness_area,
        double permeability_factor) const
    {
        EffectiveLayers::ShadeOpenness openness{
          gap_width_left, gap_width_right, gap_width_top, gap_width_bottom};

        if(!user_defined_effective_values)
        {
            return std::make_unique<EffectiveLayers::EffectiveLayerCommon>(
              width, height, thickness_meters, permeability_factor, openness);
        }
        else
        {
            return std::make_unique<EffectiveLayers::EffectiveLayerUserDefined>(
              width,
              height,
              thickness_meters,
              permeability_factor,
              effective_thermal_front_openness_area,
              openness);
        }
    }

    Product_Data_Optical_Louvered_Shutter_BSDF::Product_Data_Optical_Louvered_Shutter_BSDF(
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
      OpticsParser::LouveredGeometry const & geometry,
      std::optional<double> ir_transmittance_front,
      std::optional<double> ir_transmittance_back,
      std::optional<double> emissivity_front,
      std::optional<double> emissivity_back,
      bool flipped,
      bool user_defined_effective_values) :
        Product_Data_Dual_Band_Optical_BSDF(tf_solar,
                                            tb_solar,
                                            rf_solar,
                                            rb_solar,
                                            tf_visible,
                                            tb_visible,
                                            rf_visible,
                                            rb_visible,
                                            bsdf_hemisphere,
                                            thickness_meteres,
                                            ir_transmittance_front,
                                            ir_transmittance_back,
                                            emissivity_front,
                                            emissivity_back,
                                            flipped,
                                            user_defined_effective_values),
        geometry(geometry)
    {}

    std::unique_ptr<EffectiveLayers::EffectiveLayer>
      Product_Data_Optical_Louvered_Shutter_BSDF::effective_thermal_values(double width,
                                                                           double height,
                                                                           double gap_width_top,
                                                                           double gap_width_bottom,
                                                                           double gap_width_left,
                                                                           double gap_width_right,
                                                                           double,
                                                                           double) const
    {
        EffectiveLayers::ShadeOpenness openness{
          gap_width_left, gap_width_right, gap_width_top, gap_width_bottom};

        FenestrationCommon::LouveredShutter::Geometry wce_geometry{
          geometry.slatWidth, geometry.slatThickness, geometry.slatAngle, geometry.slatSpacing};

        return std::make_unique<EffectiveLayers::EffectiveLayerLouveredShutter>(
          width, height, thickness_meters, wce_geometry, openness);
    }

    Product_Data_Dual_Band_Optical_Hemispheric::Product_Data_Dual_Band_Optical_Hemispheric(
      double tf_solar,
      double tb_solar,
      double rf_solar,
      double rb_solar,
      double tf_visible,
      double tb_visible,
      double rf_visible,
      double rb_visible,
      double thickness_meteres,
      std::optional<double> ir_transmittance_front,
      std::optional<double> ir_transmittance_back,
      std::optional<double> emissivity_front,
      std::optional<double> emissivity_back,
      bool flipped) :
        Product_Data_Dual_Band_Optical(thickness_meteres,
                                       ir_transmittance_front,
                                       ir_transmittance_back,
                                       emissivity_front,
                                       emissivity_back,
                                       flipped),
        tf_solar(tf_solar),
        tb_solar(tb_solar),
        rf_solar(rf_solar),
        rb_solar(rb_solar),
        tf_visible(tf_visible),
        tb_visible(tb_visible),
        rf_visible(rf_visible),
        rb_visible(rb_visible)
    {}


    Venetian_Geometry::Venetian_Geometry(double slat_tilt,
                                         double slat_width,
                                         double slat_spacing,
                                         double slat_curvature,
                                         bool is_horizontal,
                                         SingleLayerOptics::DistributionMethod distribution_method,
                                         int number_slat_segments) :
        slat_tilt(slat_tilt),
        slat_width(slat_width),
        slat_spacing(slat_spacing),
        slat_curvature(slat_curvature),
        is_horizontal(is_horizontal),
        distribution_method(distribution_method),
        number_slat_segments(number_slat_segments)
    {}

    Woven_Geometry::Woven_Geometry(double thread_diameter,
                                   double thread_spacing,
                                   double shade_thickness) :
        thread_diameter(thread_diameter),
        thread_spacing(thread_spacing),
        shade_thickness(shade_thickness)
    {}

    Perforated_Geometry::Perforated_Geometry(double spacing_x,
                                             double spacing_y,
                                             double dimension_x,
                                             double dimension_y,
                                             Perforated_Geometry::Type perforation_type) :
        spacing_x(spacing_x),
        spacing_y(spacing_y),
        dimension_x(dimension_x),
        dimension_y(dimension_y),
        perforation_type(perforation_type)
    {}
}   // namespace wincalc
