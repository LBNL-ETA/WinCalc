#include "convert_optics_parser.h"
#include <sstream>
#include "util.h"

namespace wincalc
{
    FenestrationCommon::MaterialType convert_material_type(std::string const & material_type)
    {
        std::map<std::string, FenestrationCommon::MaterialType> material_type_mappings{
          {"monolithic", FenestrationCommon::MaterialType::Monolithic},
          {"coated-glass", FenestrationCommon::MaterialType::Coated},
          {"coated", FenestrationCommon::MaterialType::Coated},
          {"applied-film", FenestrationCommon::MaterialType::AppliedFilm},
          {"applied film", FenestrationCommon::MaterialType::AppliedFilm},
          {"laminate", FenestrationCommon::MaterialType::Laminate}
          //{"other", FenestrationCommon::MaterialType::Monolithic}
        };

        auto itr = material_type_mappings.find(to_lower(material_type));
        if(itr == material_type_mappings.end())
        {
            std::stringstream msg;
            msg << "Material type " << material_type << " is not supported.";
            throw std::runtime_error(msg.str());
        }

        return itr->second;
    }

    CoatedSide convert_coated_side(std::string const & coated_side)
    {
        std::map<std::string, CoatedSide> mappings{
          {"front", CoatedSide::FRONT},
          {"back", CoatedSide::BACK},
          {"both", CoatedSide::BOTH},
          {"neither", CoatedSide::NEITHER},
          {"na", CoatedSide::NEITHER},
        };

        auto itr = mappings.find(to_lower(coated_side));
        if(itr == mappings.end())
        {
            std::stringstream msg;
            msg << "Coated side " << coated_side << " is not supported.";
            throw std::runtime_error(msg.str());
        }

        return itr->second;
    }

    void validate_bsdf(OpticsParser::BSDF const & bsdf)
    {
        if(bsdf.rowAngleBasisName != "LBNL/Klems Full"
           || bsdf.columnAngleBasisName != "LBNL/Klems Full")
        {
            throw std::runtime_error(
              "Only \"LBNL/Klems Full\" is currently supported for a BSDF angle basis.");
        }
        if(bsdf.data.size() != 145)
        {
            throw std::runtime_error(
              "Only \"LBNL/Klems Full\" is currently supported for a BSDF angle basis.");
        }
        for(size_t i = 0; i < 145; ++i)
        {
            if(bsdf.data[i].size() != 145)
            {
                throw std::runtime_error(
                  "Only \"LBNL/Klems Full\" is currently supported for a BSDF angle basis.");
            }
        }
    }

    double
      get_length_unit_conversion_factor(std::shared_ptr<OpticsParser::ProductData> const & product)
    {
        // Almost all lengths coming from OpticsParser are in mm so default to that.
        double length_conversion = 1.0 / 1000.0;
        if(product->thicknessUnit.has_value())
        {
            auto thickness_unit = to_lower(product->thicknessUnit.value());
            if(thickness_unit == "meter" || thickness_unit == "meters")
            {
                length_conversion = 1.0;
            }
            else if(thickness_unit == "millimeter" || thickness_unit == "millimeter")
            {
                // mm is already the default so do nothing
            }
            else
            {
                std::stringstream msg;
                msg << "Unsupported thickness unit: " << product->thicknessUnit.value()
                    << " Currently only meter and millimeter are supported.";
                throw std::runtime_error(msg.str());
            }
        }
        return length_conversion;
    }

