
#include <algorithm>
#include <iostream>
#include <sstream>
#include <functional>

#include <FenestrationCommon.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>

#include "optical_calcs.h"
#include "create_wce_objects.h"
#include "util.h"
#include "thermal_ir.h"

namespace wincalc
{
#if 0
    template<typename T>
    WCE_Optical_Result_Simple<T>
      do_calc(std::function<T(const FenestrationCommon::Scattering scattering)> const & f)
    {
        WCE_Optical_Result_Simple<T> calc_result;
        calc_result.direct_direct = f(FenestrationCommon::Scattering::DirectDirect);
        calc_result.direct_diffuse = f(FenestrationCommon::Scattering::DirectDiffuse);
        calc_result.diffuse_diffuse = f(FenestrationCommon::Scattering::DiffuseDiffuse);
        calc_result.direct_hemispherical = f(FenestrationCommon::Scattering::DirectHemispherical);
        return calc_result;
    }


    template<typename T>
    WCE_Optical_Results_Template<T>
      do_calcs(std::function<T(const FenestrationCommon::PropertySimple prop,
                               const FenestrationCommon::Side side,
                               const FenestrationCommon::Scattering scattering)> const & f)
    {
        WCE_Optical_Results_Template<T> calc_result;

        calc_result.system_results.front.transmittance =
          do_calc<T>([&f](const FenestrationCommon::Scattering scattering) {
              return f(
                FenestrationCommon::PropertySimple::T, FenestrationCommon::Side::Front, scattering);
          });

        calc_result.system_results.back.transmittance =
          do_calc<T>([&f](const FenestrationCommon::Scattering scattering) {
              return f(
                FenestrationCommon::PropertySimple::T, FenestrationCommon::Side::Back, scattering);
          });

        calc_result.system_results.front.reflectance =
          do_calc<T>([&f](const FenestrationCommon::Scattering scattering) {
              return f(
                FenestrationCommon::PropertySimple::R, FenestrationCommon::Side::Front, scattering);
          });

        calc_result.system_results.back.reflectance =
          do_calc<T>([&f](const FenestrationCommon::Scattering scattering) {
              return f(
                FenestrationCommon::PropertySimple::R, FenestrationCommon::Side::Back, scattering);
          });

        return calc_result;
    }

    double calc_optical_property_ORIG(std::shared_ptr<SingleLayerOptics::IScatteringLayer> layers,
                                      FenestrationCommon::PropertySimple property_choice,
                                      FenestrationCommon::Side FenestrationCommon::Side,
                                      FenestrationCommon::Scattering FenestrationCommon::Scattering,
                                      double min_lambda,
                                      double max_lambda,
                                      double theta,
                                      double phi)
    {
        return layers->getPropertySimple(
          min_lambda, max_lambda, property_choice, FenestrationCommon::Side, FenestrationCommon::Scattering, theta, phi);
    }
#endif

    std::function<
      double(FenestrationCommon::PropertySimple, FenestrationCommon::Side, FenestrationCommon::Scattering, double, double)>
      get_optical_result_function_system(
        std::shared_ptr<SingleLayerOptics::IScatteringLayer> layers,
        double min_lambda,
        double max_lambda)
    {
        return std::bind(&SingleLayerOptics::IScatteringLayer::getPropertySimple,
                         layers,
                         min_lambda,
                         max_lambda,
                         std::placeholders::_1,
                         std::placeholders::_2,
                         std::placeholders::_3,
                         std::placeholders::_4,
                         std::placeholders::_5);
    }

    std::function<std::vector<double>(
      FenestrationCommon::Side, FenestrationCommon::ScatteringSimple, double, double)>
      get_layer_absorptances_total_function(
        std::shared_ptr<SingleLayerOptics::IScatteringLayer> layers,
        double min_lambda,
        double max_lambda)
    {
        return std::bind(&SingleLayerOptics::IScatteringLayer::getAbsorptanceLayers,
                         layers,
                         min_lambda,
                         max_lambda,
                         std::placeholders::_1,
                         std::placeholders::_2,
                         std::placeholders::_3,
                         std::placeholders::_4);
    }

