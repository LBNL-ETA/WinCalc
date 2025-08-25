
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
        LOGMSG("begin do_calc(std::function<T(const FenestrationCommon::Scattering scattering)> "
               "const & f)");
        WCE_Optical_Result_Simple<T> calc_result;
        calc_result.direct_direct = f(FenestrationCommon::Scattering::DirectDirect);
        LOGMSG("before DirectDiffuse");
        calc_result.direct_diffuse = f(FenestrationCommon::Scattering::DirectDiffuse);
        LOGMSG("before DiffuseDiffuse");
        calc_result.diffuse_diffuse = f(FenestrationCommon::Scattering::DiffuseDiffuse);
        LOGMSG("before DirectHemispherical");
        calc_result.direct_hemispherical = f(FenestrationCommon::Scattering::DirectHemispherical);
        LOGMSG("end do_calc(std::function<T(const FenestrationCommon::Scattering scattering)> "
               "const & f)");
        return calc_result;
    }


    template<typename T>
    WCE_Optical_Results_Template<T>
      do_calcs(std::function<T(const FenestrationCommon::PropertySurface prop,
                               const FenestrationCommon::Side side,
                               const FenestrationCommon::Scattering scattering)> const & f)
    {
        LOGMSG("begin do_calcs(prop, side, scattering)");
        WCE_Optical_Results_Template<T> calc_result;

        calc_result.system_results.front.transmittance =
          do_calc<T>([&f](const FenestrationCommon::Scattering scattering) {
              return f(
                FenestrationCommon::PropertySurface::T, FenestrationCommon::Side::Front, scattering);
          });

        LOGMSG("before back transmittance");
        calc_result.system_results.back.transmittance =
          do_calc<T>([&f](const FenestrationCommon::Scattering scattering) {
              return f(
                FenestrationCommon::PropertySurface::T, FenestrationCommon::Side::Back, scattering);
          });

        LOGMSG("before front reflectance");
        calc_result.system_results.front.reflectance =
          do_calc<T>([&f](const FenestrationCommon::Scattering scattering) {
              return f(
                FenestrationCommon::PropertySurface::R, FenestrationCommon::Side::Front, scattering);
          });

        LOGMSG("before back reflectance");
        calc_result.system_results.back.reflectance =
          do_calc<T>([&f](const FenestrationCommon::Scattering scattering) {
              return f(
                FenestrationCommon::PropertySurface::R, FenestrationCommon::Side::Back, scattering);
          });

        LOGMSG("end do_calcs(prop, side, scattering)");
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
        LOGMSG("begin calc_optical_property");
        double result = layers->getPropertySurface(
          min_lambda, max_lambda, property_choice, side_choice, scattering_choice, theta, phi);
        LOGMSG("end calc_optical_property");
        return result;
    }

    std::vector<WCE_Optical_Result_Absorptance<double>>
      get_layer_absorptances(std::shared_ptr<SingleLayerOptics::IScatteringLayer> layers,
                             FenestrationCommon::Side side_choice,
                             double min_lambda,
                             double max_lambda,
                             double theta = 0,
                             double phi = 0)
    {
        LOGMSG("begin get_layer_absorptances");
        std::vector<WCE_Optical_Result_Absorptance<double>> absorptances;
        auto direct_absorptances_total =
          layers->getAbsorptanceLayers(min_lambda,
                                       max_lambda,
                                       side_choice,
                                       FenestrationCommon::ScatteringSimple::Direct,
                                       theta,
                                       phi);
        LOGMSG("before diffuse_absorptances_total");
        auto diffuse_absorptances_total =
          layers->getAbsorptanceLayers(min_lambda,
                                       max_lambda,
                                       side_choice,
                                       FenestrationCommon::ScatteringSimple::Diffuse,
                                       theta,
                                       phi);

        LOGMSG("before direct_absorptances_heat");
        auto direct_absorptances_heat =
          layers->getAbsorptanceLayersHeat(min_lambda,
                                           max_lambda,
                                           side_choice,
                                           FenestrationCommon::ScatteringSimple::Direct,
                                           theta,
                                           phi);
        LOGMSG("before diffuse_absorptances_heat");
        auto diffuse_absorptances_heat =
          layers->getAbsorptanceLayersHeat(min_lambda,
                                           max_lambda,
                                           side_choice,
                                           FenestrationCommon::ScatteringSimple::Diffuse,
                                           theta,
                                           phi);
        LOGMSG("before direct_absorptances_electricity");
        auto direct_absorptances_electricity =
          layers->getAbsorptanceLayersElectricity(min_lambda,
                                                  max_lambda,
                                                  side_choice,
                                                  FenestrationCommon::ScatteringSimple::Direct,
                                                  theta,
                                                  phi);
        LOGMSG("before diffuse_absorptances_electricity");
        auto diffuse_absorptances_electricity =
          layers->getAbsorptanceLayersElectricity(min_lambda,
                                                  max_lambda,
                                                  side_choice,
                                                  FenestrationCommon::ScatteringSimple::Diffuse,
                                                  theta,
                                                  phi);

        LOGMSG("before bsdf_system");
        auto bsdf_system = std::dynamic_pointer_cast<MultiLayerOptics::CMultiPaneBSDF>(layers);

        for(size_t i = 0; i < diffuse_absorptances_heat.size(); ++i)
        {
            LOGMSG("in loop iteration: " + std::to_string(i));
            std::optional<std::vector<double>> angular_total;
            std::optional<std::vector<double>> angular_heat;
            std::optional<std::vector<double>> angular_electricity;

            if(bsdf_system)
            {
                LOGMSG("in if(bsdf_system)");
                angular_total = bsdf_system->Abs(min_lambda, max_lambda, side_choice, i + 1);
                LOGMSG("before angular_heat");
                angular_heat = bsdf_system->AbsHeat(min_lambda, max_lambda, side_choice, i + 1);
                LOGMSG("before angular_electricity");
                angular_electricity =
                  bsdf_system->AbsElectricity(min_lambda, max_lambda, side_choice, i + 1);
            }

            LOGMSG("before absorptances.push_back");
            absorptances.push_back(WCE_Optical_Result_Absorptance<double>{
              direct_absorptances_total[i],
              diffuse_absorptances_total[i],
              direct_absorptances_heat[i],
              diffuse_absorptances_heat[i],
              direct_absorptances_electricity[i],
              diffuse_absorptances_electricity[i],
              angular_total,
              angular_heat,
              angular_electricity,
            });
        }

        LOGMSG("end get_layer_absorptances");
        return absorptances;
    }

    WCE_Optical_Results calc_all(std::shared_ptr<SingleLayerOptics::IScatteringLayer> system,
                                 double min_lambda,
                                 double max_lambda,
                                 double theta,
                                 double phi)
    {
        LOGMSG("begin calc_all(system, " + std::to_string(min_lambda) + ", "
               + std::to_string(max_lambda) + ", " + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        if(max_lambda < min_lambda)
        {
            LOGMSG("in if(max_lambda < min_lambda)");
            max_lambda = min_lambda + 1;
        }
        LOGMSG("before calc_f = ");
        auto calc_f = [=, &system](const FenestrationCommon::PropertySurface prop,
                                   const FenestrationCommon::Side side,
                                   const FenestrationCommon::Scattering scattering) {
            return calc_optical_property(
              system, prop, side, scattering, min_lambda, max_lambda, theta, phi);
        };
        LOGMSG("before optical_results");
        auto optical_results = do_calcs<double>(calc_f);
        LOGMSG("before bsdf_system");
        auto bsdf_system = std::dynamic_pointer_cast<MultiLayerOptics::CMultiPaneBSDF>(system);
        if(bsdf_system)
        {
            LOGMSG("in if(bsdf_system)");
            // Include matrix results for BSDF systems
            optical_results.system_results.front.transmittance.matrix =
              bsdf_system
                ->getMatrix(min_lambda,
                            max_lambda,
                            FenestrationCommon::Side::Front,
                            FenestrationCommon::PropertySurface::T)
                .getMatrix();
            optical_results.system_results.front.transmittance.wavelength_matrices =
              bsdf_system->getWavelengthMatrices(min_lambda,
                                                 max_lambda,
                                                 FenestrationCommon::Side::Front,
                                                 FenestrationCommon::PropertySurface::T);

            LOGMSG("before front.reflectance.matrix");
            optical_results.system_results.front.reflectance.matrix =
              bsdf_system
                ->getMatrix(min_lambda,
                            max_lambda,
                            FenestrationCommon::Side::Front,
                            FenestrationCommon::PropertySurface::R)
                .getMatrix();
            optical_results.system_results.front.reflectance.wavelength_matrices =
              bsdf_system->getWavelengthMatrices(min_lambda,
                                                 max_lambda,
                                                 FenestrationCommon::Side::Front,
                                                 FenestrationCommon::PropertySurface::R);

            LOGMSG("before back.transmittance.matrix");
            optical_results.system_results.back.transmittance.matrix =
              bsdf_system
                ->getMatrix(min_lambda,
                            max_lambda,
                            FenestrationCommon::Side::Back,
                            FenestrationCommon::PropertySurface::T)
                .getMatrix();
            optical_results.system_results.back.transmittance.wavelength_matrices =
              bsdf_system->getWavelengthMatrices(min_lambda,
                                                 max_lambda,
                                                 FenestrationCommon::Side::Back,
                                                 FenestrationCommon::PropertySurface::T);

            LOGMSG("before back.reflectance.matrix");
            optical_results.system_results.back.reflectance.matrix =
              bsdf_system
                ->getMatrix(min_lambda,
                            max_lambda,
                            FenestrationCommon::Side::Back,
                            FenestrationCommon::PropertySurface::R)
                .getMatrix();
            optical_results.system_results.back.reflectance.wavelength_matrices =
              bsdf_system->getWavelengthMatrices(min_lambda,
                                                 max_lambda,
                                                 FenestrationCommon::Side::Back,
                                                 FenestrationCommon::PropertySurface::R);
        }

        LOGMSG("before absorptances_front");
        auto absorptances_front = get_layer_absorptances(
          system, FenestrationCommon::Side::Front, min_lambda, max_lambda, theta, phi);
        LOGMSG("before absorptances_back");
        auto absorptances_back = get_layer_absorptances(
          system, FenestrationCommon::Side::Back, min_lambda, max_lambda, theta, phi);

        for(size_t i = 0; i < absorptances_front.size(); ++i)
        {
            LOGMSG("in absorptances loop iteration: " + std::to_string(i));
            optical_results.layer_results.push_back(
              WCE_Optical_Result_By_Side<WCE_Optical_Result_Layer<double>>{
                WCE_Optical_Result_Absorptance<double>{absorptances_front[i]},
                WCE_Optical_Result_Absorptance<double>{absorptances_back[i]}});
        }

        LOGMSG("end calc_all(system, " + std::to_string(min_lambda) + ", "
               + std::to_string(max_lambda) + ", " + std::to_string(theta) + ", "
               + std::to_string(phi) + ")");
        return optical_results;
    }

    Color_Result
      calc_color_properties(std::shared_ptr<SingleLayerOptics::ColorProperties> color_props,
                            const FenestrationCommon::PropertySurface prop,
                            const FenestrationCommon::Side side,
                            const FenestrationCommon::Scattering scattering,
                            double theta = 0,
                            double phi = 0)
    {
        LOGMSG("begin calc_color_properties");
        auto trichromatic = color_props->getTrichromatic(prop, side, scattering, theta, phi);
        LOGMSG("before getRGB");
        auto rgb = color_props->getRGB(prop, side, scattering, theta, phi);
        LOGMSG("before getCIE_Lab");
        auto lab = color_props->getCIE_Lab(prop, side, scattering, theta, phi);
        LOGMSG("before result = Color_Result");
        auto result = Color_Result(trichromatic, rgb, lab);
        LOGMSG("end calc_color_properties");
        return result;
    }

    WCE_Color_Results calc_color_properties(
      std::shared_ptr<SingleLayerOptics::ColorProperties> color_props, double theta, double phi)
    {
        auto calc_f =
          [=, &color_props](const FenestrationCommon::PropertySurface prop,
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
        LOGMSG("begin calc_color");
        auto layer_x = create_multi_pane(
          product_data, method_x, bsdf_hemisphere, type, number_visible_bands, number_solar_bands);
        // auto layer_y = create_multi_pane(
        //  product_data, method_y, bsdf_hemisphere, type, number_visible_bands,
        //  number_solar_bands);
        // auto layer_z = create_multi_pane(
        //  product_data, method_z, bsdf_hemisphere, type, number_visible_bands,
        //  number_solar_bands);
        LOGMSG("before layer_x->getWavelengths()");
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

        LOGMSG("before get_wavelengths");
        std::vector<std::vector<double>> wavelengths = get_wavelengths(product_data);

        LOGMSG("before get_spectum_values X");
        auto detector_x = get_spectum_values(method_x.detector_spectrum, method_x, wavelengths);
        LOGMSG("before get_spectum_values Y");
        auto detector_y = get_spectum_values(method_y.detector_spectrum, method_y, wavelengths);
        LOGMSG("before get_spectum_values Z");
        auto detector_z = get_spectum_values(method_z.detector_spectrum, method_z, wavelengths);

        FenestrationCommon::CCommonWavelengths wavelength_combiner;
        for(auto & wavelength_set : wavelengths)
        {
            LOGMSG("in for(auto & wavelength_set : wavelengths)");
            wavelength_combiner.addWavelength(wavelength_set);
        }

        LOGMSG("before common_wavelengths");
        auto common_wavelengths =
          wavelength_combiner.getCombinedWavelengths(FenestrationCommon::Combine::Interpolate);


        // All methods must have the same source
        // spectrum? (Should it be checked above?)
        LOGMSG("before source_spectrum");
        auto source_spectrum =
          get_spectum_values(method_x.source_spectrum, method_x, common_wavelengths);

        // and the same wavelength set?
        LOGMSG("before wavelength_set");
        std::vector<double> wavelength_set = combined_layer_wavelength_range_factory(
          {common_wavelengths}, type, number_visible_bands, number_solar_bands);

        LOGMSG("before color_props");
        auto color_props = std::make_shared<SingleLayerOptics::ColorProperties>(
          std::move(layer_x), source_spectrum, detector_x, detector_y, detector_z, wavelength_set);

        LOGMSG("before calc_color_properties");
        auto result = calc_color_properties(color_props, theta, phi);
        LOGMSG("end calc_color");
        return result;
    }

    Layer_Optical_IR_Results_Needed_For_Thermal_Calcs optical_ir_results_needed_for_thermal_calcs(
      Product_Data_Optical_Thermal const & product_data,
      window_standards::Optical_Standard const & standard)
    {
        LOGMSG("begin optical_ir_results_needed_for_thermal_calcs layer");
        auto ir_results = calc_thermal_ir(standard, product_data);

        double tf = ir_results.transmittance_front_diffuse_diffuse;
        double tb = ir_results.transmittance_back_diffuse_diffuse;
        double absorptance_front = ir_results.emissivity_front_hemispheric;
        double absorptance_back = ir_results.emissivity_back_hemispheric;

        auto result = Layer_Optical_IR_Results_Needed_For_Thermal_Calcs{
          tf, tb, absorptance_front, absorptance_back};
        LOGMSG("end optical_ir_results_needed_for_thermal_calcs layer");
        return result;
    }

    std::vector<Layer_Optical_IR_Results_Needed_For_Thermal_Calcs>
      optical_ir_results_needed_for_thermal_calcs(
        std::vector<Product_Data_Optical_Thermal> const & product_data,
        window_standards::Optical_Standard const & standard)
    {
        LOGMSG("begin optical_ir_results_needed_for_thermal_calcs");
        std::vector<Layer_Optical_IR_Results_Needed_For_Thermal_Calcs> result;
        for(auto product : product_data)
        {
            LOGMSG("in for(auto product : product_data)");
            result.push_back(optical_ir_results_needed_for_thermal_calcs(product, standard));
        }
        LOGMSG("end optical_ir_results_needed_for_thermal_calcs layer");
        return result;
    }

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
          layers->getPropertySurface(lambda_range.min_lambda,
                                    lambda_range.max_lambda,
                                    FenestrationCommon::PropertySurface::T,
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
        std::optional<Product_Data_Optical_Thermal> const & non_coplanar_attachment_exterior,
        std::optional<Product_Data_Optical_Thermal> const & non_coplanar_attachment_interior,
        window_standards::Optical_Standard const & standard,
        std::optional<SingleLayerOptics::BSDFHemisphere> bsdf_hemisphere,
        Spectal_Data_Wavelength_Range_Method const & type,
        int number_visible_bands,
        int number_solar_bands)
    {
        LOGMSG("begin optical_solar_results_system_needed_for_thermal_calcs");
        auto optical_layers = get_optical_layers(
          product_data, non_coplanar_attachment_exterior, non_coplanar_attachment_interior);
        LOGMSG("before solar_method");
        auto solar_method = standard.methods.at("SOLAR");
        LOGMSG("before get_wavelengths");
        std::vector<std::vector<double>> wavelengths = get_wavelengths(optical_layers);
        LOGMSG("before create_multi_pane");
        auto result = create_multi_pane(optical_layers,
                                        solar_method,
                                        bsdf_hemisphere,
                                        type,
                                        number_visible_bands,
                                        number_solar_bands);
        LOGMSG("end optical_solar_results_system_needed_for_thermal_calcs");
        return result;
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
        LOGMSG("begin calc_optical_property");
        auto layers = create_multi_pane(
          product_data, method, bsdf_hemisphere, type, number_visible_bands, number_solar_bands);
        LOGMSG("before get_wavelengths");
        std::vector<std::vector<double>> wavelengths = get_wavelengths(product_data);
        LOGMSG("before get_lambda_range");
        auto lambda_range = get_lambda_range(wavelengths, method);
        LOGMSG("before calc_optical_property");
        auto result = calc_optical_property(std::move(layers),
                                            property_choice,
                                            side_choice,
                                            scattering_choice,
                                            lambda_range.min_lambda,
                                            lambda_range.max_lambda,
                                            theta,
                                            phi);
        LOGMSG("end calc_optical_property");
        return result;
    }
}   // namespace wincalc
