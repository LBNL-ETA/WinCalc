
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

template<typename R, typename T>
R do_calcs(std::function<T(const FenestrationCommon::PropertySimple prop,
                           const FenestrationCommon::Side side,
                           const FenestrationCommon::Scattering scattering)> const & f)
{
    R calc_result;
    // direct direct
    calc_result.tf_direct_direct = f(FenestrationCommon::PropertySimple::T,
                                     FenestrationCommon::Side::Front,
                                     FenestrationCommon::Scattering::DirectDirect);
    calc_result.tb_direct_direct = f(FenestrationCommon::PropertySimple::T,
                                     FenestrationCommon::Side::Back,
                                     FenestrationCommon::Scattering::DirectDirect);
    calc_result.rf_direct_direct = f(FenestrationCommon::PropertySimple::R,
                                     FenestrationCommon::Side::Front,
                                     FenestrationCommon::Scattering::DirectDirect);
    calc_result.rb_direct_direct = f(FenestrationCommon::PropertySimple::R,
                                     FenestrationCommon::Side::Back,
                                     FenestrationCommon::Scattering::DirectDirect);

    // direct diffuse
    calc_result.tf_direct_diffuse = f(FenestrationCommon::PropertySimple::T,
                                      FenestrationCommon::Side::Front,
                                      FenestrationCommon::Scattering::DirectDiffuse);
    calc_result.tb_direct_diffuse = f(FenestrationCommon::PropertySimple::T,
                                      FenestrationCommon::Side::Back,
                                      FenestrationCommon::Scattering::DirectDiffuse);
    calc_result.rf_direct_diffuse = f(FenestrationCommon::PropertySimple::R,
                                      FenestrationCommon::Side::Front,
                                      FenestrationCommon::Scattering::DirectDiffuse);
    calc_result.rb_direct_diffuse = f(FenestrationCommon::PropertySimple::R,
                                      FenestrationCommon::Side::Back,
                                      FenestrationCommon::Scattering::DirectDiffuse);

    // diffuse direct.  Not implemented in WCE
    /*
    calc_result.tf_diffuse_direct =
    calc_result.tb_diffuse_direct =
    calc_result.rf_diffuse_direct =
    calc_result.rb_diffuse_direct =
    */

    // diffuse diffuse
    calc_result.tf_diffuse_diffuse = f(FenestrationCommon::PropertySimple::T,
                                       FenestrationCommon::Side::Front,
                                       FenestrationCommon::Scattering::DiffuseDiffuse);
    calc_result.tb_diffuse_diffuse = f(FenestrationCommon::PropertySimple::T,
                                       FenestrationCommon::Side::Back,
                                       FenestrationCommon::Scattering::DiffuseDiffuse);
    calc_result.rf_diffuse_diffuse = f(FenestrationCommon::PropertySimple::R,
                                       FenestrationCommon::Side::Front,
                                       FenestrationCommon::Scattering::DiffuseDiffuse);
    calc_result.rb_diffuse_diffuse = f(FenestrationCommon::PropertySimple::R,
                                       FenestrationCommon::Side::Back,
                                       FenestrationCommon::Scattering::DiffuseDiffuse);

    return calc_result;
}

double calc_optical_property(MultiLayerOptics::CMultiPaneSpecular & layer,
                             Calculated_Property_Choice property_choice,
                             Side_Choice side_choice,
                             Scattering_Choice scattering_choice)
{
    return layer.getPropertySimple(property_choice, side_choice, scattering_choice);
}


WCE_Optical_Result calc_all(OpticsParser::ProductData const & product_data, Method const & method)
{
    auto layer = create_multi_pane_specular({product_data}, method);

    auto calc_f = [&layer](const FenestrationCommon::PropertySimple prop,
                           const FenestrationCommon::Side side,
                           const FenestrationCommon::Scattering scattering) {
        return calc_optical_property(*layer, prop, side, scattering);
    };

    return do_calcs<WCE_Optical_Result, double>(calc_f);
}

WCE_Optical_Result calc_all(std::vector<OpticsParser::ProductData> const & product_data,
                   Method const & method)
{
    auto layer = create_multi_pane_specular(product_data, method);

    auto calc_f = [&layer](const FenestrationCommon::PropertySimple prop,
                           const FenestrationCommon::Side side,
                           const FenestrationCommon::Scattering scattering) {
        return calc_optical_property(*layer, prop, side, scattering);
    };

    return do_calcs<WCE_Optical_Result, double>(calc_f);
}


Color_Result calc_color_properties(std::shared_ptr<SingleLayerOptics::ColorProperties> color_props,
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

WCE_Color_Result
  calc_color_properties(std::shared_ptr<SingleLayerOptics::ColorProperties> color_props)
{
    auto calc_f = [&color_props](const FenestrationCommon::PropertySimple prop,
                                 const FenestrationCommon::Side side,
                                 const FenestrationCommon::Scattering scattering) -> Color_Result {
        return calc_color_properties(color_props, prop, side, scattering);
    };

    return do_calcs<WCE_Color_Result, Color_Result>(calc_f);
}


WCE_Color_Result calc_color(std::vector<OpticsParser::ProductData> const & product_data,
                          Method const & method_x,
                          Method const & method_y,
                          Method const & method_z)
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


    auto detector_x =
      get_spectum_values(method_x.detector_spectrum, method_x.wavelength_set, product_data);
    auto detector_y =
      get_spectum_values(method_y.detector_spectrum, method_y.wavelength_set, product_data);
    auto detector_z =
      get_spectum_values(method_z.detector_spectrum, method_z.wavelength_set, product_data);

    auto source_spectrum =
      get_spectum_values(method_x.detector_spectrum,
                         method_x.wavelength_set,
                         product_data[0]);   // All methods must have the same source
                                          // spectrum? (Should it be checked above?)

    std::vector<double> wavelength_set =
      get_wavelength_set_to_use(method_x, product_data[0]);   // and the same wavelength set?

    std::shared_ptr<SingleLayerOptics::ColorProperties> color_props =
      std::make_shared<SingleLayerOptics::ColorProperties>(std::move(layer_x),
                                                           std::move(layer_y),
                                                           std::move(layer_z),
                                                           *source_spectrum,
                                                           *detector_x,
                                                           *detector_y,
                                                           *detector_z,
                                                           wavelength_set);


    return calc_color_properties(color_props);
}



double calc_optical_property(OpticsParser::ProductData const & product_data,
                             Method const & method,
                             Calculated_Property_Choice property_choice,
                             Side_Choice side_choice,
                             Scattering_Choice scattering_choice)
{
    auto layer = create_multi_pane_specular({product_data}, method);
    
    return calc_optical_property(*layer, property_choice, side_choice, scattering_choice);
}

double calc_optical_property(std::vector<OpticsParser::ProductData> const & product_data,
                             Method const & method,
                             Calculated_Property_Choice property_choice,
                             Side_Choice side_choice,
                             Scattering_Choice scattering_choice)
{
	auto layer = create_multi_pane_specular(product_data, method);

    return calc_optical_property(*layer, property_choice, side_choice, scattering_choice);
}

