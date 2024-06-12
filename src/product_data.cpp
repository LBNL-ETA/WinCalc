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
                                               double permeability_factor,
                                               bool flipped) :
        Flippable_Solid_Layer(thickness_meters, flipped),
        ir_transmittance_front(ir_transmittance_front),
        ir_transmittance_back(ir_transmittance_back),
        emissivity_front(emissivity_front),
        emissivity_back(emissivity_back),
        permeability_factor(permeability_factor)
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
                                                     double gap_width_right) const
    {
        EffectiveLayers::ShadeOpenness openness{
          permeability_factor, gap_width_left, gap_width_right, gap_width_top, gap_width_bottom};

        return std::make_unique<EffectiveLayers::EffectiveLayerOther>(
          width, height, thickness_meters, openness);
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
      double permeability_factor,
      bool flipped) :
        Product_Data_Optical(thickness_meters,
                             ir_transmittance_front,
                             ir_transmittance_back,
                             emissivity_front,
                             emissivity_back,
                             permeability_factor,
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

    Product_Data_Thermal::Product_Data_Thermal(
      std::optional<double> conductivity,
      double thickness_meters,
      bool flipped,
      double opening_top,
      double opening_bottom,
      double opening_left,
      double opening_right,
      std::optional<double> effective_openness,
      std::optional<double> effective_thickness) :
        Flippable_Solid_Layer(thickness_meters, flipped),
        conductivity(conductivity),
        opening_top(opening_top),
        opening_bottom(opening_bottom),
        opening_left(opening_left),
        opening_right(opening_right),
        effective_openness(effective_openness),
        effective_thickness(effective_thickness),
        youngs_modulus(Tarcog::DeflectionConstants::YOUNGSMODULUS),
        density(Tarcog::MaterialConstants::GLASSDENSITY)
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
    {
        permeability_factor = material_data->permeability_factor;
    }

    Product_Data_Optical_Venetian::Product_Data_Optical_Venetian(
      std::shared_ptr<Product_Data_Optical> const & optical_data,
      Venetian_Geometry const & geometry) :
        Product_Data_Optical_With_Material(optical_data), geometry(geometry)
    {}

    std::unique_ptr<EffectiveLayers::EffectiveLayer>
      Product_Data_Optical_Venetian::effective_thermal_values(double width,
                                                              double height,
                                                              double gap_width_top,
                                                              double gap_width_bottom,
                                                              double gap_width_left,
                                                              double gap_width_right) const
    {
        double front_openness =
          ThermalPermeability::Venetian::openness(geometry.slat_tilt,
                                                  geometry.slat_spacing,
                                                  material_optical_data->thickness_meters,
                                                  geometry.slat_curvature,
                                                  geometry.slat_width);

        EffectiveLayers::ShadeOpenness openness{
          front_openness, gap_width_left, gap_width_right, gap_width_top, gap_width_bottom};

        return std::make_unique<EffectiveLayers::EffectiveHorizontalVenetian>(
          width,
          height,
          material_optical_data->thickness_meters,
          openness,
          geometry.slat_tilt,
          geometry.slat_width);
    }

    Product_Data_Optical_Woven_Shade::Product_Data_Optical_Woven_Shade(
      std::shared_ptr<Product_Data_Optical> const & material_optical_data,
      Woven_Geometry const & geometry) :
        Product_Data_Optical_With_Material(material_optical_data), geometry(geometry)
    {}

    std::unique_ptr<EffectiveLayers::EffectiveLayer>
      Product_Data_Optical_Woven_Shade::effective_thermal_values(double width,
                                                                 double height,
                                                                 double gap_width_top,
                                                                 double gap_width_bottom,
                                                                 double gap_width_left,
                                                                 double gap_width_right) const
    {
        double front_openness =
          ThermalPermeability::Woven::openness(geometry.thread_diameter, geometry.thread_spacing);
        EffectiveLayers::ShadeOpenness openness{
          front_openness, gap_width_left, gap_width_right, gap_width_top, gap_width_bottom};

        return std::make_unique<EffectiveLayers::EffectiveLayerWoven>(
          width, height, geometry.shade_thickness, openness);
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
      Product_Data_Optical_Perforated_Screen::effective_thermal_values(double width,
                                                                       double height,
                                                                       double gap_width_top,
                                                                       double gap_width_bottom,
                                                                       double gap_width_left,
                                                                       double gap_width_right) const
    {
        std::map<Perforated_Geometry::Type, std::function<double(void)>> front_openness_calcs;
        front_openness_calcs[Perforated_Geometry::Type::CIRCULAR] = [=]() {
            auto cell_dimension =
              ThermalPermeability::Perforated::diameterToXYDimension(geometry.dimension_x * 2);
            return ThermalPermeability::Perforated::openness(
              ThermalPermeability::Perforated::Geometry::Circular,
              geometry.spacing_x,
              geometry.spacing_y,
              cell_dimension.x,
              cell_dimension.y);
        };

        front_openness_calcs[Perforated_Geometry::Type::RECTANGULAR] = [=]() {
            return ThermalPermeability::Perforated::openness(
              ThermalPermeability::Perforated::Geometry::Rectangular,
              geometry.spacing_x,
              geometry.spacing_y,
              geometry.dimension_x,
              geometry.dimension_y);
        };

        front_openness_calcs[Perforated_Geometry::Type::SQUARE] = [=]() {
            return ThermalPermeability::Perforated::openness(
              ThermalPermeability::Perforated::Geometry::Square,
              geometry.spacing_x,
              geometry.spacing_y,
              geometry.dimension_x,
              geometry.dimension_x);
        };

        auto front_openness_calc = front_openness_calcs.find(geometry.perforation_type);

        if(front_openness_calc == front_openness_calcs.end())
        {
            std::stringstream msg;
            msg << "Unsupported perforation type: "
                << static_cast<std::underlying_type<Perforated_Geometry::Type>::type>(
                     geometry.perforation_type);
            throw std::runtime_error(msg.str());
        }

        double front_openness = front_openness_calc->second();

        EffectiveLayers::ShadeOpenness openness{
          front_openness, gap_width_left, gap_width_right, gap_width_top, gap_width_bottom};

        return std::make_unique<EffectiveLayers::EffectiveLayerPerforated>(
          width, height, material_optical_data->thickness_meters, openness);
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
      double permeability_factor,
      bool flipped) :
        Product_Data_Dual_Band_Optical(thickness_meteres,
                                       ir_transmittance_front,
                                       ir_transmittance_back,
                                       emissivity_front,
                                       emissivity_back,
                                       permeability_factor,
                                       flipped),
        bsdf_hemisphere(bsdf_hemisphere),
        tf_solar(tf_solar),
        tb_solar(tb_solar),
        rf_solar(rf_solar),
        rb_solar(rb_solar),
        tf_visible(tf_visible),
        tb_visible(tb_visible),
        rf_visible(rf_visible),
        rb_visible(rb_visible)
    {}

    std::unique_ptr<EffectiveLayers::EffectiveLayer>
      Product_Data_Dual_Band_Optical_BSDF::effective_thermal_values(double width,
                                                                    double height,
                                                                    double gap_width_top,
                                                                    double gap_width_bottom,
                                                                    double gap_width_left,
                                                                    double gap_width_right) const
    {
        EffectiveLayers::ShadeOpenness openness{
          permeability_factor, gap_width_left, gap_width_right, gap_width_top, gap_width_bottom};

        return std::make_unique<EffectiveLayers::EffectiveLayerBSDF>(
          width, height, thickness_meters, openness);
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
      double permeability_factor,
      bool flipped) :
        Product_Data_Dual_Band_Optical(thickness_meteres,
                                       ir_transmittance_front,
                                       ir_transmittance_back,
                                       emissivity_front,
                                       emissivity_back,
                                       permeability_factor,
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
