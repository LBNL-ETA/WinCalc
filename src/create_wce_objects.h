#ifndef WINCALC_CONVERT_H_
#define WINCALC_CONVERT_H_

#include <vector>
#include <memory>

#include <WCECommon.hpp>
#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <OpticsProductData.hpp>
#include <windows_standards/windows_standard.h>
#include <WCEGases.hpp>

#include "gap.h"

namespace wincalc
{
    struct Engine_Gap_Info
    {
        Gases::GasDef gas;
        double thickness;
    };

    enum class Spectal_Data_Wavelength_Range_Method
    {
        FULL,
        ISO_9050,
        CONDENSED
    };

    std::vector<Engine_Gap_Info> convert(std::vector<Gap_Data> const & data);

    SpectralAveraging::MeasuredRow convert(OpticsParser::WLData const & data);
    std::vector<SpectralAveraging::MeasuredRow>
      convert(std::vector<OpticsParser::WLData> const & data);
    FenestrationCommon::CSeries convert(std::vector<std::pair<double, double>> const & v);
    std::shared_ptr<SpectralAveraging::CSpectralSampleData>
      convert(std::vector<std::tuple<double, double, double, double>> const & measured_data);
    FenestrationCommon::IntegrationType convert(Integration_Rule_Type integration_rule_type);


    FenestrationCommon::CSeries get_spectum_values(Spectrum const & spectrum,
                                                   Method const & method,
                                                   OpticsParser::ProductData const & product_data);

    FenestrationCommon::CSeries
      get_spectum_values(Spectrum const & spectrum,
                         Method const & method,
                         std::vector<OpticsParser::ProductData> const & product_data);

    std::vector<double> get_wavelength_set_to_use(Method const & method,
                                                  OpticsParser::ProductData const & product_data);

#if 0   // Removing scattering layers for now
    SingleLayerOptics::CScatteringLayer
      create_scattering_layer(OpticsParser::ProductData const & product_data,
                              Method const & method);
#endif

    std::shared_ptr<SingleLayerOptics::SpecularLayer>
      create_specular_layer(OpticsParser::ProductData const & product_data,
                            Method const & method,
                            Spectal_Data_Wavelength_Range_Method const & type =
                              Spectal_Data_Wavelength_Range_Method::FULL,
                            int number_visible_bands = 5,
                            int number_solar_bands = 10);

#if 0   // remove scattering layers for now
    std::unique_ptr<MultiLayerOptics::CMultiLayerScattered>
      create_multi_layer_scattered(std::vector<OpticsParser::ProductData> const & product_data,
                                   Method const & method);
#endif

    std::unique_ptr<MultiLayerOptics::CMultiPaneSpecular>
      create_multi_pane_specular(std::vector<OpticsParser::ProductData> const & product_data,
                                 Method const & method,
                                 Spectal_Data_Wavelength_Range_Method const & type =
                                   Spectal_Data_Wavelength_Range_Method::FULL,
                                 int number_visible_bands = 5,
                                 int number_solar_bands = 10);
}   // namespace wincalc
#endif
