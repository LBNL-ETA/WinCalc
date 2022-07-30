#include "create_wce_objects.h"
#include <sstream>
#include "convert_optics_parser.h"
#include "optical_calcs.h"
#include "util.h"
#include "thermal_ir.h"


namespace wincalc
{
    std::shared_ptr<Tarcog::ISO15099::CIndoorEnvironment>
      create_indoor_environment(Environment const & environment)
    {
        std::shared_ptr<Tarcog::ISO15099::CIndoorEnvironment> indoor =
          Tarcog::ISO15099::Environments::indoor(environment.air_temperature, environment.pressure);

        indoor->setHCoeffModel(environment.coefficient_model, environment.convection_coefficient);

        return indoor;
    }

    std::shared_ptr<Tarcog::ISO15099::COutdoorEnvironment>
      create_outdoor_environment(Environment const & environment)
    {
        std::shared_ptr<Tarcog::ISO15099::COutdoorEnvironment> outdoor =
          Tarcog::ISO15099::Environments::outdoor(environment.air_temperature,
                                                  environment.air_speed,
                                                  environment.direct_solar_radiation,
                                                  environment.radiation_temperature,
                                                  Tarcog::ISO15099::SkyModel::AllSpecified,
                                                  environment.pressure);

        outdoor->setHCoeffModel(environment.coefficient_model, environment.convection_coefficient);
        return outdoor;
    }


    SpectralAveraging::MeasuredRow convert(OpticsParser::WLData const & data)
    {
        if(!data.directComponent.has_value())
        {
            throw std::runtime_error("Missing wavelength direct component");
        }
        SpectralAveraging::MeasuredRow converted(data.wavelength,
                                                 data.directComponent.value().tf,
                                                 data.directComponent.value().rf,
                                                 data.directComponent.value().rb);
        return converted;
    }

    std::vector<SpectralAveraging::MeasuredRow>
      convert(std::vector<OpticsParser::WLData> const & data)
    {
        std::vector<SpectralAveraging::MeasuredRow> converted;

        for(auto const & row : data)
        {
            if(row.directComponent.has_value())
            {
                converted.push_back(convert(row));
            }
        }
        return converted;
    }

    FenestrationCommon::CSeries convert(std::vector<std::pair<double, double>> const & v)
    {
        FenestrationCommon::CSeries series;
        for(auto val : v)
        {
            series.addProperty(val.first, val.second);
        }
        return series;
    }

    std::shared_ptr<SpectralAveraging::CSpectralSampleData>
      convert(std::vector<std::tuple<double, double, double, double>> const & measured_data)
    {
        std::shared_ptr<SpectralAveraging::CSpectralSampleData> spectral_data =
          std::make_shared<SpectralAveraging::CSpectralSampleData>();

        for(auto val : measured_data)
        {
            spectral_data->addRecord(
              std::get<0>(val), std::get<1>(val), std::get<2>(val), std::get<3>(val));
        }

        return spectral_data;
    }

    FenestrationCommon::IntegrationType
      convert(window_standards::Integration_Rule_Type integration_rule_type)
    {
        std::map<window_standards::Integration_Rule_Type, FenestrationCommon::IntegrationType>
          integration_types;
        integration_types[window_standards::Integration_Rule_Type::RECTANGULAR] =
          FenestrationCommon::IntegrationType::Rectangular;
        integration_types[window_standards::Integration_Rule_Type::TRAPEZOIDAL] =
          FenestrationCommon::IntegrationType::Trapezoidal;

        return integration_types[integration_rule_type];
    }


    double get_minimum_wavelength(window_standards::Optical_Standard_Method const & method,
                                  double product_data_min_wavelength,
                                  FenestrationCommon::CSeries const & source_spectrum)
    {
        double result = std::numeric_limits<double>::quiet_NaN();

        if(method.min_wavelength.type == window_standards::Wavelength_Boundary_Type::WAVELENGTH_SET)
        {
            if(method.wavelength_set.type == window_standards::Wavelength_Set_Type::FILE)
            {
                result = method.wavelength_set.values[0];
            }
            else if(method.wavelength_set.type == window_standards::Wavelength_Set_Type::SOURCE)
            {
                result = source_spectrum.getXArray().front();
            }
            if(method.wavelength_set.type == window_standards::Wavelength_Set_Type::DATA)
            {
                result = product_data_min_wavelength;
            }
        }
        else if(method.min_wavelength.type == window_standards::Wavelength_Boundary_Type::NUMBER)
        {
            result = method.min_wavelength.value;
        }

        return result;
    }


    double get_maximum_wavelength(window_standards::Optical_Standard_Method const & method,
                                  double product_data_max_wavelength,
                                  FenestrationCommon::CSeries const & source_spectrum)
    {
        double result = std::numeric_limits<double>::quiet_NaN();

        if(method.max_wavelength.type == window_standards::Wavelength_Boundary_Type::WAVELENGTH_SET)
        {
            if(method.wavelength_set.type == window_standards::Wavelength_Set_Type::FILE)
            {
                result = method.wavelength_set.values.back();
            }
            else if(method.wavelength_set.type == window_standards::Wavelength_Set_Type::SOURCE)
            {
                result = source_spectrum.getXArray().back();
            }
            if(method.wavelength_set.type == window_standards::Wavelength_Set_Type::DATA)
            {
                result = product_data_max_wavelength;
            }
        }
        else if(method.max_wavelength.type == window_standards::Wavelength_Boundary_Type::NUMBER)
        {
            result = method.max_wavelength.value;
        }

        return result;
    }


