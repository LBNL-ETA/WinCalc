#include "thermal_ir.h"
#include "convert_optics_parser.h"

wincalc::ThermalIRResults
  wincalc::calc_thermal_ir(window_standards::Optical_Standard const & standard,
                           Product_Data_Optical_Thermal const & product_data,
                           Spectal_Data_Wavelength_Range_Method const & type,
                           int number_visible_bands,
                           int number_solar_bands)
{
    auto method = standard.methods.at("THERMAL IR");
    auto material = wincalc::create_material(
      product_data.optical_data, method, type, number_visible_bands, number_solar_bands);
    auto layer = SingleLayerOptics::CScatteringLayer::createSpecularLayer(material);
    layer.setBlackBodySource(method.source_spectrum.t);
    std::vector<std::vector<double>> wavelengths;
    wavelengths.push_back(product_data.optical_data->wavelengths());
    auto lambda_range = get_lambda_range(wavelengths, method);
    auto tf = layer.getPropertySimple(lambda_range.min_lambda,
                                      lambda_range.max_lambda,
                                      FenestrationCommon::PropertySimple::T,
                                      FenestrationCommon::Side::Front,
                                      FenestrationCommon::Scattering::DirectDirect);
    auto tb = layer.getPropertySimple(lambda_range.min_lambda,
                                      lambda_range.max_lambda,
                                      FenestrationCommon::PropertySimple::T,
                                      FenestrationCommon::Side::Back,
                                      FenestrationCommon::Scattering::DirectDirect);
    auto rf = layer.getPropertySimple(lambda_range.min_lambda,
                                      lambda_range.max_lambda,
                                      FenestrationCommon::PropertySimple::R,
                                      FenestrationCommon::Side::Front,
                                      FenestrationCommon::Scattering::DirectDirect);
    auto rb = layer.getPropertySimple(lambda_range.min_lambda,
                                      lambda_range.max_lambda,
                                      FenestrationCommon::PropertySimple::R,
                                      FenestrationCommon::Side::Back,
                                      FenestrationCommon::Scattering::DirectDirect);

    double emissivty_front = -1;
    double emissivty_back = -1;

    auto nband_data =
      std::dynamic_pointer_cast<wincalc::Product_Data_N_Band_Optical>(product_data.optical_data);
    if(nband_data)
    {
        auto polynomial = nband_data->material_type == FenestrationCommon::MaterialType::Coated
                            ? SingleLayerOptics::EmissivityPolynomials::NFRC_301_Coated
                            : SingleLayerOptics::EmissivityPolynomials::NFRC_301_Uncoated;
        emissivty_front =
          layer.normalToHemisphericalEmissivity(FenestrationCommon::Side::Front, polynomial);
        emissivty_back =
          layer.normalToHemisphericalEmissivity(FenestrationCommon::Side::Back, polynomial);
    }
    else
    {
        emissivty_front = product_data.optical_data->emissivity_front.value();
        emissivty_back = product_data.optical_data->emissivity_back.value();
    }

    return wincalc::ThermalIRResults{tf, tb, rf, rb, emissivty_front, emissivty_back};
}

wincalc::ThermalIRResults
  wincalc::calc_thermal_ir(window_standards::Optical_Standard const & standard,
                           std::shared_ptr<OpticsParser::ProductData> const & product_data,
                           Spectal_Data_Wavelength_Range_Method const & type,
                           int number_visible_bands,
                           int number_solar_bands)
{
    auto layer = wincalc::convert_to_solid_layer(product_data);
	return calc_thermal_ir(standard, layer, type, number_visible_bands, number_solar_bands);
}