    std::function<std::vector<double>(
      FenestrationCommon::Side, FenestrationCommon::ScatteringSimple, double, double)>
      get_layer_absorptances_heat_function(
        std::shared_ptr<SingleLayerOptics::IScatteringLayer> layers,
        double min_lambda,
        double max_lambda)
    {
        return std::bind(&SingleLayerOptics::IScatteringLayer::getAbsorptanceLayersHeat,
                         layers,
                         min_lambda,
                         max_lambda,
                         std::placeholders::_1,
                         std::placeholders::_2,
                         std::placeholders::_3,
                         std::placeholders::_4);
    }

    std::function<std::vector<double>(
      FenestrationCommon::Side, FenestrationCommon::ScatteringSimple, double, double)>
      get_layer_absorptances_electricity_function(
        std::shared_ptr<SingleLayerOptics::IScatteringLayer> layers,
        double min_lambda,
        double max_lambda)
    {
        return std::bind(&SingleLayerOptics::IScatteringLayer::getAbsorptanceLayersElectricity,
                         layers,
                         min_lambda,
                         max_lambda,
                         std::placeholders::_1,
                         std::placeholders::_2,
                         std::placeholders::_3,
                         std::placeholders::_4);
    }

#if 0
    std::vector<WCE_Optical_Result_Absorptance<double>>
      get_layer_absorptances(std::shared_ptr<SingleLayerOptics::IScatteringLayer> layers,
                             FenestrationCommon::Side FenestrationCommon::Side,
                             double min_lambda,
                             double max_lambda,
                             double theta = 0,
                             double phi = 0)
    {
        std::vector<WCE_Optical_Result_Absorptance<double>> absorptances;

        auto direct_absorptances_total =
          std::bind(&SingleLayerOptics::IScatteringLayer::getAbsorptanceLayers,
                    layers,
                    min_lambda,
                    max_lambda,
                    FenestrationCommon::Side,
                    FenestrationCommon::ScatteringSimple::Direct,
                    std::placeholders::_1,
                    std::placeholders::_2);


        auto diffuse_absorptances_total =
          layers->getAbsorptanceLayers(min_lambda,
                                       max_lambda,
                                       FenestrationCommon::Side,
                                       FenestrationCommon::ScatteringSimple::Diffuse,
                                       theta,
                                       phi);

        auto direct_absorptances_heat =
          std::bind(&SingleLayerOptics::IScatteringLayer::getAbsorptanceLayersHeat,
                    layers,
                    min_lambda,
                    max_lambda,
                    FenestrationCommon::Side,
                    FenestrationCommon::ScatteringSimple::Direct,
                    std::placeholders::_1,
                    std::placeholders::_2);

        auto diffuse_absorptances_heat =
          layers->getAbsorptanceLayersHeat(min_lambda,
                                           max_lambda,
                                           FenestrationCommon::Side,
                                           FenestrationCommon::ScatteringSimple::Diffuse,
                                           theta,
                                           phi);

        auto direct_absorptances_electricity =
          std::bind(&SingleLayerOptics::IScatteringLayer::getAbsorptanceLayersElectricity,
                    layers,
                    min_lambda,
                    max_lambda,
                    FenestrationCommon::Side,
                    FenestrationCommon::ScatteringSimple::Direct,
                    std::placeholders::_1,
                    std::placeholders::_2);

        auto diffuse_absorptances_electricity =
          layers->getAbsorptanceLayersElectricity(min_lambda,
                                                  max_lambda,
                                                  FenestrationCommon::Side,
                                                  FenestrationCommon::ScatteringSimple::Diffuse,
                                                  theta,
                                                  phi);

        for(size_t i = 0; i < diffuse_absorptances_heat.size(); ++i)
        {
            absorptances.push_back(
              WCE_Optical_Result_Absorptance<double>{direct_absorptances_total[i],
                                                     diffuse_absorptances_total[i],
                                                     direct_absorptances_heat[i],
                                                     diffuse_absorptances_heat[i],
                                                     direct_absorptances_electricity[i],
                                                     diffuse_absorptances_electricity[i]});
        }

        return absorptances;
    }
#endif