    FenestrationCommon::CSeries
      get_spectum_values(window_standards::Spectrum const & spectrum,
                         window_standards::Optical_Standard_Method const & method,
                         std::vector<double> const & product_wavelengths)
    {
        FenestrationCommon::CSeries result;
        switch(spectrum.type)
        {
            case window_standards::Spectrum_Type::UV_ACTION:
                switch(method.wavelength_set.type)
                {
                    case window_standards::Wavelength_Set_Type::DATA:
                        // Wavelengths come from the measured data.  Extract first column and pass
                        // those
                        result = convert(
                          SpectralAveraging::UVAction(product_wavelengths, spectrum.a, spectrum.b));
                        break;
                    case window_standards::Wavelength_Set_Type::SOURCE:
                        // Wavelengths come from the source spectrum.  Extract first column and pass
                        // those
                        result = convert(SpectralAveraging::UVAction(
                          get_first_val(method.source_spectrum.values), spectrum.a, spectrum.b));
                        break;
                    case window_standards::Wavelength_Set_Type::FILE:
                        // Wavelengths should already be loaded into the wavelength_set
                        result = convert(SpectralAveraging::UVAction(
                          method.wavelength_set.values, spectrum.a, spectrum.b));
                        break;
                }
                break;
            case window_standards::Spectrum_Type::KROCHMANN:
                switch(method.wavelength_set.type)
                {
                    case window_standards::Wavelength_Set_Type::DATA:
                        // Wavelengths come from the measured data.  Extract first column and pass
                        // those
                        result = convert(SpectralAveraging::Krochmann(product_wavelengths));
                        break;
                    case window_standards::Wavelength_Set_Type::SOURCE:
                        // Wavelengths come from the source spectrum.  Extract first column and pass
                        // those
                        result = convert(SpectralAveraging::Krochmann(
                          get_first_val(method.source_spectrum.values)));
                        break;
                    case window_standards::Wavelength_Set_Type::FILE:
                        // Wavelengths should already be loaded into the wavelength_set
                        result =
                          convert(SpectralAveraging::Krochmann(method.wavelength_set.values));
                        break;
                }
                break;
            case window_standards::Spectrum_Type::BLACKBODY:
                switch(method.wavelength_set.type)
                {
                    case window_standards::Wavelength_Set_Type::DATA:
                        // Wavelengths come from the measured data.  Extract first column and pass
                        // those
                        result = convert(
                          SpectralAveraging::BlackBodySpectrum(product_wavelengths, spectrum.t));
                        break;
                    case window_standards::Wavelength_Set_Type::SOURCE:
                        // Wavelengths come from the source spectrum.  Extract first column and pass
                        // those
                        result = convert(SpectralAveraging::BlackBodySpectrum(
                          get_first_val(method.source_spectrum.values), spectrum.t));
                        break;
                    case window_standards::Wavelength_Set_Type::FILE:
                        // Wavelengths should already be loaded into the wavelength_set
                        result = convert(SpectralAveraging::BlackBodySpectrum(
                          method.wavelength_set.values, spectrum.t));
                        break;
                }
                break;
            case window_standards::Spectrum_Type::FILE:
                result = convert(spectrum.values);
                break;
            case window_standards::Spectrum_Type::NONE:
                // if spectrum is none just use empty CSeries
                break;
            default:
                throw std::runtime_error("Unknown spectrum type.");
                break;
        }

        return result;
    }

    FenestrationCommon::CSeries
      get_spectum_values(window_standards::Spectrum const & spectrum,
                         window_standards::Optical_Standard_Method const & method,
                         std::vector<std::vector<double>> const & products_wavelengths)
    {
        FenestrationCommon::CCommonWavelengths wavelength_combiner;
        for(auto & product_wavelengths : products_wavelengths)
        {
            wavelength_combiner.addWavelength(product_wavelengths);
        }
        auto common_wavelengths =
          wavelength_combiner.getCombinedWavelengths(FenestrationCommon::Combine::Interpolate);
        return get_spectum_values(spectrum, method, common_wavelengths);
    }

    std::vector<double>
      get_wavelength_set_to_use(window_standards::Optical_Standard_Method const & method,
                                std::vector<double> const & product_wavelengths)
    {
        std::vector<double> result;

        // if there is anything in wavelength_set give that preference
        if(!method.wavelength_set.values.empty())
        {
            result = method.wavelength_set.values;
        }
        else
        {
            if(method.wavelength_set.type == window_standards::Wavelength_Set_Type::SOURCE)
            {
                if(method.source_spectrum.values.empty())
                {
                    throw std::runtime_error("Wavelength set to source but no source data loaded");
                }
                // Get the wavelengths from the source curve
                result = get_first_val(method.source_spectrum.values);
            }
            else if(method.wavelength_set.type == window_standards::Wavelength_Set_Type::DATA)
            {
                result = product_wavelengths;
            }
        }

        return result;
    }

    std::vector<double>
      wavelength_range_factory(std::vector<double> const & product_wavelengths,
                               window_standards::Optical_Standard_Method const & method,
                               Spectal_Data_Wavelength_Range_Method const & type,
                               int number_visible_bands,
                               int number_solar_bands)
    {
        if(type == Spectal_Data_Wavelength_Range_Method::CONDENSED)
        {
            return FenestrationCommon::generateSpectrum(number_visible_bands, number_solar_bands);
        }
        else if(type == Spectal_Data_Wavelength_Range_Method::ISO_9050)
        {
            return FenestrationCommon::generateISO9050Wavelengths();
        }
        else if(type == Spectal_Data_Wavelength_Range_Method::FULL)
        {
            return get_wavelength_set_to_use(method, product_wavelengths);
        }
        else
        {
            throw std::runtime_error("Unknown wavelength method.");
        }
    }

