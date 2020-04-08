
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

        return calc_result;
    }


    template<typename T>
    WCE_Optical_Result_By_Side<T>
      do_calcs(std::function<T(const FenestrationCommon::PropertySimple prop,
                               const FenestrationCommon::Side side,
                               const FenestrationCommon::Scattering scattering)> const & f)
    {
        WCE_Optical_Result_By_Side<T> calc_result;

        calc_result.tf = do_calc<T>([&f](const FenestrationCommon::Scattering scattering) {
            return f(
              FenestrationCommon::PropertySimple::T, FenestrationCommon::Side::Front, scattering);
        });

        calc_result.tb = do_calc<T>([&f](const FenestrationCommon::Scattering scattering) {
            return f(
              FenestrationCommon::PropertySimple::T, FenestrationCommon::Side::Back, scattering);
        });

        calc_result.rf = do_calc<T>([&f](const FenestrationCommon::Scattering scattering) {
            return f(
              FenestrationCommon::PropertySimple::R, FenestrationCommon::Side::Front, scattering);
        });

        calc_result.rb = do_calc<T>([&f](const FenestrationCommon::Scattering scattering) {
            return f(
              FenestrationCommon::PropertySimple::R, FenestrationCommon::Side::Back, scattering);
        });

        return calc_result;
    }

    double calc_optical_property(MultiLayerOptics::CMultiPaneSpecular & layer,
                                 Calculated_Property_Choice property_choice,
                                 Side_Choice side_choice,
                                 Scattering_Choice scattering_choice,
                                 double theta,
                                 double phi)
    {
        return layer.getPropertySimple(property_choice, side_choice, scattering_choice, theta, phi);
    }

    WCE_Optical_Result_Absorptance<double>
      get_layer_absorptances(MultiLayerOptics::CMultiPaneSpecular & layers,
                             FenestrationCommon::Side side_choice,
                             double min_lambda,
                             double max_lambda,
                             double theta = 0,
                             double phi = 0)
    {
        WCE_Optical_Result_Absorptance<double> absorptances;
        absorptances.direct =
          layers.getAbsorptanceLayers(min_lambda,
                                      max_lambda,
                                      side_choice,
                                      FenestrationCommon::ScatteringSimple::Direct,
                                      theta,
                                      phi);
        absorptances.diffuse =
          layers.getAbsorptanceLayers(min_lambda,
                                      max_lambda,
                                      side_choice,
                                      FenestrationCommon::ScatteringSimple::Diffuse,
                                      theta,
                                      phi);

        return absorptances;
    }

    WCE_Optical_Results calc_all(MultiLayerOptics::CMultiPaneSpecular & layers,
                                 double min_lambda,
                                 double max_lambda,
                                 double theta,
                                 double phi)
    {
        auto calc_f = [=, &layers](const FenestrationCommon::PropertySimple prop,
                                   const FenestrationCommon::Side side,
                                   const FenestrationCommon::Scattering scattering) {
            return calc_optical_property(layers, prop, side, scattering, theta, phi);
        };

        auto optical_results = do_calcs<double>(calc_f);
        optical_results.absorptances_front = get_layer_absorptances(
          layers, FenestrationCommon::Side::Front, min_lambda, max_lambda, theta, phi);
        optical_results.absorptances_back = get_layer_absorptances(
          layers, FenestrationCommon::Side::Back, min_lambda, max_lambda, theta, phi);
        return optical_results;
    }

#if 0
    WCE_Optical_Results
      calc_all(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
               window_standards::Optical_Standard_Method const & method,
               double theta,
               double phi)
    {
        return calc_all({product_data}, method, theta, phi);
    }
#endif

    WCE_Optical_Results
      calc_all(std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
               window_standards::Optical_Standard_Method const & method,
               double theta,
               double phi)
    {
        auto layers = create_multi_pane_specular(product_data, method);
        auto lambda_range = get_lambda_range(product_data, method);
        return calc_all(*layers, lambda_range.min_lambda, lambda_range.max_lambda, theta, phi);
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
                 double phi)
    {
        auto layer_x = create_multi_pane_specular(product_data, method_x);
        auto layer_y = create_multi_pane_specular(product_data, method_y);
        auto layer_z = create_multi_pane_specular(product_data, method_z);

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


        auto detector_x = get_spectum_values(method_x.detector_spectrum, method_x, product_data);
        auto detector_y = get_spectum_values(method_y.detector_spectrum, method_y, product_data);
        auto detector_z = get_spectum_values(method_z.detector_spectrum, method_z, product_data);

        // All methods must have the same source
        // spectrum? (Should it be checked above?)
        auto source_spectrum =
          get_spectum_values(method_x.source_spectrum, method_x, product_data[0]);

        // and the same wavelength set?
        std::vector<double> wavelength_set = get_wavelength_set_to_use(method_x, product_data[0]);

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


    Optical_Results_Needed_For_Thermal_Calcs optical_results_needed_for_thermal_calcs(
      std::vector<std::shared_ptr<Product_Data_Optical>> const & solid_layers,
      window_standards::Optical_Standard const & standard,
      double theta,
      double phi)
    {
        auto solar_method =
          standard.methods.at(window_standards::Optical_Standard_Method_Type::SOLAR);

        auto multi_pane_specular = create_multi_pane_specular(solid_layers, solar_method);

        double t_sol =
          multi_pane_specular->getPropertySimple(FenestrationCommon::PropertySimple::T,
                                                 FenestrationCommon::Side::Front,
                                                 FenestrationCommon::Scattering::DirectDirect,
                                                 theta,
                                                 phi);

        auto lambda_range = get_lambda_range(solid_layers, solar_method);

        std::vector<double> layer_absorptances =
          multi_pane_specular->getAbsorptanceLayers(lambda_range.min_lambda,
                                                    lambda_range.max_lambda,
                                                    FenestrationCommon::Side::Front,
                                                    FenestrationCommon::ScatteringSimple::Direct,
                                                    theta,
                                                    phi);

        return Optical_Results_Needed_For_Thermal_Calcs{t_sol, layer_absorptances};
    }

    double
      calc_optical_property(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
                            window_standards::Optical_Standard_Method const & method,
                            Calculated_Property_Choice property_choice,
                            Side_Choice side_choice,
                            Scattering_Choice scattering_choice,
                            double theta,
                            double phi)
    {
        auto layer = create_multi_pane_specular({product_data}, method);

        return calc_optical_property(
          *layer, property_choice, side_choice, scattering_choice, theta, phi);
    }

    double calc_optical_property(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
      window_standards::Optical_Standard_Method const & method,
      Calculated_Property_Choice property_choice,
      Side_Choice side_choice,
      Scattering_Choice scattering_choice,
      double theta,
      double phi)
    {
        auto layer = create_multi_pane_specular(product_data, method);

        return calc_optical_property(
          *layer, property_choice, side_choice, scattering_choice, theta, phi);
    }
}   // namespace wincalc
