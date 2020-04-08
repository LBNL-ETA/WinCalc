#ifndef WINCALC_CONVERT_H_
#define WINCALC_CONVERT_H_

#include <vector>
#include <memory>

#include <WCECommon.hpp>
#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCEGases.hpp>
#include <WCETarcog.hpp>
#include <OpticsProductData.hpp>
#include <windows_standards/windows_standard.h>

#include "gap.h"
#include "environmental_conditions.h"
#include "product_data.h"

namespace wincalc
{
    enum class Spectal_Data_Wavelength_Range_Method
    {
        FULL,
        ISO_9050,
        CONDENSED
    };

    std::shared_ptr<Tarcog::ISO15099::CIndoorEnvironment>
      create_indoor_environment(Environment const & environment);

    std::shared_ptr<Tarcog::ISO15099::COutdoorEnvironment>
      create_outdoor_environment(Environment const & environment);

    // std::vector<Engine_Gap_Info> convert(std::vector<Gap_Data> const & data);


    // SpectralAveraging::MeasuredRow convert(OpticsParser::WLData const & data);
    SpectralAveraging::MeasuredRow convert(wincalc::Wavelength_Data const & data);

    // std::vector<SpectralAveraging::MeasuredRow> convert(std::vector<OpticsParser::WLData> const &
    // data);
    std::vector<SpectralAveraging::MeasuredRow>
      convert(std::vector<wincalc::Wavelength_Data> const & data);

    FenestrationCommon::CSeries convert(std::vector<std::pair<double, double>> const & v);

    std::shared_ptr<SpectralAveraging::CSpectralSampleData>
      convert(std::vector<std::tuple<double, double, double, double>> const & measured_data);

    FenestrationCommon::IntegrationType
      convert(window_standards::Integration_Rule_Type integration_rule_type);


    FenestrationCommon::CSeries
      get_spectum_values(window_standards::Spectrum const & spectrum,
                         window_standards::Optical_Standard_Method const & method,
                         wincalc::Product_Data_N_Band_Optical const & product_data);


    FenestrationCommon::CSeries
      get_spectum_values(window_standards::Spectrum const & spectrum,
                         window_standards::Optical_Standard_Method const & method,
                         std::shared_ptr<wincalc::Product_Data_Optical> const & product_data);

    FenestrationCommon::CSeries get_spectum_values(
      window_standards::Spectrum const & spectrum,
      window_standards::Optical_Standard_Method const & method,
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data);

    std::vector<double> get_wavelength_set_to_use(
      window_standards::Optical_Standard_Method const & method,
      std::shared_ptr<wincalc::Product_Data_Optical> const & product_data);

    std::vector<double>
      get_wavelength_set_to_use(window_standards::Optical_Standard_Method const & method,
                                wincalc::Product_Data_N_Band_Optical const & product_data);


    std::shared_ptr<SingleLayerOptics::CMaterial>
      create_material(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
                      window_standards::Optical_Standard_Method const & method,
                      Spectal_Data_Wavelength_Range_Method const & type =
                        Spectal_Data_Wavelength_Range_Method::FULL,
                      int number_visible_bands = 5,
                      int number_solar_bands = 10);

    std::shared_ptr<SingleLayerOptics::SpecularLayer>
      create_specular_layer(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
                            window_standards::Optical_Standard_Method const & method,
                            Spectal_Data_Wavelength_Range_Method const & type =
                              Spectal_Data_Wavelength_Range_Method::FULL,
                            int number_visible_bands = 5,
                            int number_solar_bands = 10);


    std::unique_ptr<MultiLayerOptics::CMultiPaneSpecular> create_multi_pane_specular(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
      window_standards::Optical_Standard_Method const & method,
      Spectal_Data_Wavelength_Range_Method const & type =
        Spectal_Data_Wavelength_Range_Method::FULL,
      int number_visible_bands = 5,
      int number_solar_bands = 10);

    std::shared_ptr<SingleLayerOptics::CBSDFLayer>
      create_bsdf_layer(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
                        window_standards::Optical_Standard_Method const & method,
                        SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
                        Spectal_Data_Wavelength_Range_Method const & type =
                          Spectal_Data_Wavelength_Range_Method::FULL,
                        int number_visible_bands = 5,
                        int number_solar_bands = 10);

    std::unique_ptr<MultiLayerOptics::CMultiPaneBSDF> create_multi_pane_bsdf(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical_With_Material>> const &
        product_optical_data,
      window_standards::Optical_Standard_Method const & method,
      SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & type =
        Spectal_Data_Wavelength_Range_Method::FULL,
      int number_visible_bands = 5,
      int number_solar_bands = 10);

    struct IGU_Info
    {
        Tarcog::ISO15099::CIGU igu;
        double t_sol;
    };

    Tarcog::ISO15099::CIGU
      create_igu(std::vector<std::shared_ptr<wincalc::Product_Data_Thermal>> const & layers,
                 std::vector<Engine_Gap_Info> const & gaps,
                 double width,
                 double height);

    Tarcog::ISO15099::CSystem create_system(Tarcog::ISO15099::CIGU & igu,
                                            Environments const & environments);

    struct Lambda_Range
    {
        double min_lambda;
        double max_lambda;
    };

    Lambda_Range get_lambda_range(wincalc::Product_Data_N_Band_Optical const & product_data,
                                  window_standards::Optical_Standard_Method const & method);
#if 0
	Lambda_Range
      get_lambda_range(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
                       window_standards::Optical_Standard_Method const & method);
#endif

    Lambda_Range get_lambda_range(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
      window_standards::Optical_Standard_Method const & method);

}   // namespace wincalc
#endif