    std::function<std::vector<std::vector<double>>(FenestrationCommon::Side,
                                                   FenestrationCommon::PropertySimple)>
      get_matrix_function(std::shared_ptr<MultiLayerOptics::CMultiPaneBSDF> system,
                          double min_lambda,
                          double max_lambda)
    {
        return
          [&](FenestrationCommon::Side side, FenestrationCommon::PropertySimple property_simple) {
              return system->getMatrix(min_lambda, max_lambda, side, property_simple).getMatrix();
          };
    }

    WCE_Optical_Results calc_all(std::shared_ptr<SingleLayerOptics::IScatteringLayer> system,
                                 double min_lambda,
                                 double max_lambda)
    {
        if(max_lambda < min_lambda)
        {
            max_lambda = min_lambda + 1;
        }
#if 0
        auto calc_f = [=, &system](const FenestrationCommon::PropertySimple prop,
                                   const FenestrationCommon::Side side,
                                   const FenestrationCommon::Scattering scattering) {
            return calc_optical_property(
              system, prop, side, scattering, min_lambda, max_lambda, theta, phi);
        };

        auto optical_results = do_calcs<double>(calc_f);
#endif
        WCE_Optical_Results optical_results;
        optical_results.system_results.optical_results_f =
          get_optical_result_function_system(system, min_lambda, max_lambda);

        auto bsdf_system = std::dynamic_pointer_cast<MultiLayerOptics::CMultiPaneBSDF>(system);
        if(bsdf_system)
        {
            optical_results.system_results.matrix_results_f =
              get_matrix_function(bsdf_system, min_lambda, max_lambda);
#if 0
            // Include matrix results for BSDF systems
            optical_results.system_results.front.transmittance.matrix =
              bsdf_system
                ->getMatrix(min_lambda,
                            max_lambda,
                            FenestrationCommon::Side::Front,
                            FenestrationCommon::PropertySimple::T)
                .getMatrix();

            optical_results.system_results.front.reflectance.matrix =
              bsdf_system
                ->getMatrix(min_lambda,
                            max_lambda,
                            FenestrationCommon::Side::Front,
                            FenestrationCommon::PropertySimple::R)
                .getMatrix();

            optical_results.system_results.back.transmittance.matrix =
              bsdf_system
                ->getMatrix(min_lambda,
                            max_lambda,
                            FenestrationCommon::Side::Back,
                            FenestrationCommon::PropertySimple::T)
                .getMatrix();

            optical_results.system_results.back.reflectance.matrix =
              bsdf_system
                ->getMatrix(min_lambda,
                            max_lambda,
                            FenestrationCommon::Side::Back,
                            FenestrationCommon::PropertySimple::R)
                .getMatrix();
#endif
        }

#if 0
        auto absorptances_front = get_layer_absorptances(
          system, FenestrationCommon::Side::Front, min_lambda, max_lambda, theta, phi);
        auto absorptances_back = get_layer_absorptances(
          system, FenestrationCommon::Side::Back, min_lambda, max_lambda, theta, phi);

        for(size_t i = 0; i < absorptances_front.size(); ++i)
        {
            optical_results.layer_results.push_back(
              WCE_Optical_Result_By_Side<WCE_Optical_Result_Layer<double>>{
                WCE_Optical_Result_Absorptance<double>{absorptances_front[i]},
                WCE_Optical_Result_Absorptance<double>{absorptances_back[i]}});
        }
#endif

        return optical_results;
    }


