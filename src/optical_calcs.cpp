
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

    double calc_optical_property(std::shared_ptr<SingleLayerOptics::IScatteringLayer> layers,
                                 Calculated_Property_Choice property_choice,
                                 Side_Choice side_choice,
                                 Scattering_Choice scattering_choice,
                                 double min_lambda,
                                 double max_lambda,
                                 double theta,
                                 double phi)
    {
        return layers->getPropertySimple(
          min_lambda, max_lambda, property_choice, side_choice, scattering_choice, theta, phi);
    }

    std::vector<WCE_Optical_Result_Absorptance<double>>
      get_layer_absorptances(std::shared_ptr<SingleLayerOptics::IScatteringLayer> layers,
                             FenestrationCommon::Side side_choice,
                             double min_lambda,
                             double max_lambda,
                             double theta = 0,
                             double phi = 0)
    {
        std::vector<WCE_Optical_Result_Absorptance<double>> absorptances;
        auto direct_absorptances_total =
          layers->getAbsorptanceLayers(min_lambda,
                                       max_lambda,
                                       side_choice,
                                       FenestrationCommon::ScatteringSimple::Direct,
                                       theta,
                                       phi);
        auto diffuse_absorptances_total =
          layers->getAbsorptanceLayers(min_lambda,
                                       max_lambda,
                                       side_choice,
                                       FenestrationCommon::ScatteringSimple::Diffuse,
                                       theta,
                                       phi);

        auto direct_absorptances_heat =
          layers->getAbsorptanceLayersHeat(min_lambda,
                                           max_lambda,
                                           side_choice,
                                           FenestrationCommon::ScatteringSimple::Direct,
                                           theta,
                                           phi);
        auto diffuse_absorptances_heat =
          layers->getAbsorptanceLayersHeat(min_lambda,
                                           max_lambda,
                                           side_choice,
                                           FenestrationCommon::ScatteringSimple::Diffuse,
                                           theta,
                                           phi);

        auto direct_absorptances_electricity =
          layers->getAbsorptanceLayersElectricity(min_lambda,
                                                  max_lambda,
                                                  side_choice,
                                                  FenestrationCommon::ScatteringSimple::Direct,
                                                  theta,
                                                  phi);

        auto diffuse_absorptances_electricity =
          layers->getAbsorptanceLayersElectricity(min_lambda,
                                                  max_lambda,
                                                  side_choice,
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

        auto bsdf_system = std::dynamic_pointer_cast<MultiLayerOptics::CMultiPaneBSDF>(layers);
        if(bsdf_system)
        {
            for(size_t i = 0; i < diffuse_absorptances_heat.size(); ++i)
            {
                auto angular_total = bsdf_system->Abs(min_lambda, max_lambda, side_choice, i + 1);
                auto angular_heat =
                  bsdf_system->AbsHeat(min_lambda, max_lambda, side_choice, i + 1);
                auto angular_electricity =
                  bsdf_system->AbsElectricity(min_lambda, max_lambda, side_choice, i + 1);
                absorptances[i].angular_total = angular_total;
                absorptances[i].angular_heat = angular_heat;
                absorptances[i].angular_electricity = angular_electricity;
            }
        }

        return absorptances;
    }

    WCE_Optical_Results calc_all(std::shared_ptr<SingleLayerOptics::IScatteringLayer> system,
                                 double min_lambda,
                                 double max_lambda,
                                 double theta,
                                 double phi)
    {
        if(max_lambda < min_lambda)
        {
            max_lambda = min_lambda + 1;
        }
        auto calc_f = [=, &system](const FenestrationCommon::PropertySimple prop,
                                   const FenestrationCommon::Side side,
                                   const FenestrationCommon::Scattering scattering) {
            return calc_optical_property(
              system, prop, side, scattering, min_lambda, max_lambda, theta, phi);
        };
        auto optical_results = do_calcs<double>(calc_f);
        auto bsdf_system = std::dynamic_pointer_cast<MultiLayerOptics::CMultiPaneBSDF>(system);
        if(bsdf_system)
        {
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
        }

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

        return optical_results;
    }

    WCE_Optical_Results
      calc_all(std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
               window_standards::Optical_Standard_Method const & method,
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
        return calc_all(
          std::move(layers), lambda_range.min_lambda, lambda_range.max_lambda, theta, phi);
    }

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
        // auto layer_y = create_multi_pane(
        //  product_data, method_y, bsdf_hemisphere, type, number_visible_bands,
        //  number_solar_bands);
        // auto layer_z = create_multi_pane(
        //  product_data, method_z, bsdf_hemisphere, type, number_visible_bands,
        //  number_solar_bands);

        auto x_wavelengths = layer_x->getWavelengths();
        // auto y_wavelengths = layer_y->getWavelengths();
        // auto z_wavelengths = layer_z->getWavelengths();

        /// if((x_wavelengths.front() != y_wavelengths.front())
        ///   || (y_wavelengths.front() != z_wavelengths.front())
        ///   || (x_wavelengths.back() != y_wavelengths.back())
        ///   || (y_wavelengths.back() != z_wavelengths.back()))
        ///{
        ///    std::stringstream err_msg;
        ///    err_msg << "Mismatched min and max wavelengths.  X: [" << x_wavelengths.front() << ",
        ///    "
        ///            << x_wavelengths.back() << "] Y: [" << y_wavelengths.front() << ", "
        ///            << y_wavelengths.back() << "] Z: [" << z_wavelengths.front() << ", "
        ///            << z_wavelengths.back() << "]" << std::endl;
        ///    throw std::runtime_error(err_msg.str());
        ///}

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

        auto color_props = std::make_shared<SingleLayerOptics::ColorProperties>(
          std::move(layer_x), source_spectrum, detector_x, detector_y, detector_z, wavelength_set);


        return calc_color_properties(color_props, theta, phi);
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
#if 0
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
#endif

    std::unique_ptr<SingleLayerOptics::IScatteringLayer>
      optical_solar_results_system_needed_for_thermal_calcs(
        std::vector<Product_Data_Optical_Thermal> const & product_data,
        window_standards::Optical_Standard const & standard,
        std::optional<SingleLayerOptics::BSDFHemisphere> bsdf_hemisphere,
        Spectal_Data_Wavelength_Range_Method const & type,
        int number_visible_bands,
        int number_solar_bands)
    {
        auto optical_layers = get_optical_layers(product_data);
        auto solar_method = standard.methods.at("SOLAR");

        std::vector<std::vector<double>> wavelengths = get_wavelengths(optical_layers);

        auto lambda_range = get_lambda_range(wavelengths, solar_method);

        return create_multi_pane(optical_layers,
                                 solar_method,
                                 bsdf_hemisphere,
                                 type,
                                 number_visible_bands,
                                 number_solar_bands);
    }

    double calc_optical_property(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
      window_standards::Optical_Standard_Method const & method,
      Calculated_Property_Choice property_choice,
      Side_Choice side_choice,
      Scattering_Choice scattering_choice,
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
                                     side_choice,
                                     scattering_choice,
                                     lambda_range.min_lambda,
                                     lambda_range.max_lambda,
                                     theta,
                                     phi);
    }
}   // namespace wincalc
