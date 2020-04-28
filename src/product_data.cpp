#include "product_data.h"
#include "create_wce_objects.h"
#include <sstream>

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

    void Product_Data_Optical::set_slat_tilt(double)
    {
        throw std::runtime_error("Can only set slat tilt for Venetian products.");
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
          0, gap_width_left, gap_width_right, gap_width_top, gap_width_bottom};

        return std::make_unique<EffectiveLayers::EffectiveLayerOther>(
          width, height, thickness_meters, openness);
    }


    Product_Data_N_Band_Optical::Product_Data_N_Band_Optical(
      FenestrationCommon::MaterialType material_type,
      double thickness_meters,
      std::vector<Wavelength_Data> wavelength_data,
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
        wavelength_data(wavelength_data)
    {}

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
        opening_front(opening_front)
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

    void Product_Data_Optical_Venetian::set_slat_tilt(double tilt)
    {
        slat_tilt = tilt;
    }

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
          width, height, material_optical_data->thickness_meters, openness);
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

}   // namespace wincalc
