
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
                                 Scattering_Choice scattering_choice)
    {
        return layer.getPropertySimple(property_choice, side_choice, scattering_choice);
    }

    WCE_Optical_Result_Absorptance<double>
      get_layer_absorptances(MultiLayerOptics::CMultiPaneSpecular & layers,
                             FenestrationCommon::Side side_choice,
                             double theta = 0,
                             double phi = 0)
    {
        WCE_Optical_Result_Absorptance<double> absorptances;
        absorptances.direct = layers.getAbsorptanceLayers(
          side_choice, FenestrationCommon::ScatteringSimple::Direct, theta, phi);
        absorptances.diffuse = layers.getAbsorptanceLayers(
          side_choice, FenestrationCommon::ScatteringSimple::Diffuse, theta, phi);

        return absorptances;
    }

    WCE_Optical_Results calc_all(MultiLayerOptics::CMultiPaneSpecular & layers)
    {
        auto calc_f = [&layers](const FenestrationCommon::PropertySimple prop,
                                const FenestrationCommon::Side side,
                                const FenestrationCommon::Scattering scattering) {
            return calc_optical_property(layers, prop, side, scattering);
        };

        auto optical_results = do_calcs<double>(calc_f);
        optical_results.absorptances_front =
          get_layer_absorptances(layers, FenestrationCommon::Side::Front);
        optical_results.absorptances_back =
          get_layer_absorptances(layers, FenestrationCommon::Side::Back);
        return optical_results;
    }


    WCE_Optical_Results
      calc_all(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
               window_standards::Optical_Standard_Method const & method)
    {
        auto layers = create_multi_pane_specular({product_data}, method);

        return calc_all(*layers);
    }


    WCE_Optical_Results
      calc_all(std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
               window_standards::Optical_Standard_Method const & method)
    {
        auto layers = create_multi_pane_specular(product_data, method);

        return calc_all(*layers);
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

    WCE_Color_Results
      calc_color_properties(std::shared_ptr<SingleLayerOptics::ColorProperties> color_props)
    {
        auto calc_f =
          [&color_props](const FenestrationCommon::PropertySimple prop,
                         const FenestrationCommon::Side side,
                         const FenestrationCommon::Scattering scattering) -> Color_Result {
            return calc_color_properties(color_props, prop, side, scattering);
        };

        return do_calcs<Color_Result>(calc_f);
    }


    WCE_Color_Results
      calc_color(std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
                 window_standards::Optical_Standard_Method const & method_x,
                 window_standards::Optical_Standard_Method const & method_y,
                 window_standards::Optical_Standard_Method const & method_z)
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


        return calc_color_properties(color_props);
    }


    double
      calc_optical_property(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
                            window_standards::Optical_Standard_Method const & method,
                            Calculated_Property_Choice property_choice,
                            Side_Choice side_choice,
                            Scattering_Choice scattering_choice)
    {
        auto layer = create_multi_pane_specular({product_data}, method);

        return calc_optical_property(*layer, property_choice, side_choice, scattering_choice);
    }

    double calc_optical_property(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
      window_standards::Optical_Standard_Method const & method,
      Calculated_Property_Choice property_choice,
      Side_Choice side_choice,
      Scattering_Choice scattering_choice)
    {
        auto layer = create_multi_pane_specular(product_data, method);

        return calc_optical_property(*layer, property_choice, side_choice, scattering_choice);
    }
}   // namespace wincalc