    WCE_Optical_Results
      calc_all(std::shared_ptr<SingleLayerOptics::IScatteringLayer> optical_system,
               Lambda_Range const & lambda_range)
    {
        // std::vector<std::vector<double>> wavelengths = get_wavelengths(product_data);
        // auto lambda_range = get_lambda_range(wavelengths, method);
        return calc_all(optical_system, lambda_range.min_lambda, lambda_range.max_lambda);
    }

#if 0
    Color_Result
      calc_color_properties(std::shared_ptr<SingleLayerOptics::ColorProperties> color_props,
                            const FenestrationCommon::PropertySimple prop,
                            const FenestrationCommon::Side side,
                            const FenestrationCommon::Scattering scattering,
                            double theta = 0,
                            double phi = 0)
    {
        auto trichromatic = color_props->getTrichromatic(prop, side, scattering, theta, phi);
        auto rgb = color_props->getRGB(prop, side, scattering, theta, phi);
        auto lab = color_props->getCIE_Lab(prop, side, scattering, theta, phi);
        return Color_Result(trichromatic, rgb, lab);
    }
#endif

#if 0
    WCE_Color_Results calc_color_properties(
      std::shared_ptr<SingleLayerOptics::ColorProperties> color_props, double theta, double phi)
    {
        auto calc_f =
          [=, &color_props](const FenestrationCommon::PropertySimple prop,
                            const FenestrationCommon::Side side,
                            const FenestrationCommon::Scattering scattering) -> Color_Result {
            return calc_color_properties(color_props, prop, side, scattering, theta, phi);
        };

        return do_calcs<Color_Result>(calc_f);
    }
#endif

#if 0
    WCE_Color_Results
      calc_color(std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
                 window_standards::Optical_Standard_Method const & method_x,
                 window_standards::Optical_Standard_Method const & method_y,
                 window_standards::Optical_Standard_Method const & method_z,
                 double theta,
                 double phi,
                 std::optional<SingleLayerOptics::BSDFHemisphere> bsdf_hemisphere,
                 Spectal_Data_Wavelength_Range_Method const & type,
                 int number_visible_bands,
                 int number_solar_bands)
    {
        auto layer_x = create_multi_pane(
          product_data, method_x, bsdf_hemisphere, type, number_visible_bands, number_solar_bands);
        auto layer_y = create_multi_pane(
          product_data, method_y, bsdf_hemisphere, type, number_visible_bands, number_solar_bands);
        auto layer_z = create_multi_pane(
          product_data, method_z, bsdf_hemisphere, type, number_visible_bands, number_solar_bands);

        auto x_wavelengths = layer_x->getWavelengths();
        auto y_wavelengths = layer_y->getWavelengths();
        auto z_wavelengths = layer_z->getWavelengths();

        if((x_wavelengths.front() != y_wavelengths.front())
           || (y_wavelengths.front() != z_wavelengths.front())
           || (x_wavelengths.back() != y_wavelengths.back())
           || (y_wavelengths.back() != z_wavelengths.back()))
        {
            std::stringstream err_msg;
            err_msg << "Mismatched min and max wavelengths.  X: [" << x_wavelengths.front() << ", "
                    << x_wavelengths.back() << "] Y: [" << y_wavelengths.front() << ", "
                    << y_wavelengths.back() << "] Z: [" << z_wavelengths.front() << ", "
                    << z_wavelengths.back() << "]" << std::endl;
            throw std::runtime_error(err_msg.str());
        }

        std::vector<std::vector<double>> wavelengths = get_wavelengths(product_data);

        auto detector_x = get_spectum_values(method_x.detector_spectrum, method_x, wavelengths);
        auto detector_y = get_spectum_values(method_y.detector_spectrum, method_y, wavelengths);
        auto detector_z = get_spectum_values(method_z.detector_spectrum, method_z, wavelengths);

        FenestrationCommon::CCommonWavelengths wavelength_combiner;
        for(auto & wavelength_set : wavelengths)
        {
            wavelength_combiner.addWavelength(wavelength_set);
        }
        auto common_wavelengths =
          wavelength_combiner.getCombinedWavelengths(FenestrationCommon::Combine::Interpolate);


        // All methods must have the same source
        // spectrum? (Should it be checked above?)
        auto source_spectrum =
          get_spectum_values(method_x.source_spectrum, method_x, common_wavelengths);

        // and the same wavelength set?
        std::vector<double> wavelength_set = combined_layer_wavelength_range_factory(
          {common_wavelengths}, type, number_visible_bands, number_solar_bands);

        auto color_props = std::make_shared<SingleLayerOptics::ColorProperties>(std::move(layer_x),
                                                                                std::move(layer_y),
                                                                                std::move(layer_z),
                                                                                source_spectrum,
                                                                                detector_x,
                                                                                detector_y,
                                                                                detector_z,
                                                                                wavelength_set);


        return calc_color_properties(color_props, theta, phi);
    }
#endif