    std::shared_ptr<SingleLayerOptics::CMaterial>
      create_material(wincalc::Product_Data_Dual_Band_Optical_Hemispheric const & product,
                      window_standards::Optical_Standard_Method const & method,
                      Spectal_Data_Wavelength_Range_Method const & type,
                      int number_visible_bands,
                      int number_solar_bands)
    {
        auto wavelength_set = wavelength_range_factory(
          product.wavelengths(), method, type, number_visible_bands, number_solar_bands);

        std::shared_ptr<SingleLayerOptics::CMaterial> material =
          SingleLayerOptics::Material::dualBandMaterial(product.tf_solar,
                                                        product.tb_solar,
                                                        product.rf_solar,
                                                        product.rb_solar,
                                                        product.tf_visible,
                                                        product.tb_visible,
                                                        product.rf_visible,
                                                        product.rb_visible);

        material->setBandWavelengths(wavelength_set);
        return material;
        // throw std::runtime_error("Dual band specular materials not yet supported.");
    }

    std::shared_ptr<SingleLayerOptics::CMaterial>
      create_material(wincalc::Product_Data_Dual_Band_Optical_BSDF const & product,
                      window_standards::Optical_Standard_Method const & optical_method,
                      size_t number_of_layers,
                      Spectal_Data_Wavelength_Range_Method const & type,
                      int number_visible_bands,
                      int number_solar_bands)
    {
        auto wavelength_set = wavelength_range_factory(
          product.wavelengths(), optical_method, type, number_visible_bands, number_solar_bands);
        std::shared_ptr<SingleLayerOptics::CMaterial> material;
        if(number_of_layers == 1 && to_lower(optical_method.name) == "solar")
        {
            material = SingleLayerOptics::Material::singleBandBSDFMaterial(
              product.tf_solar,
              product.tb_solar,
              product.rf_solar,
              product.rb_solar,
              product.bsdf_hemisphere,
              FenestrationCommon::WavelengthRange::Solar);
        }
        else if(number_of_layers == 1 && to_lower(optical_method.name) == "photopic")
        {
            material = SingleLayerOptics::Material::singleBandBSDFMaterial(
              product.tf_visible,
              product.tb_visible,
              product.rf_visible,
              product.rb_visible,
              product.bsdf_hemisphere,
              FenestrationCommon::WavelengthRange::Visible);
        }
        else
        {
            material =
              SingleLayerOptics::Material::dualBandBSDFMaterial(product.tf_solar,
                                                                product.tb_solar,
                                                                product.rf_solar,
                                                                product.rb_solar,
                                                                product.tf_visible,
                                                                product.tb_visible,
                                                                product.rf_visible,
                                                                product.rb_visible,
                                                                product.bsdf_hemisphere,
                                                                0.49);   // TODO, replace 0.49 ratio
        }
        material->setBandWavelengths(wavelength_set);
        return material;
    }


    Lambda_Range get_lambda_range(std::vector<std::vector<double>> const & products_wavelengths,
                                  window_standards::Optical_Standard_Method const & method)
    {
        auto source_spectrum =
          get_spectum_values(method.source_spectrum, method, products_wavelengths);
        std::vector<double> min_wavelengths;
        std::vector<double> max_wavelengths;
        for(auto product_wavelengths : products_wavelengths)
        {
            min_wavelengths.push_back(
              get_minimum_wavelength(method, product_wavelengths[0], source_spectrum));
            max_wavelengths.push_back(
              get_maximum_wavelength(method, product_wavelengths.back(), source_spectrum));
        }
        // The min and max lambda should be the tighest boundary not the loosest
        //  So it should be the largest minimum and smallest maximum
        double min_wavelength = *std::max_element(min_wavelengths.begin(), min_wavelengths.end());
        double max_wavelength = *std::min_element(max_wavelengths.begin(), max_wavelengths.end());
        return Lambda_Range{min_wavelength, max_wavelength};
    }

    FenestrationCommon::CSeries get_eqef(std::vector<OpticsParser::WLData> const & data)
    {
        FenestrationCommon::CSeries res;
        for(auto & row : data)
        {
            if(row.pvComponent.has_value())
            {
                res.addProperty(row.wavelength, row.pvComponent.value().eqef);
            }
        }
        return res;
    }

    FenestrationCommon::CSeries get_eqeb(std::vector<OpticsParser::WLData> const & data)
    {
        FenestrationCommon::CSeries res;
        for(auto & row : data)
        {
            if(row.pvComponent.has_value())
            {
                res.addProperty(row.wavelength, row.pvComponent.value().eqeb);
            }
        }
        return res;
    }

    std::shared_ptr<SingleLayerOptics::CMaterial>
      create_material(wincalc::Product_Data_N_Band_Optical const & product_data,
                      window_standards::Optical_Standard_Method const & method,
                      Spectal_Data_Wavelength_Range_Method const & type,
                      int number_visible_bands,
                      int number_solar_bands)
    {
        auto wavelength_set = wavelength_range_factory(
          product_data.wavelengths(), method, type, number_visible_bands, number_solar_bands);

        std::shared_ptr<std::vector<double>> converted_wavelengths =
          std::make_shared<std::vector<double>>(wavelength_set);

        auto integration_rule = convert(method.integration_rule.type);

        auto measured_wavelength_data = convert(product_data.wavelength_data);
        auto spectral_sample_data =
          SpectralAveraging::CSpectralSampleData::create(measured_wavelength_data);

        auto lambda_range = get_lambda_range({product_data.wavelengths()}, method);

        std::shared_ptr<SingleLayerOptics::CMaterial> material =
          SingleLayerOptics::Material::nBandMaterial(spectral_sample_data,
                                                     product_data.thickness_meters,
                                                     product_data.material_type,
                                                     lambda_range.min_lambda,
                                                     lambda_range.max_lambda,
                                                     integration_rule,
                                                     method.integration_rule.k);

        material->setBandWavelengths(wavelength_set);
        return material;
    }

