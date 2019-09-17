
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

#if 0
	template<>
    WCE_Optical_Result_By_Side<Color_Result>
      do_calc(std::function<Color_Result(const FenestrationCommon::PropertySimple prop,
                              const FenestrationCommon::Side side)> const & f)
    {
        WCE_Optical_Result_By_Side<T> calc_result;
        calc_result.tf = f(FenestrationCommon::PropertySimple::T, FenestrationCommon::Side::Front);
        calc_result.tb = f(FenestrationCommon::PropertySimple::T, FenestrationCommon::Side::Back);
        calc_result.rf = f(FenestrationCommon::PropertySimple::R, FenestrationCommon::Side::Front);
        calc_result.rb = f(FenestrationCommon::PropertySimple::R, FenestrationCommon::Side::Back);

        return calc_result;
    }
#endif


    template<typename T>
    WCE_Optical_Result_By_Side<T>
      do_calcs(std::function<T(const FenestrationCommon::PropertySimple prop,
                               const FenestrationCommon::Side side,
                               const FenestrationCommon::Scattering scattering)> const & f)
    {
        WCE_Optical_Result_By_Side<T> calc_result;
#if 0
        calc_result.direct_direct = do_calc<T>(
          [&f](const FenestrationCommon::PropertySimple p, const FenestrationCommon::Side s) {
              return f(p, s, FenestrationCommon::Scattering::DirectDirect);
          });

        calc_result.direct_diffuse = do_calc<T>(
          [&f](const FenestrationCommon::PropertySimple p, const FenestrationCommon::Side s) {
              return f(p, s, FenestrationCommon::Scattering::DirectDiffuse);
          });

        calc_result.diffuse_diffuse = do_calc<T>(
          [&f](const FenestrationCommon::PropertySimple p, const FenestrationCommon::Side s) {
              return f(p, s, FenestrationCommon::Scattering::DiffuseDiffuse);
          });
#endif
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

#if 0
    WCE_Simple_Result calc_all(OpticsParser::ProductData const & product_data,
                               window_standards::Optical_Standard_Method const & method)
    {
        auto layer = create_multi_pane_specular({product_data}, method);

        auto calc_f = [&layer](const FenestrationCommon::PropertySimple prop,
                               const FenestrationCommon::Side side,
                               const FenestrationCommon::Scattering scattering) {
            return calc_optical_property(*layer, prop, side, scattering);
        };

        return do_calcs<double>(calc_f);
    }
#endif
    WCE_Optical_Results
      calc_all(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
                               window_standards::Optical_Standard_Method const & method)
    {
        auto layer = create_multi_pane_specular({product_data}, method);

        auto calc_f = [&layer](const FenestrationCommon::PropertySimple prop,
                               const FenestrationCommon::Side side,
                               const FenestrationCommon::Scattering scattering) {
            return calc_optical_property(*layer, prop, side, scattering);
        };

        return do_calcs<double>(calc_f);
    }


    WCE_Optical_Results
      calc_all(std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
               window_standards::Optical_Standard_Method const & method)
    {
        auto layer = create_multi_pane_specular(product_data, method);

        auto calc_f = [&layer](const FenestrationCommon::PropertySimple prop,
                               const FenestrationCommon::Side side,
                               const FenestrationCommon::Scattering scattering) {
            return calc_optical_property(*layer, prop, side, scattering);
        };

        return do_calcs<double>(calc_f);
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