    std::unique_ptr<SingleLayerOptics::ColorProperties> create_optical_system_color(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
      window_standards::Optical_Standard_Method const & method_x,
      window_standards::Optical_Standard_Method const & method_y,
      window_standards::Optical_Standard_Method const & method_z,
      std::optional<SingleLayerOptics::BSDFHemisphere> bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands)
    {
        auto layer_x = create_multi_pane(
          product_data, method_x, bsdf_hemisphere, type, number_visible_bands, number_solar_bands);
        auto layer_y = create_multi_pane(
          product_data, method_y, bsdf_hemisphere, type, number_visible_bands, number_solar_bands);
        auto layer_z = create_multi_pane(
          product_data, method_z, bsdf_hemisphere, type, number_visible_bands, number_solar_bands);

        auto x_wavelengths = layer_x->getWavelengths();
        auto y_wavelengths = layer_y->getWavelengths();
        auto z_wavelengths = layer_z->getWavelengths();

        if((x_wavelengths.front() != y_wavelengths.front())
           || (y_wavelengths.front() != z_wavelengths.front())
           || (x_wavelengths.back() != y_wavelengths.back())
           || (y_wavelengths.back() != z_wavelengths.back()))
        {
            std::stringstream err_msg;
            err_msg << "Mismatched min and max wavelengths.  X: [" << x_wavelengths.front() << ", "
                    << x_wavelengths.back() << "] Y: [" << y_wavelengths.front() << ", "
                    << y_wavelengths.back() << "] Z: [" << z_wavelengths.front() << ", "
                    << z_wavelengths.back() << "]" << std::endl;
            throw std::runtime_error(err_msg.str());
        }

        std::vector<std::vector<double>> wavelengths = get_wavelengths(product_data);

        auto detector_x = get_spectum_values(method_x.detector_spectrum, method_x, wavelengths);
        auto detector_y = get_spectum_values(method_y.detector_spectrum, method_y, wavelengths);
        auto detector_z = get_spectum_values(method_z.detector_spectrum, method_z, wavelengths);

        FenestrationCommon::CCommonWavelengths wavelength_combiner;
        for(auto & wavelength_set : wavelengths)
        {
            wavelength_combiner.addWavelength(wavelength_set);
        }
        auto common_wavelengths =
          wavelength_combiner.getCombinedWavelengths(FenestrationCommon::Combine::Interpolate);


        // All methods must have the same source
        // spectrum? (Should it be checked above?)
        auto source_spectrum =
          get_spectum_values(method_x.source_spectrum, method_x, common_wavelengths);

        // and the same wavelength set?
        std::vector<double> wavelength_set = combined_layer_wavelength_range_factory(
          {common_wavelengths}, type, number_visible_bands, number_solar_bands);

        auto color_system = std::make_unique<SingleLayerOptics::ColorProperties>(std::move(layer_x),
                                                                                 std::move(layer_y),
                                                                                 std::move(layer_z),
                                                                                 source_spectrum,
                                                                                 detector_x,
                                                                                 detector_y,
                                                                                 detector_z,
                                                                                 wavelength_set);

        return color_system;
    }

    WCE_Color_Results
      get_color_results_functions(std::shared_ptr<SingleLayerOptics::ColorProperties> system)
    {
        WCE_Color_Results results;
        results.system_results.lab_results_f =
          std::bind(&SingleLayerOptics::ColorProperties::getCIE_Lab,
                    system,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    std::placeholders::_5);

        results.system_results.trichromatic_results_f =
          std::bind(&SingleLayerOptics::ColorProperties::getTrichromatic,
                    system,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    std::placeholders::_5);

        results.system_results.rgb_results_f =
          std::bind(&SingleLayerOptics::ColorProperties::getRGB,
                    system,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    std::placeholders::_5);

        return results;
    }

    WCE_Color_Results
      calc_color(std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
                 window_standards::Optical_Standard_Method const & method_x,
                 window_standards::Optical_Standard_Method const & method_y,
                 window_standards::Optical_Standard_Method const & method_z,
                 std::optional<SingleLayerOptics::BSDFHemisphere> bsdf_hemisphere,
                 Spectal_Data_Wavelength_Range_Method const & type,
                 int number_visible_bands,
                 int number_solar_bands)
    {
        auto system = std::shared_ptr<SingleLayerOptics::ColorProperties>(
          create_optical_system_color(product_data,
                                      method_x,
                                      method_y,
                                      method_z,
                                      bsdf_hemisphere,
                                      type,
                                      number_visible_bands,
                                      number_solar_bands));
		return get_color_results_functions(system);
    }