    std::shared_ptr<SingleLayerOptics::CMaterial>
      create_pv_material(wincalc::Product_Data_N_Band_Optical const & product_data,
                         window_standards::Optical_Standard_Method const & method,
                         Spectal_Data_Wavelength_Range_Method const & type,
                         int number_visible_bands,
                         int number_solar_bands)
    {
        auto wavelength_set = wavelength_range_factory(
          product_data.wavelengths(), method, type, number_visible_bands, number_solar_bands);

        std::shared_ptr<std::vector<double>> converted_wavelengths =
          std::make_shared<std::vector<double>>(wavelength_set);

        auto integration_rule = convert(method.integration_rule.type);

        auto measured_wavelength_data = convert(product_data.wavelength_data);
        auto spectral_sample_data =
          SpectralAveraging::CSpectralSampleData::create(measured_wavelength_data);

        auto lambda_range = get_lambda_range({product_data.wavelengths()}, method);

        auto eqef = get_eqef(product_data.wavelength_data);
        auto eqeb = get_eqeb(product_data.wavelength_data);

        auto pvSample = std::make_shared<SpectralAveraging::PhotovoltaicSampleData>(
          measured_wavelength_data, eqef, eqeb);

        auto material =
          SingleLayerOptics::Material::nBandPhotovoltaicMaterial(pvSample,
                                                                 product_data.thickness_meters,
                                                                 product_data.material_type,
                                                                 lambda_range.min_lambda,
                                                                 lambda_range.max_lambda,
                                                                 integration_rule,
                                                                 method.integration_rule.k);

        material->setBandWavelengths(wavelength_set);
        return material;
    }

    std::shared_ptr<SingleLayerOptics::CMaterial>
      create_material(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
                      window_standards::Optical_Standard_Method const & method,
                      size_t number_of_layers,
                      Spectal_Data_Wavelength_Range_Method const & type,
                      int number_visible_bands,
                      int number_solar_bands)
    {
        std::shared_ptr<SingleLayerOptics::CMaterial> material;
        auto wavelengths = product_data->wavelengths();
        double material_min_wavelength = wavelengths.front();
        double material_max_wavelength = wavelengths.back();
        auto source_spectrum = get_spectum_values(method.source_spectrum, method, wavelengths);
        auto min_wavelength =
          get_minimum_wavelength(method, material_min_wavelength, source_spectrum);
        auto max_wavelength =
          get_maximum_wavelength(method, material_max_wavelength, source_spectrum);
        // Need to check max wavelength > min wavelength because the way methods are setup it is
        // possible to have min wavelength > max wavelength based on definitions.
        // e.g. the NFRC Thermal IR standard has min wavelength: 5 and max wavelength:
        // wavelength set and wavelength set: Data.  So if the data only goes up to 2.5 then
        // based on the definitions max wavelength = 5 and min wavelength = 2.5
        if(max_wavelength > min_wavelength
           && material_min_wavelength <= (min_wavelength + ConstantsData::wavelengthErrorTolerance)
           && (material_max_wavelength + ConstantsData::wavelengthErrorTolerance) >= max_wavelength)
        {
            // has the required wavelength ranges to calculate from measured values, priortize
            // this case

            if(std::dynamic_pointer_cast<Product_Data_N_Band_Optical>(product_data))
            {
                material = create_material(
                  *std::dynamic_pointer_cast<Product_Data_N_Band_Optical>(product_data),
                  method,
                  type,
                  number_visible_bands,
                  number_solar_bands);
            }
            else if(std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_BSDF>(
                      product_data))
            {
                material = create_material(
                  *std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_BSDF>(
                    product_data),
                  method,
                  number_of_layers,
                  type,
                  number_visible_bands,
                  number_solar_bands);
            }
            else if(std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_Hemispheric>(
                      product_data))
            {
                material = create_material(
                  *std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_Hemispheric>(
                    product_data),
                  method,
                  type,
                  number_visible_bands,
                  number_solar_bands);
            }
            else
            {
                throw std::runtime_error("Unsupported optical data format");
            }
        }
        else
        {
            if(method.name == "THERMAL IR")
            {
                // Thermal IR is a special case where we calculate values even if the individual
                // measured wavelength values does not extend into the wavelength range defined
                // in the standard method.  To do the calculation in that case a single band
                // material is created NOTE:  Since the method defines the max wavelength as the
                // max measured value
                // this will be less than the minimum wavelength as defined in the method.
                // Since this is a single band material it doesn't matter what the max lambda
                // is so long as it is greater than min lambda.  So define max_lambda =
                // min_lambda + 1

                if(!product_data->ir_transmittance_front.has_value())
                {
                    throw std::runtime_error("Missing IR transmittance front");
                }
                if(!product_data->ir_transmittance_back.has_value())
                {
                    throw std::runtime_error("Missing IR transmittance back");
                }
                double tf = product_data->ir_transmittance_front.value();
                double tb = product_data->ir_transmittance_back.value();
                if(!product_data->emissivity_front.has_value())
                {
                    throw std::runtime_error("Missing emissivity front");
                }
                if(!product_data->emissivity_back.has_value())
                {
                    throw std::runtime_error("Missing emissivity back");
                }
                double rf = 1.0 - tf - product_data->emissivity_front.value();
                double rb = 1.0 - tb - product_data->emissivity_back.value();
                material = SingleLayerOptics::Material::singleBandMaterial(
                  tf, tb, rf, rb, FenestrationCommon::WavelengthRange::IR);
            }
            else
            {
                std::stringstream msg;
                msg << "Product without measured data for entire wavelength range in method: "
                    << method.name;
                throw std::runtime_error(msg.str());
            }
        }

        return material;
    }

