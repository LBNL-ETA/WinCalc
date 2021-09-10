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
    auto bsdf = SingleLayerOptics::CBSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Full);

    auto bsdf_layer = create_bsdf_layer(
      product_data.optical_data, method, bsdf, type, number_visible_bands, number_solar_bands);
    auto layer = SingleLayerOptics::CScatteringLayer(bsdf_layer);
    layer.setBlackBodySource(method.source_spectrum.t);

    std::vector<std::vector<double>> wavelengths;
    wavelengths.push_back(product_data.optical_data->wavelengths());
    auto lambda_range = get_lambda_range(wavelengths, method);

    auto tf = layer.getPropertySimple(lambda_range.min_lambda,
                                      lambda_range.max_lambda,
                                      FenestrationCommon::PropertySimple::T,
                                      FenestrationCommon::Side::Front,
                                      FenestrationCommon::Scattering::DiffuseDiffuse);
    auto tb = layer.getPropertySimple(lambda_range.min_lambda,
                                      lambda_range.max_lambda,
                                      FenestrationCommon::PropertySimple::T,
                                      FenestrationCommon::Side::Back,
                                      FenestrationCommon::Scattering::DiffuseDiffuse);


    auto polynomial_back = SingleLayerOptics::EmissivityPolynomials::NFRC_301_Uncoated;
    auto polynomial_front = SingleLayerOptics::EmissivityPolynomials::NFRC_301_Uncoated;

    auto nband_data =
      std::dynamic_pointer_cast<wincalc::Product_Data_N_Band_Optical>(product_data.optical_data);

    if(nband_data)
    {
        if(!nband_data->coated_side.has_value())
        {
            throw std::runtime_error(
              "No coated side specified for n-band data layer.  Coated side must be one of: "
              "FRONT, BACK, BOTH, or NEITHER.");
        }
        else
        {
            auto coated_side = nband_data->coated_side.value();

            // Emissivity polynomial can vary by side depending on if the side is coated
            // If "coated side" is front or both then use the coated polynomial for the
            // front hemispheric emissivity.  Else use uncoated
            polynomial_front = (coated_side == CoatedSide::FRONT || coated_side == CoatedSide::BOTH)
                                 ? SingleLayerOptics::EmissivityPolynomials::NFRC_301_Coated
                                 : SingleLayerOptics::EmissivityPolynomials::NFRC_301_Uncoated;

            // If "coated side" is back or both then use the coated polynomial for the
            // back hemispheric emissivity.  Else use uncoated
            polynomial_back = (coated_side == CoatedSide::BACK || coated_side == CoatedSide::BOTH)
                                ? SingleLayerOptics::EmissivityPolynomials::NFRC_301_Coated
                                : SingleLayerOptics::EmissivityPolynomials::NFRC_301_Uncoated;
        }
    }

    auto ir_layer = SingleLayerOptics::CScatteringLayerIR(layer);
    auto emissivity_front_hemispheric =
      ir_layer.emissivity(FenestrationCommon::Side::Front, polynomial_front);
	auto emissivity_back_hemispheric =
      ir_layer.emissivity(FenestrationCommon::Side::Back, polynomial_back);

	if(product_data.optical_data->flipped)
	{
		std::swap(tf, tb);
		std::swap(emissivity_front_hemispheric, emissivity_back_hemispheric);
	}

    return wincalc::ThermalIRResults{tf,
                                     tb,
                                     emissivity_front_hemispheric,
                                     emissivity_back_hemispheric};
}