    Layer_Optical_IR_Results_Needed_For_Thermal_Calcs optical_ir_results_needed_for_thermal_calcs(
      Product_Data_Optical_Thermal const & product_data,
      window_standards::Optical_Standard const & standard)
    {
        auto ir_results = calc_thermal_ir(standard, product_data);

        double tf = ir_results.transmittance_front_diffuse_diffuse;
        double tb = ir_results.transmittance_back_diffuse_diffuse;
        double absorptance_front = ir_results.emissivity_front_hemispheric;
        double absorptance_back = ir_results.emissivity_back_hemispheric;
        return Layer_Optical_IR_Results_Needed_For_Thermal_Calcs{
          tf, tb, absorptance_front, absorptance_back};
    }

    std::vector<Layer_Optical_IR_Results_Needed_For_Thermal_Calcs>
      optical_ir_results_needed_for_thermal_calcs(
        std::vector<Product_Data_Optical_Thermal> const & product_data,
        window_standards::Optical_Standard const & standard)
    {
        std::vector<Layer_Optical_IR_Results_Needed_For_Thermal_Calcs> result;
        for(auto product : product_data)
        {
            result.push_back(optical_ir_results_needed_for_thermal_calcs(product, standard));
        }
        return result;
    }

#include <iostream>

    Optical_Solar_Results_Needed_For_Thermal_Calcs optical_solar_results_needed_for_thermal_calcs(
      std::vector<Product_Data_Optical_Thermal> const & product_data,
      window_standards::Optical_Standard const & standard,
      double theta,
      double phi,
      std::optional<SingleLayerOptics::BSDFHemisphere> bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands)
    {
        auto optical_layers = get_optical_layers(product_data);
        auto solar_method = standard.methods.at("SOLAR");

        std::vector<std::vector<double>> wavelengths = get_wavelengths(optical_layers);

        auto lambda_range = get_lambda_range(wavelengths, solar_method);

        auto layers = create_multi_pane(optical_layers,
                                        solar_method,
                                        bsdf_hemisphere,
                                        type,
                                        number_visible_bands,
                                        number_solar_bands);

        double t_sol =
          layers->getPropertySimple(lambda_range.min_lambda,
                                    lambda_range.max_lambda,
                                    FenestrationCommon::PropertySimple::T,
                                    FenestrationCommon::Side::Front,
                                    FenestrationCommon::Scattering::DirectHemispherical,
                                    theta,
                                    phi);

        std::vector<double> layer_absorptances =
          layers->getAbsorptanceLayersHeat(lambda_range.min_lambda,
                                           lambda_range.max_lambda,
                                           FenestrationCommon::Side::Front,
                                           FenestrationCommon::ScatteringSimple::Direct,
                                           theta,
                                           phi);

        return Optical_Solar_Results_Needed_For_Thermal_Calcs{t_sol, layer_absorptances};
    }

#if 0
    double calc_optical_property(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
      window_standards::Optical_Standard_Method const & method,
      FenestrationCommon::PropertySimple property_choice,
      FenestrationCommon::Side FenestrationCommon::Side,
      FenestrationCommon::Scattering FenestrationCommon::Scattering,
      double theta,
      double phi,
      std::optional<SingleLayerOptics::BSDFHemisphere> bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands)
    {
        auto layers = create_multi_pane(
          product_data, method, bsdf_hemisphere, type, number_visible_bands, number_solar_bands);
        std::vector<std::vector<double>> wavelengths = get_wavelengths(product_data);
        auto lambda_range = get_lambda_range(wavelengths, method);

        return calc_optical_property(std::move(layers),
                                     property_choice,
                                     FenestrationCommon::Side,
                                     FenestrationCommon::Scattering,
                                     lambda_range.min_lambda,
                                     lambda_range.max_lambda,
                                     theta,
                                     phi);
    }
#endif
}   // namespace wincalc