    std::shared_ptr<SingleLayerOptics::CMaterial>
      create_pv_material(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
                         window_standards::Optical_Standard_Method const & method,
                         Spectal_Data_Wavelength_Range_Method const & type,
                         int number_visible_bands,
                         int number_solar_bands)
    {
        std::shared_ptr<SingleLayerOptics::CMaterial> material;
        auto wavelengths = product_data->wavelengths();
        double material_min_wavelength = wavelengths.front();
        double material_max_wavelength = wavelengths.back();
        auto source_spectrum = get_spectum_values(method.source_spectrum, method, wavelengths);
        auto min_wavelength =
          get_minimum_wavelength(method, material_min_wavelength, source_spectrum);
        auto max_wavelength =
          get_maximum_wavelength(method, material_max_wavelength, source_spectrum);
        // Need to check max wavelength > min wavelength because the way methods are setup it is
        // possible to have min wavelength > max wavelength based on definitions.
        // e.g. the NFRC Thermal IR standard has min wavelength: 5 and max wavelength:
        // wavelength set and wavelength set: Data.  So if the data only goes up to 2.5 then
        // based on the definitions max wavelength = 5 and min wavelength = 2.5
        if(max_wavelength > min_wavelength
           && material_min_wavelength <= (min_wavelength + ConstantsData::wavelengthErrorTolerance)
           && (material_max_wavelength + ConstantsData::wavelengthErrorTolerance) >= max_wavelength)
        {
            // has the required wavelength ranges to calculate from measured values, priortize
            // this case

            if(std::dynamic_pointer_cast<Product_Data_N_Band_Optical>(product_data))
            {
                material = create_pv_material(
                  *std::dynamic_pointer_cast<Product_Data_N_Band_Optical>(product_data),
                  method,
                  type,
                  number_visible_bands,
                  number_solar_bands);
            }
            else if(std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_BSDF>(
                      product_data))
            {
                throw std::runtime_error("Dual band BSDF material does not yet support PV");
            }
            else if(std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_Hemispheric>(
                      product_data))
            {
                throw std::runtime_error("Dual band hemispheric material does not yet support PV");
            }
            else
            {
                throw std::runtime_error("Unsupported optical data format");
            }
        }
        else
        {
            if(method.name == "THERMAL IR")
            {
#if 1
                // Thermal IR is a special case where we calculate values even if the individual
                // measured wavelength values does not extend into the wavelength range defined
                // in the standard method.  To do the calculation in that case a single band
                // material is created NOTE:  Since the method defines the max wavelength as the
                // max measured value
                // this will be less than the minimum wavelength as defined in the method.
                // Since this is a single band material it doesn't matter what the max lambda
                // is so long as it is greater than min lambda.  So define max_lambda =
                // min_lambda + 1
                if(!product_data->ir_transmittance_front.has_value())
                {
                    throw std::runtime_error("Missing IR transmittance front");
                }
                if(!product_data->ir_transmittance_back.has_value())
                {
                    throw std::runtime_error("Missing IR transmittance back");
                }
                double tf = product_data->ir_transmittance_front.value();
                double tb = product_data->ir_transmittance_back.value();
                if(!product_data->emissivity_front.has_value())
                {
                    throw std::runtime_error("Missing emissivity front");
                }
                if(!product_data->emissivity_back.has_value())
                {
                    throw std::runtime_error("Missing emissivity back");
                }
                double rf = 1.0 - tf - product_data->emissivity_front.value();
                double rb = 1.0 - tb - product_data->emissivity_back.value();
                material = SingleLayerOptics::Material::singleBandMaterial(
                  tf, tb, rf, rb, FenestrationCommon::WavelengthRange::IR);
#endif
            }
            else
            {
                std::stringstream msg;
                msg << "Product without measured data for entire wavelength range in method: "
                    << method.name;
                throw std::runtime_error(msg.str());
            }
        }

        return material;
    }

    SingleLayerOptics::PVPowerPropertiesTable
      get_power_properties(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data)
    {
        // Currently there is only one temperature that power properties are available at
        // so use that.  In the future if there are more values and interpolation is needed this
        // will need to change.  But a lot more will need to change because that will introduce
        // a temperature dependence into optical calculations.  And that means iterations between
        // optical and thermal calculations to get results.
        std::vector<SingleLayerOptics::PVPowerProperties> properties;

        if(product_data->pv_power_properties.has_value())
        {
            auto default_power_table = product_data->pv_power_properties.value().begin();
            for(auto & power_property : default_power_table->second)
            {
                properties.emplace_back(power_property.jsc, power_property.voc, power_property.ff);
            }
        }
        SingleLayerOptics::PVPowerPropertiesTable table(properties);
        return table;
    }

    bool is_pv(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data)
    {
        return product_data->pv_power_properties.has_value();
    }

