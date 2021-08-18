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
    {}

    std::vector<double> Product_Data_N_Band_Optical::wavelengths() const
    {
        return get_first_val(wavelength_data);
    }

    Flippable_Solid_Layer::Flippable_Solid_Layer(double thickness_meters, bool flipped) :
        thickness_meters(thickness_meters), flipped(flipped)
    {}

    Product_Data_Thermal::Product_Data_Thermal(double conductivity,
                                               double thickness_meters,
                                               bool flipped,
                                               double opening_top,
                                               double opening_bottom,
                                               double opening_left,
                                               double opening_right,
                                               double opening_front) :
        Flippable_Solid_Layer(thickness_meters, flipped),
        conductivity(conductivity),
        opening_top(opening_top),
        opening_bottom(opening_bottom),
        opening_left(opening_left),
        opening_right(opening_right),
        opening_front(opening_front),
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

    std::unique_ptr<EffectiveLayers::EffectiveLayer>
      Product_Data_Optical_Perfectly_Diffuse::effective_thermal_values(double width,
                                                                       double height,
                                                                       double gap_width_top,
                                                                       double gap_width_bottom,
                                                                       double gap_width_left,
                                                                       double gap_width_right) const
    {
        return Product_Data_Optical::effective_thermal_values(
          width, height, gap_width_top, gap_width_bottom, gap_width_left, gap_width_right);
    }

    Product_Data_Optical_Venetian::Product_Data_Optical_Venetian(
      std::shared_ptr<Product_Data_Optical> const & optical_data,
      double slat_tilt,
      double slat_width,
      double slat_spacing,
      double slat_curvature,
      int number_slats,
      SingleLayerOptics::DistributionMethod distribution_method) :
        Product_Data_Optical_With_Material(optical_data),
        slat_tilt(slat_tilt),
        slat_width(slat_width),
        slat_spacing(slat_spacing),
        slat_curvature(slat_curvature),
        number_slats(number_slats),
        distribution_method(distribution_method)
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

    std::unique_ptr<EffectiveLayers::EffectiveLayer>
      Product_Data_Optical_Venetian::effective_thermal_values(double width,
                                                              double height,
                                                              double gap_width_top,
                                                              double gap_width_bottom,
                                                              double gap_width_left,
                                                              double gap_width_right) const
    {
        double front_openness =
          ThermalPermeability::Venetian::openness(slat_tilt,
                                                  slat_spacing,
                                                  material_optical_data->thickness_meters,
                                                  slat_curvature,
                                                  slat_width);

        EffectiveLayers::ShadeOpenness openness{
          front_openness, gap_width_left, gap_width_right, gap_width_top, gap_width_bottom};

        return std::make_unique<EffectiveLayers::EffectiveHorizontalVenetian>(
          width, height, material_optical_data->thickness_meters, openness, slat_tilt, slat_width);
    }

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

    std::unique_ptr<EffectiveLayers::EffectiveLayer>
      Product_Data_Optical_Woven_Shade::effective_thermal_values(double width,
                                                                 double height,
                                                                 double gap_width_top,
                                                                 double gap_width_bottom,
                                                                 double gap_width_left,
                                                                 double gap_width_right) const
    {
        double front_openness =
          ThermalPermeability::Woven::openness(thread_diameter, thread_spacing);
        EffectiveLayers::ShadeOpenness openness{
          front_openness, gap_width_left, gap_width_right, gap_width_top, gap_width_bottom};

        return std::make_unique<EffectiveLayers::EffectiveLayerWoven>(
          width, height, shade_thickness, openness);
    }

    Product_Data_Optical_Thermal::Product_Data_Optical_Thermal(
      std::shared_ptr<Product_Data_Optical> optical_data,
      std::shared_ptr<Product_Data_Thermal> thermal_data) :
        optical_data(optical_data), thermal_data(thermal_data)
    {}

    Product_Data_Optical_Perforated_Screen::Product_Data_Optical_Perforated_Screen(
      std::shared_ptr<Product_Data_Optical> const & material_optical_data,
      double spacing_x,
      double spacing_y,
      double dimension_x,
      double dimension_y,
      Product_Data_Optical_Perforated_Screen::Type perforation_type) :
        Product_Data_Optical_With_Material(material_optical_data),
        spacing_x(spacing_x),
        spacing_y(spacing_y),
        dimension_x(dimension_x),
        dimension_y(dimension_y),
        perforation_type(perforation_type)
    {}

    std::unique_ptr<EffectiveLayers::EffectiveLayer>
      Product_Data_Optical_Perforated_Screen::effective_thermal_values(double width,
                                                                       double height,
                                                                       double gap_width_top,
                                                                       double gap_width_bottom,
                                                                       double gap_width_left,
                                                                       double gap_width_right) const
    {
        std::map<Product_Data_Optical_Perforated_Screen::Type, std::function<double(void)>>
          front_openness_calcs;
        front_openness_calcs[Product_Data_Optical_Perforated_Screen::Type::CIRCULAR] = [=]() {
            auto cell_dimension =
              ThermalPermeability::Perforated::diameterToXYDimension(dimension_x * 2);
            return ThermalPermeability::Perforated::openness(
              ThermalPermeability::Perforated::Geometry::Circular,
              spacing_x,
              spacing_y,
              cell_dimension.x,
              cell_dimension.y);
        };

        front_openness_calcs[Product_Data_Optical_Perforated_Screen::Type::RECTANGULAR] = [=]() {
            return ThermalPermeability::Perforated::openness(
              ThermalPermeability::Perforated::Geometry::Rectangular,
              spacing_x,
              spacing_y,
              dimension_x,
              dimension_y);
        };

        front_openness_calcs[Product_Data_Optical_Perforated_Screen::Type::SQUARE] = [=]() {
            return ThermalPermeability::Perforated::openness(
              ThermalPermeability::Perforated::Geometry::Square,
              spacing_x,
              spacing_y,
              dimension_x,
              dimension_x);
        };

        auto front_openness_calc = front_openness_calcs.find(perforation_type);

        if(front_openness_calc == front_openness_calcs.end())
        {
            std::stringstream msg;
            msg << "Unsupported perforation type: "
                << static_cast<
                     std::underlying_type<Product_Data_Optical_Perforated_Screen::Type>::type>(
                     perforation_type);
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
      SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
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

    Product_Data_Dual_Band_Optical_Specular::Product_Data_Dual_Band_Optical_Specular(
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
}   // namespace wincalc