    std::shared_ptr<Product_Data_Optical>
      convert_optical(std::shared_ptr<OpticsParser::ProductData> const & product)
    {
        std::shared_ptr<OpticsParser::ComposedProductData> composed_product =
          std::dynamic_pointer_cast<OpticsParser::ComposedProductData>(product);

        auto length_conversion = get_length_unit_conversion_factor(product);

        if(composed_product)
        {
            auto material = convert_optical(composed_product->compositionInformation->material);

            std::shared_ptr<OpticsParser::VenetianGeometry> venetian_geometry =
              std::dynamic_pointer_cast<OpticsParser::VenetianGeometry>(
                composed_product->compositionInformation->geometry);
            if(venetian_geometry)
            {
                Venetian_Geometry geometry{
                  venetian_geometry->slatTilt,
                  venetian_geometry->slatWidth * length_conversion,
                  venetian_geometry->slatSpacing * length_conversion,
                  venetian_geometry->slatCurvature * length_conversion,
                  true,
                  SingleLayerOptics::DistributionMethod::DirectionalDiffuse,
                  venetian_geometry->numberSegments};

                std::shared_ptr<Product_Data_Optical> converted(
                  new Product_Data_Optical_Venetian{material, geometry});
                return converted;
            }
            std::shared_ptr<OpticsParser::WovenGeometry> woven_geometry =
              std::dynamic_pointer_cast<OpticsParser::WovenGeometry>(
                composed_product->compositionInformation->geometry);
            if(woven_geometry)
            {
                Woven_Geometry geometry{woven_geometry->threadDiameter * length_conversion,
                                        woven_geometry->threadSpacing * length_conversion,
                                        woven_geometry->shadeThickness * length_conversion};

                std::shared_ptr<Product_Data_Optical> converted(
                  new Product_Data_Optical_Woven_Shade(material, geometry));
                return converted;
            }
            std::shared_ptr<OpticsParser::PerforatedGeometry> perforated_geometry =
              std::dynamic_pointer_cast<OpticsParser::PerforatedGeometry>(
                composed_product->compositionInformation->geometry);
            if(perforated_geometry)
            {
                Perforated_Geometry::Type perforation_type;
                auto perforation_type_str = to_lower(perforated_geometry->perforationType);
                if(perforation_type_str == "circular")
                {
                    perforation_type = Perforated_Geometry::Type::CIRCULAR;
                }
                else if(perforation_type_str == "square")
                {
                    perforation_type = Perforated_Geometry::Type::SQUARE;
                }
                else if(perforation_type_str == "rectangular")
                {
                    perforation_type = Perforated_Geometry::Type::RECTANGULAR;
                }
                else
                {
                    std::stringstream msg;
                    msg << "Unsupported perforation type: " << perforated_geometry->perforationType;
                    throw std::runtime_error(msg.str());
                }
                Perforated_Geometry geometry{perforated_geometry->spacingX * length_conversion,
                                             perforated_geometry->spacingY * length_conversion,
                                             perforated_geometry->dimensionX * length_conversion,
                                             perforated_geometry->dimensionY * length_conversion,
                                             perforation_type};
                std::shared_ptr<Product_Data_Optical> converted(
                  new Product_Data_Optical_Perforated_Screen{material, geometry});
                return converted;
            }
            // If this point is reached then the product is either missing a geometry or
            // has a geometry that is not yet supported in WinCalc
            throw std::runtime_error("Composed product with missing or unsupported geometry");
        }
        else
        {
            if(!product->measurements.has_value())
            {
                throw std::runtime_error("Missing wavelength measurements");
            }
            if(!product->thickness.has_value())
            {
                throw std::runtime_error("Missing product thickness");
            }
            auto wavelength_measured_values = product->measurements.value();
            std::shared_ptr<Product_Data_Optical> converted;
            if(std::holds_alternative<std::vector<OpticsParser::WLData>>(
                 wavelength_measured_values))
            {
                if(!product->productSubtype.has_value())
                {
                    throw std::runtime_error("Missing product subtype");
                }
                FenestrationCommon::MaterialType material_type =
                  convert_material_type(product->productSubtype.value());

                std::optional<CoatedSide> coated_side;
                if(product->coatedSide.has_value())
                {
                    coated_side = convert_coated_side(product->coatedSide.value());
                }

                converted.reset(new Product_Data_N_Band_Optical(
                  material_type,
                  product->thickness.value() * length_conversion,
                  std::get<std::vector<OpticsParser::WLData>>(wavelength_measured_values),
                  coated_side,
                  product->IRTransmittance,
                  product->IRTransmittance,
                  product->frontEmissivity,
                  product->backEmissivity));
            }
            else if(std::holds_alternative<OpticsParser::DualBandBSDF>(wavelength_measured_values))
            {
                auto bsdfHemisphere =
                  SingleLayerOptics::CBSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Full);
                auto wavelengthValues =
                  std::get<OpticsParser::DualBandBSDF>(wavelength_measured_values);
                auto solar = wavelengthValues.solar;
                auto visible = wavelengthValues.visible;
                validate_bsdf(solar.tf);
                validate_bsdf(solar.tb);
                validate_bsdf(solar.rf);
                validate_bsdf(solar.rb);
                validate_bsdf(visible.tf);
                validate_bsdf(visible.tb);
                validate_bsdf(visible.rf);
                validate_bsdf(visible.rb);
                converted.reset(new Product_Data_Dual_Band_Optical_BSDF(
                  solar.tf.data,
                  solar.tb.data,
                  solar.rf.data,
                  solar.rb.data,
                  visible.tf.data,
                  visible.tb.data,
                  visible.rf.data,
                  visible.rb.data,
                  bsdfHemisphere,
                  product->thickness.value() * length_conversion,
                  product->IRTransmittance,
                  product->IRTransmittance,
                  product->frontEmissivity,
                  product->backEmissivity,
                  product->permeabilityFactor.value_or(
                    0)));   // If permeability factor is not defined assume it is zero
            }
            return converted;
        }
    }


    wincalc::Product_Data_Thermal
      convert_thermal(std::shared_ptr<OpticsParser::ProductData> const & product)
    {
        std::shared_ptr<OpticsParser::ComposedProductData> composed_product =
          std::dynamic_pointer_cast<OpticsParser::ComposedProductData>(product);

        auto length_conversion = get_length_unit_conversion_factor(product);

        std::shared_ptr<OpticsParser::ProductData> data = product;
        if(composed_product)
        {
            data = composed_product->compositionInformation->material;
        }

        if(!data->thickness.has_value())
        {
            throw std::runtime_error("Missing thickness");
        }

        auto thermal_data = wincalc::Product_Data_Thermal(
          data->conductivity, data->thickness.value() * length_conversion, false);

        thermal_data.density = data->density;
        thermal_data.youngs_modulus = data->youngsModulus;

        return thermal_data;
    }

    wincalc::Product_Data_Optical_Thermal
      convert_to_solid_layer(std::shared_ptr<OpticsParser::ProductData> const & product)
    {
        auto optical = convert_optical(product);
        // PV power properties are properties of the layer and so far do not require any conversion
        optical->pv_power_properties = product->pvPowerProperties;
        auto thermal = std::make_shared<Product_Data_Thermal>(convert_thermal(product));
        return wincalc::Product_Data_Optical_Thermal{optical, thermal};
    }

    std::vector<wincalc::Product_Data_Optical_Thermal> convert_to_solid_layers(
      std::vector<std::shared_ptr<OpticsParser::ProductData>> const & products)
    {
        std::vector<wincalc::Product_Data_Optical_Thermal> converted;
        for(auto product : products)
        {
            converted.push_back(convert_to_solid_layer(product));
        }

        return converted;
    }


}   // namespace wincalc