    std::shared_ptr<SingleLayerOptics::SpecularLayer>
      create_specular_layer(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
                            window_standards::Optical_Standard_Method const & method,
                            size_t number_of_layers,
                            Spectal_Data_Wavelength_Range_Method const & type,
                            int number_visible_bands,
                            int number_solar_bands)
    {
        std::shared_ptr<SingleLayerOptics::SpecularLayer> specular_layer;
        if(is_pv(product_data))
        {
            auto material = create_pv_material(
              product_data, method, type, number_visible_bands, number_solar_bands);
            auto power_properties = get_power_properties(product_data);
            specular_layer =
              SingleLayerOptics::PhotovoltaicSpecularLayer::createLayer(material, power_properties);
        }
        else
        {
            auto material = create_material(product_data,
                                            method,
                                            number_of_layers,
                                            type,
                                            number_visible_bands,
                                            number_solar_bands);
            specular_layer = SingleLayerOptics::SpecularLayer::createLayer(material);
        }
        specular_layer->Flipped(product_data->flipped);
        return specular_layer;
    }


    std::unique_ptr<MultiLayerOptics::CMultiPaneSpecular> create_multi_pane_specular(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
      window_standards::Optical_Standard_Method const & method,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands)
    {
        std::vector<std::shared_ptr<SingleLayerOptics::SpecularLayer>> layers;
        auto number_of_layers = product_data.size();
        for(std::shared_ptr<wincalc::Product_Data_Optical> const & product : product_data)
        {
            layers.push_back(create_specular_layer(
              product, method, number_of_layers, type, number_visible_bands, number_solar_bands));
        }

        std::vector<std::vector<double>> wavelengths = get_wavelengths(product_data);
        auto source_spectrum = get_spectum_values(method.source_spectrum, method, wavelengths);
        auto detector_spectrum = get_spectum_values(method.detector_spectrum, method, wavelengths);

        auto layer =
          MultiLayerOptics::CMultiPaneSpecular::create(layers, source_spectrum, detector_spectrum);

        return layer;
    }

    std::shared_ptr<SingleLayerOptics::CBSDFLayer> create_bsdf_layer_specular(
      std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
      window_standards::Optical_Standard_Method const & method,
      size_t number_of_layers,
      SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands)
    {
        auto material = create_material(
          product_data, method, number_of_layers, type, number_visible_bands, number_solar_bands);
        auto layer =
          SingleLayerOptics::CBSDFLayerMaker::getSpecularLayer(material, bsdf_hemisphere);
        return layer;
    }

    std::shared_ptr<SingleLayerOptics::CBSDFLayer> create_bsdf_layer_perfectly_diffuse(
      std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
      window_standards::Optical_Standard_Method const & method,
      size_t number_of_layers,
      SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands)
    {
        auto material = create_material(
          product_data, method, number_of_layers, type, number_visible_bands, number_solar_bands);
        auto layer =
          SingleLayerOptics::CBSDFLayerMaker::getPerfectlyDiffuseLayer(material, bsdf_hemisphere);
        return layer;
    }

    std::shared_ptr<SingleLayerOptics::CBSDFLayer> create_bsdf_layer_preloaded_matrices(
      std::shared_ptr<wincalc::Product_Data_Dual_Band_Optical_BSDF> const & product_data,
      window_standards::Optical_Standard_Method const & method,
      size_t number_of_layers,
      SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands)
    {
        auto material = create_material(
          product_data, method, number_of_layers, type, number_visible_bands, number_solar_bands);
        auto layer =
          SingleLayerOptics::CBSDFLayerMaker::getPreLoadedBSDFLayer(material, bsdf_hemisphere);
        return layer;
    }

    std::shared_ptr<SingleLayerOptics::CBSDFLayer> create_bsdf_layer_venetian(
      std::shared_ptr<wincalc::Product_Data_Optical_Venetian> const & product_data,
      window_standards::Optical_Standard_Method const & method,
      size_t number_of_layers,
      SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands)
    {
        auto material = create_material(product_data->material_optical_data,
                                        method,
                                        number_of_layers,
                                        type,
                                        number_visible_bands,
                                        number_solar_bands);
        auto layer =
          SingleLayerOptics::CBSDFLayerMaker::getVenetianLayer(material,
                                                               bsdf_hemisphere,
                                                               product_data->geometry.slat_width,
                                                               product_data->geometry.slat_spacing,
                                                               product_data->geometry.slat_tilt,
                                                               product_data->geometry.slat_curvature,
                                                               product_data->geometry.number_slat_segments,
                                                               product_data->geometry.distribution_method,
                                                               product_data->geometry.is_horizontal);
        return layer;
    }

    std::shared_ptr<SingleLayerOptics::CBSDFLayer> create_bsdf_layer_woven_shade(
      std::shared_ptr<wincalc::Product_Data_Optical_Woven_Shade> const & product_data,
      window_standards::Optical_Standard_Method const & method,
      size_t number_of_layers,
      SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands)
    {
        auto material = create_material(product_data->material_optical_data,
                                        method,
                                        number_of_layers,
                                        type,
                                        number_visible_bands,
                                        number_solar_bands);
        auto layer = SingleLayerOptics::CBSDFLayerMaker::getWovenLayer(
          material, bsdf_hemisphere, product_data->geometry.thread_diameter, product_data->geometry.thread_spacing);
        return layer;
    }

    std::shared_ptr<SingleLayerOptics::CBSDFLayer> create_bsdf_layer_perforated_screen(
      std::shared_ptr<wincalc::Product_Data_Optical_Perforated_Screen> const & product_data,
      window_standards::Optical_Standard_Method const & method,
      size_t number_of_layers,
      SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands)
    {
        auto material = create_material(product_data->material_optical_data,
                                        method,
                                        number_of_layers,
                                        type,
                                        number_visible_bands,
                                        number_solar_bands);
        if(product_data->geometry.perforation_type
           == wincalc::Perforated_Geometry::Type::CIRCULAR)
        {
            return SingleLayerOptics::CBSDFLayerMaker::getCircularPerforatedLayer(
              material,
              bsdf_hemisphere,
              product_data->geometry.spacing_x,
              product_data->geometry.spacing_y,
              product_data->material_optical_data->thickness_meters,
              product_data->geometry.dimension_x);
        }
        else if(product_data->geometry.perforation_type
                == wincalc::Perforated_Geometry::Type::RECTANGULAR)
        {
            return SingleLayerOptics::CBSDFLayerMaker::getRectangularPerforatedLayer(
              material,
              bsdf_hemisphere,
              product_data->geometry.spacing_x,
              product_data->geometry.spacing_y,
              product_data->material_optical_data->thickness_meters,
              product_data->geometry.dimension_x,
              product_data->geometry.dimension_y);
        }
        else if(product_data->geometry.perforation_type
                == wincalc::Perforated_Geometry::Type::SQUARE)
        {
            return SingleLayerOptics::CBSDFLayerMaker::getRectangularPerforatedLayer(
              material,
              bsdf_hemisphere,
              product_data->geometry.spacing_x,
              product_data->geometry.spacing_y,
              product_data->material_optical_data->thickness_meters,
              product_data->geometry.dimension_x,
              product_data->geometry.dimension_x);
        }
        else
        {
            std::stringstream msg;
            msg << "Unsupported perforation type for optical bsdf layer: "
                << static_cast<
                     std::underlying_type<Perforated_Geometry::Type>::type>(
                     product_data->geometry.perforation_type);
            throw std::runtime_error(msg.str());
        }
    }


    std::shared_ptr<SingleLayerOptics::CBSDFLayer>
      create_bsdf_layer(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
                        window_standards::Optical_Standard_Method const & method,
                        size_t number_of_layers,
                        SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
                        Spectal_Data_Wavelength_Range_Method const & type,
                        int number_visible_bands,
                        int number_solar_bands)
    {
        std::shared_ptr<SingleLayerOptics::CBSDFLayer> layer;
        if(std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Perfectly_Diffuse>(product_data))
        {
            layer = create_bsdf_layer_perfectly_diffuse(
              std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Perfectly_Diffuse>(
                product_data)
                ->material_optical_data,
              method,
              number_of_layers,
              bsdf_hemisphere,
              type,
              number_visible_bands,
              number_solar_bands);
        }
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Venetian>(product_data))
        {
            layer = create_bsdf_layer_venetian(
              std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Venetian>(product_data),
              method,
              number_of_layers,
              bsdf_hemisphere,
              type,
              number_visible_bands,
              number_solar_bands);
        }
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Woven_Shade>(product_data))
        {
            layer = create_bsdf_layer_woven_shade(
              std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Woven_Shade>(product_data),
              method,
              number_of_layers,
              bsdf_hemisphere,
              type,
              number_visible_bands,
              number_solar_bands);
        }
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Perforated_Screen>(
                  product_data))
        {
            layer = create_bsdf_layer_perforated_screen(
              std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Perforated_Screen>(
                product_data),
              method,
              number_of_layers,
              bsdf_hemisphere,
              type,
              number_visible_bands,
              number_solar_bands);
            auto tf = layer->getResults()->DirHem(FenestrationCommon::Side::Front,
                                                  FenestrationCommon::PropertySimple::T);
        }
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_Optical_With_Material>(
                  product_data))
        {
            layer = create_bsdf_layer_specular(
              std::dynamic_pointer_cast<wincalc::Product_Data_Optical_With_Material>(product_data)
                ->material_optical_data,
              method,
              number_of_layers,
              bsdf_hemisphere,
              type,
              number_visible_bands,
              number_solar_bands);
        }
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_BSDF>(
                  product_data))
        {
            if(method.name == "THERMAL IR")
            {
                // Thermal IR is a special case that can be calculated despite a lack of
                // BSDF data.  Since there is no BSDF for the IR range yet the IR range
                // is instead modeled as a perfectly diffusing shade
                layer = create_bsdf_layer_perfectly_diffuse(product_data,
                                                            method,
                                                            number_of_layers,
                                                            bsdf_hemisphere,
                                                            type,
                                                            number_visible_bands,
                                                            number_solar_bands);
            }
            else
            {
                layer = create_bsdf_layer_preloaded_matrices(
                  std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_BSDF>(
                    product_data),
                  method,
                  number_of_layers,
                  bsdf_hemisphere,
                  type,
                  number_visible_bands,
                  number_solar_bands);
            }
        }
        else
        {
            layer = create_bsdf_layer_specular(product_data,
                                               method,
                                               number_of_layers,
                                               bsdf_hemisphere,
                                               type,
                                               number_visible_bands,
                                               number_solar_bands);
        }


        return layer;
    }

    std::unique_ptr<MultiLayerOptics::CMultiPaneBSDF> create_multi_pane_bsdf(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & products,
      window_standards::Optical_Standard_Method const & method,
      SingleLayerOptics::CBSDFHemisphere const & bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands)
    {
        std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> layers;
        std::vector<std::vector<double>> wavelengths;
        auto number_of_layers = products.size();
        for(auto const & product : products)
        {
            layers.push_back(create_bsdf_layer(product,
                                               method,
                                               number_of_layers,
                                               bsdf_hemisphere,
                                               type,
                                               number_visible_bands,
                                               number_solar_bands));
            wavelengths.push_back(product->wavelengths());
        }

        auto source_spectrum = get_spectum_values(method.source_spectrum, method, wavelengths);

        auto detector_spectrum = get_spectum_values(method.detector_spectrum, method, wavelengths);

        auto layer =
          MultiLayerOptics::CMultiPaneBSDF::create(layers, source_spectrum, detector_spectrum);

        return layer;
    }

    std::unique_ptr<SingleLayerOptics::IScatteringLayer> create_multi_pane(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
      window_standards::Optical_Standard_Method const & method,
      std::optional<SingleLayerOptics::CBSDFHemisphere> bsdf_hemisphere,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands)
    {
        bool as_bsdf = false;
        for(auto product : product_data)
        {
            if(std::dynamic_pointer_cast<wincalc::Product_Data_Optical_With_Material>(product)
               || std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_BSDF>(product))
            {
                as_bsdf = true;
                break;
            }
        }
        if(as_bsdf && !bsdf_hemisphere.has_value())
        {
            throw std::runtime_error(
              "No BSDF hemisphere provided for a system with at least one bsdf type.");
        }

        as_bsdf =
          as_bsdf || bsdf_hemisphere.has_value();   // Use bsdf method if at least one product is
                                                    // bsdf type or a bsdf hemisphere was provided

        if(as_bsdf)
        {
            return create_multi_pane_bsdf(product_data,
                                          method,
                                          bsdf_hemisphere.value(),
                                          type,
                                          number_visible_bands,
                                          number_solar_bands);
        }
        else
        {
            return create_multi_pane_specular(
              product_data, method, type, number_visible_bands, number_solar_bands);
        }
    }

    Tarcog::ISO15099::CIGU create_igu(
      std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUSolidLayer>> const & solid_layers,
      std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> const & gaps,
      double width,
      double height,
      double tilt)
    {
        Tarcog::ISO15099::CIGU igu(width, height, tilt);
        igu.addLayer(solid_layers[0]);
        if(!gaps.empty())
        {
            for(size_t i = 0; i < gaps.size(); ++i)
            {
                // if there are gaps then there should be one more solid layer
                // than gap layer.  So we can just add a gap then a solid layer
                // at the next index until the end of the gaps
                igu.addLayer(gaps[i]);
                igu.addLayer(solid_layers[i + 1]);
            }
        }
        return igu;
    }

    Tarcog::ISO15099::CIGU
      create_igu(std::vector<wincalc::Product_Data_Optical_Thermal> const & layers,
                 std::vector<Engine_Gap_Info> const & gaps,
                 double width,
                 double height,
                 double tilt,
                 window_standards::Optical_Standard const & standard,
                 double theta,
                 double phi,
                 std::optional<SingleLayerOptics::CBSDFHemisphere> bsdf_hemisphere)
    {
        std::ignore = theta;
        std::ignore = phi;
        std::ignore = bsdf_hemisphere;
        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUSolidLayer>> tarcog_solid_layers;
        auto ir_method = standard.methods.at("THERMAL IR");

        for(auto const & layer : layers)
        {
            if(!layer.thermal_data->conductivity.has_value())
            {
                throw std::runtime_error("Missing conductivity");
            }
            auto ir_results = calc_thermal_ir(standard, layer);

            auto effective_thermal_values =
              layer.optical_data->effective_thermal_values(width,
                                                           height,
                                                           layer.thermal_data->opening_top,
                                                           layer.thermal_data->opening_bottom,
                                                           layer.thermal_data->opening_left,
                                                           layer.thermal_data->opening_right);
            auto effective_openness = effective_thermal_values->getEffectiveOpenness();
            auto tarcog_layer =
              Tarcog::ISO15099::Layers::shading(effective_thermal_values->effectiveThickness(),
                                                layer.thermal_data->conductivity.value(),
                                                effective_openness,
                                                ir_results.emissivity_front_hemispheric,
                                                ir_results.transmittance_front_diffuse_diffuse,
                                                ir_results.emissivity_back_hemispheric,
                                                ir_results.transmittance_back_diffuse_diffuse);
            tarcog_layer = Tarcog::ISO15099::Layers::updateMaterialData(
              tarcog_layer,
              layer.thermal_data->density.value_or(Tarcog::MaterialConstants::GLASSDENSITY),
              layer.thermal_data->youngs_modulus.value_or(
                Tarcog::DeflectionConstants::YOUNGSMODULUS));
            tarcog_solid_layers.push_back(tarcog_layer);
        }

        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> tarcog_gaps;
        for(const Engine_Gap_Info & engine_gap_info : gaps)
        {
            std::vector<std::pair<double, Gases::CGasData>> converted_gas;
            for(Engine_Gas_Mixture_Component gas : engine_gap_info.gases)
            {
                converted_gas.emplace_back(gas.percent, gas.gas);
            }
            auto gap = Tarcog::ISO15099::Layers::gap(
              engine_gap_info.thickness, Gases::CGas(converted_gas), engine_gap_info.pressure);
            if(engine_gap_info.pillar)
            {
                gap = engine_gap_info.pillar->createGapPillar(gap);
            }
            tarcog_gaps.push_back(gap);
        }

        return create_igu(tarcog_solid_layers, tarcog_gaps, width, height, tilt);
    }

    Tarcog::ISO15099::CSystem create_system(Tarcog::ISO15099::CIGU & igu,
                                            Environments const & environments)
    {
        auto indoor = create_indoor_environment(environments.inside);
        auto outdoor = create_outdoor_environment(environments.outside);
        auto system = Tarcog::ISO15099::CSystem(igu, indoor, outdoor);
        return system;
    }
}   // namespace wincalc
