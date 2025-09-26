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
            Tarcog::ISO15099::Environments::indoor(environment.air_temperature,
                                                   environment.pressure);

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
        if(!data.directComponent.has_value() && !data.diffuseComponent.has_value())
        {
            std::stringstream msg;
            msg << "Wavelength " << data.wavelength << " does not have either a direct or diffuse measurement";
            throw std::runtime_error(msg.str());
        }
        // clang-format off
        SpectralAveraging::MeasuredRow converted
        {
            data.wavelength,
            {
                data.directComponent.has_value() ? data.directComponent.value().tf : 0.0,
                data.directComponent.has_value() ? data.directComponent.value().tb : 0.0,
                data.directComponent.has_value() ? data.directComponent.value().rf : 0.0,
                data.directComponent.has_value() ? data.directComponent.value().rb : 0.0
            },
            {
                data.diffuseComponent.has_value() ? data.diffuseComponent.value().tf : 0.0,
                data.diffuseComponent.has_value() ? data.diffuseComponent.value().tb : 0.0,
                data.diffuseComponent.has_value() ? data.diffuseComponent.value().rf : 0.0,
                data.diffuseComponent.has_value() ? data.diffuseComponent.value().rb : 0.0
            }
        };
        // clang-format on
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
                std::get<0>(val),
                std::get<1>(val),
                std::get<1>(val),
                std::get<2>(val),
                std::get<3>(val));
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
        LOGMSG("begin get_minimum_wavelength for method " + method.name
            + " with product_data_min_wavelength = "
            + std::to_string(product_data_min_wavelength));
        double result = std::numeric_limits<double>::quiet_NaN();

        if(method.min_wavelength.type == window_standards::Wavelength_Boundary_Type::WAVELENGTH_SET)
        {
            LOGMSG("in if(method.min_wavelength.type == "
                "window_standards::Wavelength_Boundary_Type::WAVELENGTH_SET)");
            if(method.wavelength_set.type == window_standards::Wavelength_Set_Type::FILE)
            {
                LOGMSG("in if(method.wavelength_set.type == "
                    "window_standards::Wavelength_Set_Type::FILE) with "
                    "method.wavelength_set.values.size = "
                    + std::to_string(method.wavelength_set.values.size()));
                result = method.wavelength_set.values[0];
            }
            else if(method.wavelength_set.type == window_standards::Wavelength_Set_Type::SOURCE)
            {
                LOGMSG("in else if(method.wavelength_set.type == "
                    "window_standards::Wavelength_Set_Type::SOURCE) with "
                    "source_spectrum.getXArray().size() = "
                    + std::to_string(source_spectrum.getXArray().size()));
                result = source_spectrum.getXArray().front();
            }
            if(method.wavelength_set.type == window_standards::Wavelength_Set_Type::DATA)
            {
                LOGMSG("method.wavelength_set.type == window_standards::Wavelength_Set_Type::DATA");
                result = product_data_min_wavelength;
            }
        }
        else if(method.min_wavelength.type == window_standards::Wavelength_Boundary_Type::NUMBER)
        {
            LOGMSG("in else if(method.min_wavelength.type == "
                "window_standards::Wavelength_Boundary_Type::NUMBER) with "
                "method.min_wavelength.value = "
                + std::to_string(method.min_wavelength.value));
            result = method.min_wavelength.value;
        }

        LOGMSG("end get_minimum_wavelength for method " + method.name
            + " with product_data_min_wavelength = "
            + std::to_string(product_data_min_wavelength)
            + " result = " + std::to_string(result));

        return result;
    }


    double get_maximum_wavelength(window_standards::Optical_Standard_Method const & method,
                                  double product_data_max_wavelength,
                                  FenestrationCommon::CSeries const & source_spectrum)
    {
        LOGMSG("begin get_maximum_wavelength for method " + method.name
            + " with product_data_max_wavelength = "
            + std::to_string(product_data_max_wavelength));

        double result = std::numeric_limits<double>::quiet_NaN();

        if(method.max_wavelength.type == window_standards::Wavelength_Boundary_Type::WAVELENGTH_SET)
        {
            LOGMSG("in if(method.max_wavelength.type == "
                "window_standards::Wavelength_Boundary_Type::WAVELENGTH_SET)");
            if(method.wavelength_set.type == window_standards::Wavelength_Set_Type::FILE)
            {
                LOGMSG("in if(method.wavelength_set.type == "
                    "window_standards::Wavelength_Set_Type::FILE) with "
                    "method.wavelength_set.values.size = "
                    + std::to_string(method.wavelength_set.values.size()));

                result = method.wavelength_set.values.back();
            }
            else if(method.wavelength_set.type == window_standards::Wavelength_Set_Type::SOURCE)
            {
                LOGMSG("in else if(method.wavelength_set.type == "
                    "window_standards::Wavelength_Set_Type::SOURCE) with "
                    "source_spectrum.getXArray().size() = "
                    + std::to_string(source_spectrum.getXArray().size()));
                result = source_spectrum.getXArray().back();
            }
            if(method.wavelength_set.type == window_standards::Wavelength_Set_Type::DATA)
            {
                LOGMSG("method.wavelength_set.type == window_standards::Wavelength_Set_Type::DATA");
                result = product_data_max_wavelength;
            }
        }
        else if(method.max_wavelength.type == window_standards::Wavelength_Boundary_Type::NUMBER)
        {
            LOGMSG("in else if(method.max_wavelength.type == "
                "window_standards::Wavelength_Boundary_Type::NUMBER) with "
                "method.max_wavelength.value = "
                + std::to_string(method.max_wavelength.value));
            result = method.max_wavelength.value;
        }

        LOGMSG("end get_maximum_wavelength for method " + method.name
            + " with product_data_max_wavelength = "
            + std::to_string(product_data_max_wavelength)
            + " result = " + std::to_string(result));
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
                        result =
                            SpectralAveraging::UVAction(product_wavelengths,
                                                        spectrum.a,
                                                        spectrum.b);
                        break;
                    case window_standards::Wavelength_Set_Type::SOURCE:
                        // Wavelengths come from the source spectrum.  Extract first column and pass
                        // those
                        result = SpectralAveraging::UVAction(
                            get_first_val(method.source_spectrum.values),
                            spectrum.a,
                            spectrum.b);
                        break;
                    case window_standards::Wavelength_Set_Type::FILE:
                        // Wavelengths should already be loaded into the wavelength_set
                        result = SpectralAveraging::UVAction(
                            method.wavelength_set.values,
                            spectrum.a,
                            spectrum.b);
                        break;
                }
                break;
            case window_standards::Spectrum_Type::KROCHMANN:
                switch(method.wavelength_set.type)
                {
                    case window_standards::Wavelength_Set_Type::DATA:
                        // Wavelengths come from the measured data.  Extract first column and pass
                        // those
                        result = SpectralAveraging::Krochmann(product_wavelengths);
                        break;
                    case window_standards::Wavelength_Set_Type::SOURCE:
                        // Wavelengths come from the source spectrum.  Extract first column and pass
                        // those
                        result = SpectralAveraging::Krochmann(
                            get_first_val(method.source_spectrum.values));
                        break;
                    case window_standards::Wavelength_Set_Type::FILE:
                        // Wavelengths should already be loaded into the wavelength_set
                        result = SpectralAveraging::Krochmann(method.wavelength_set.values);
                        break;
                }
                break;
            case window_standards::Spectrum_Type::BLACKBODY:
                switch(method.wavelength_set.type)
                {
                    case window_standards::Wavelength_Set_Type::DATA:
                        // Wavelengths come from the measured data.  Extract first column and pass
                        // those
                        result =
                            SpectralAveraging::BlackBodySpectrum(product_wavelengths, spectrum.t);
                        break;
                    case window_standards::Wavelength_Set_Type::SOURCE:
                        // Wavelengths come from the source spectrum.  Extract first column and pass
                        // those
                        result = SpectralAveraging::BlackBodySpectrum(
                            get_first_val(method.source_spectrum.values),
                            spectrum.t);
                        break;
                    case window_standards::Wavelength_Set_Type::FILE:
                        // Wavelengths should already be loaded into the wavelength_set
                        result = SpectralAveraging::BlackBodySpectrum(method.wavelength_set.values,
                            spectrum.t);
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

    std::vector<double> get_wavelength_set_to_use_combined_layers(
        std::vector<std::vector<double>> const & product_wavelengths)
    {
        FenestrationCommon::CCommonWavelengths combined;
        for(auto const & wl : product_wavelengths)
        {
            combined.addWavelength(wl);
        }

        return combined.getCombinedWavelengths(FenestrationCommon::Combine::Interpolate);
    }

    std::vector<double>
        optical_standard_wavelength_set(window_standards::Optical_Standard_Method const & method,
                                        std::vector<std::vector<double>> const &
                                        product_wavelengths)
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
                result = get_wavelength_set_to_use_combined_layers(product_wavelengths);
            }
        }

        return result;
    }

    std::vector<double> combined_layer_wavelength_range_factory(
        std::vector<std::vector<double>> const & product_wavelengths,
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
            return get_wavelength_set_to_use_combined_layers(product_wavelengths);
        }
        else
        {
            throw std::runtime_error("Unknown wavelength method.");
        }
    }

    std::shared_ptr<SingleLayerOptics::CMaterial>
        create_material(wincalc::Product_Data_Dual_Band_Optical_Hemispheric const & product)
    {
        std::shared_ptr<SingleLayerOptics::CMaterial> material =
            SingleLayerOptics::Material::dualBandMaterial(product.tf_solar,
                                                          product.tb_solar,
                                                          product.rf_solar,
                                                          product.rb_solar,
                                                          product.tf_visible,
                                                          product.tb_visible,
                                                          product.rf_visible,
                                                          product.rb_visible);

        return material;
    }

    std::shared_ptr<SingleLayerOptics::CMaterial>
        create_material(wincalc::Product_Data_Dual_Band_Optical_BSDF const & product,
                        window_standards::Optical_Standard_Method const & optical_method,
                        size_t number_of_layers)
    {
        std::shared_ptr<SingleLayerOptics::CMaterial> material;
        if(number_of_layers == 1 && to_lower(optical_method.name) == "solar")
        {
            material = SingleLayerOptics::Material::singleBandBSDFMaterial(product.tf_solar,
                product.tb_solar,
                product.rf_solar,
                product.rb_solar,
                product.bsdf_hemisphere);
        }
        else if(number_of_layers == 1 && to_lower(optical_method.name) == "photopic")
        {
            material = SingleLayerOptics::Material::singleBandBSDFMaterial(product.tf_visible,
                product.tb_visible,
                product.rf_visible,
                product.rb_visible,
                product.bsdf_hemisphere);
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
                    0.49); // TODO, replace 0.49 ratio
        }
        return material;
    }


    Lambda_Range get_lambda_range(std::vector<std::vector<double>> const & products_wavelengths,
                                  window_standards::Optical_Standard_Method const & method)
    {
        LOGMSG("begin get_lambda_range");
        auto source_spectrum =
            get_spectum_values(method.source_spectrum, method, products_wavelengths);
        std::vector<double> min_wavelengths;
        std::vector<double> max_wavelengths;
        for(auto product_wavelengths : products_wavelengths)
        {
            LOGMSG("in for(auto product_wavelengths : products_wavelengths)");
            min_wavelengths.push_back(
                get_minimum_wavelength(method, product_wavelengths[0], source_spectrum));
            LOGMSG("before max_wavelengths.push_back");
            max_wavelengths.push_back(
                get_maximum_wavelength(method, product_wavelengths.back(), source_spectrum));
        }
        // The min and max lambda should be the tighest boundary not the loosest
        //  So it should be the largest minimum and smallest maximum
        LOGMSG("before min_wavelength = with min_wavelengths.size = "
            + std::to_string(min_wavelengths.size()));
        double min_wavelength = *std::max_element(min_wavelengths.begin(), min_wavelengths.end());
        LOGMSG("before max_wavelength = with max_wavelengths.size = "
            + std::to_string(max_wavelengths.size()));
        double max_wavelength = *std::min_element(max_wavelengths.begin(), max_wavelengths.end());
        LOGMSG("end get_lambda_range");
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
        create_material(wincalc::Product_Data_N_Band_Optical const & product_data)
    {
        auto measured_wavelength_data = convert(product_data.wavelength_data);
        auto spectral_sample_data =
            SpectralAveraging::CSpectralSampleData::create(measured_wavelength_data);

        std::shared_ptr<SingleLayerOptics::CMaterial> material =
            SingleLayerOptics::Material::nBandMaterial(
                spectral_sample_data,
                product_data.thickness_meters,
                product_data.material_type);
        return material;
    }

    std::shared_ptr<SingleLayerOptics::CMaterial>
        create_pv_material(wincalc::Product_Data_N_Band_Optical const & product_data)
    {
        auto measured_wavelength_data = convert(product_data.wavelength_data);
        auto spectral_sample_data =
            SpectralAveraging::CSpectralSampleData::create(measured_wavelength_data);

        auto eqef = get_eqef(product_data.wavelength_data);
        auto eqeb = get_eqeb(product_data.wavelength_data);

        auto sample = SpectralAveraging::CSpectralSampleData::create(measured_wavelength_data);
        auto pvSample = std::make_shared<SpectralAveraging::PhotovoltaicSampleData>(
            *sample,
            eqef,
            eqeb);

        auto material = SingleLayerOptics::Material::nBandPhotovoltaicMaterial(
            pvSample,
            product_data.thickness_meters,
            product_data.material_type);

        return material;
    }

    std::shared_ptr<SingleLayerOptics::CMaterial>
        create_material(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
                        window_standards::Optical_Standard_Method const & method,
                        size_t number_of_layers)
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
                    *std::dynamic_pointer_cast<Product_Data_N_Band_Optical>(product_data));
            }
            else if(std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_BSDF>(
                product_data))
            {
                material = create_material(
                    *std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_BSDF>(
                        product_data),
                    method,
                    number_of_layers);
            }
            else if(std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_Hemispheric>(
                product_data))
            {
                material = create_material(
                    *std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_Hemispheric>(
                        product_data));
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
                material = SingleLayerOptics::Material::singleBandMaterial(tf, tb, rf, rb);
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
                           window_standards::Optical_Standard_Method const & method)
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
                    *std::dynamic_pointer_cast<Product_Data_N_Band_Optical>(product_data));
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
                material = SingleLayerOptics::Material::singleBandMaterial(tf, tb, rf, rb);
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
                              size_t number_of_layers)
    {
        std::shared_ptr<SingleLayerOptics::SpecularLayer> specular_layer;
        if(is_pv(product_data))
        {
            auto material = create_pv_material(product_data, method);
            auto power_properties = get_power_properties(product_data);
            specular_layer =
                SingleLayerOptics::PhotovoltaicSpecularLayer::createLayer(
                    material,
                    power_properties);
        }
        else
        {
            auto material = create_material(product_data, method, number_of_layers);
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
        LOGMSG("begin create_multi_pane_specular(product_data, " + method.name + ")");
        std::vector<std::shared_ptr<SingleLayerOptics::SpecularLayer>> layers;
        LOGMSG("before number_of_layers = product_data.size()");
        auto number_of_layers = product_data.size();
        for(std::shared_ptr<wincalc::Product_Data_Optical> const & product : product_data)
        {
            LOGMSG("in for(std::shared_ptr<wincalc::Product_Data_Optical> const & product : "
                "product_data)");
            layers.push_back(create_specular_layer(product, method, number_of_layers));
        }

        LOGMSG("before wavelengths = get_wavelengths");
        std::vector<std::vector<double>> wavelengths = get_wavelengths(product_data);
        LOGMSG("before source_spectrum = get_spectum_values");
        auto source_spectrum = get_spectum_values(method.source_spectrum, method, wavelengths);
        LOGMSG("before detector_spectrum = get_spectum_values");
        auto detector_spectrum = get_spectum_values(method.detector_spectrum, method, wavelengths);
        LOGMSG("before MultiLayerOptics::CMultiPaneSpecular::create");
        auto layer = MultiLayerOptics::CMultiPaneSpecular::create(layers);
        LOGMSG("before standard_wavelengths = optical_standard_wavelength_set");
        auto standard_wavelengths = optical_standard_wavelength_set(method, wavelengths);
        if(type == Spectal_Data_Wavelength_Range_Method::CONDENSED
           && ((method.name == "SOLAR") || (method.name == "PHOTOPIC")))
        {
            standard_wavelengths = combined_layer_wavelength_range_factory(
                wavelengths,
                type,
                number_visible_bands,
                number_solar_bands);
        }
        LOGMSG("before SingleLayerOptics::CalculationProperties input");
        const SingleLayerOptics::CalculationProperties input{
            source_spectrum, standard_wavelengths, detector_spectrum};
        LOGMSG("before layer->setCalculationProperties");
        layer->setCalculationProperties(input);
        LOGMSG("end create_multi_pane_specular(product_data, " + method.name + ")");
        return layer;
    }

    std::shared_ptr<SingleLayerOptics::CBSDFLayer> create_bsdf_layer_specular(
        std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
        window_standards::Optical_Standard_Method const & method,
        size_t number_of_layers,
        SingleLayerOptics::BSDFHemisphere const & bsdf_hemisphere)
    {
        LOGMSG("begin create_bsdf_layer_specular(product_data, " + method.name + ")");
        auto material = create_material(product_data, method, number_of_layers);
        LOGMSG("before SingleLayerOptics::CBSDFLayerMaker::getSpecularLayer");
        auto layer =
            SingleLayerOptics::CBSDFLayerMaker::getSpecularLayer(material, bsdf_hemisphere);
        LOGMSG("begin create_bsdf_layer_specular(product_data, " + method.name + ")");
        return layer;
    }

    std::shared_ptr<SingleLayerOptics::CBSDFLayer> create_bsdf_layer_direct_diffuse(
        std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
        window_standards::Optical_Standard_Method const & method,
        size_t number_of_layers,
        SingleLayerOptics::BSDFHemisphere const & bsdf_hemisphere)
    {
        LOGMSG("begin create_bsdf_layer_direct_diffuse(product_data, " + method.name + ")");
        auto material = create_material(product_data, method, number_of_layers);
        LOGMSG("before SingleLayerOptics::CBSDFLayerMaker::getDirDifLayer");
        auto layer =
            SingleLayerOptics::CBSDFLayerMaker::getDirDifLayer(material, bsdf_hemisphere);
        LOGMSG("begin create_bsdf_layer_direct_diffuse(product_data, " + method.name + ")");
        return layer;
    }

    std::shared_ptr<SingleLayerOptics::CBSDFLayer> create_bsdf_layer_perfectly_diffuse(
        std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
        window_standards::Optical_Standard_Method const & method,
        size_t number_of_layers,
        SingleLayerOptics::BSDFHemisphere const & bsdf_hemisphere)
    {
        LOGMSG("begin create_bsdf_layer_perfectly_diffuse(product_data, " + method.name + ")");
        auto material = create_material(product_data, method, number_of_layers);
        LOGMSG("before SingleLayerOptics::CBSDFLayerMaker::getPerfectlyDiffuseLayer");
        auto layer =
            SingleLayerOptics::CBSDFLayerMaker::getPerfectlyDiffuseLayer(material, bsdf_hemisphere);
        LOGMSG("end create_bsdf_layer_perfectly_diffuse(product_data, " + method.name + ")");
        return layer;
    }

    std::shared_ptr<SingleLayerOptics::CBSDFLayer> create_bsdf_layer_homogeneous_diffuse(
        std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
        window_standards::Optical_Standard_Method const & method,
        size_t number_of_layers,
        SingleLayerOptics::BSDFHemisphere const & bsdf_hemisphere)
    {
        LOGMSG("begin create_bsdf_layer_homogeneous_diffuse(product_data, " + method.name + ")");
        auto material = create_material(product_data, method, number_of_layers);
        LOGMSG("before SingleLayerOptics::CBSDFLayerMaker::getHomogeneousDiffuseLayer");
        auto layer =
            SingleLayerOptics::CBSDFLayerMaker::getHomogeneousDiffuseLayer(
                material,
                bsdf_hemisphere);
        LOGMSG("end create_bsdf_layer_homogeneous_diffuse(product_data, " + method.name + ")");
        return layer;
    }

    std::shared_ptr<SingleLayerOptics::CBSDFLayer> create_bsdf_layer_preloaded_matrices(
        std::shared_ptr<wincalc::Product_Data_Dual_Band_Optical_BSDF> const & product_data,
        window_standards::Optical_Standard_Method const & method,
        size_t number_of_layers,
        SingleLayerOptics::BSDFHemisphere const & bsdf_hemisphere)
    {
        LOGMSG("begin create_bsdf_layer_preloaded_matrices(product_data, " + method.name + ")");
        auto material = create_material(product_data, method, number_of_layers);
        LOGMSG("before SingleLayerOptics::CBSDFLayerMaker::getPreLoadedBSDFLayer");
        auto layer =
            SingleLayerOptics::CBSDFLayerMaker::getPreLoadedBSDFLayer(material, bsdf_hemisphere);
        LOGMSG("end create_bsdf_layer_preloaded_matrices(product_data, " + method.name + ")");
        return layer;
    }

    std::shared_ptr<SingleLayerOptics::CBSDFLayer> create_bsdf_layer_venetian(
        std::shared_ptr<wincalc::Product_Data_Optical_Venetian> const & product_data,
        window_standards::Optical_Standard_Method const & method,
        size_t number_of_layers,
        SingleLayerOptics::BSDFHemisphere const & bsdf_hemisphere)
    {
        LOGMSG("begin create_bsdf_layer_venetian(product_data, " + method.name + ")");
        auto material =
            create_material(product_data->material_optical_data, method, number_of_layers);
        LOGMSG("before SingleLayerOptics::CBSDFLayerMaker::getVenetianLayer");
        auto layer = SingleLayerOptics::CBSDFLayerMaker::getVenetianLayer(
            material,
            bsdf_hemisphere,
            product_data->geometry.slat_width,
            product_data->geometry.slat_spacing,
            product_data->geometry.slat_tilt,
            product_data->geometry.slat_curvature,
            product_data->geometry.number_slat_segments,
            product_data->geometry.distribution_method,
            product_data->geometry.is_horizontal);
        LOGMSG("end create_bsdf_layer_venetian(product_data, " + method.name + ")");
        return layer;
    }

    std::shared_ptr<SingleLayerOptics::CBSDFLayer> create_bsdf_layer_woven_shade(
        std::shared_ptr<wincalc::Product_Data_Optical_Woven_Shade> const & product_data,
        window_standards::Optical_Standard_Method const & method,
        size_t number_of_layers,
        SingleLayerOptics::BSDFHemisphere const & bsdf_hemisphere)
    {
        LOGMSG("begin create_bsdf_layer_woven_shade(product_data, " + method.name + ")");
        auto material =
            create_material(product_data->material_optical_data, method, number_of_layers);
        LOGMSG("before SingleLayerOptics::CBSDFLayerMaker::getWovenLayer");
        auto layer =
            SingleLayerOptics::CBSDFLayerMaker::getWovenLayer(material,
                                                              bsdf_hemisphere,
                                                              product_data->geometry.
                                                              thread_diameter,
                                                              product_data->geometry.
                                                              thread_spacing);
        LOGMSG("end create_bsdf_layer_woven_shade(product_data, " + method.name + ")");
        return layer;
    }

    std::shared_ptr<SingleLayerOptics::CBSDFLayer> create_bsdf_layer_perforated_screen(
        std::shared_ptr<wincalc::Product_Data_Optical_Perforated_Screen> const & product_data,
        window_standards::Optical_Standard_Method const & method,
        size_t number_of_layers,
        SingleLayerOptics::BSDFHemisphere const & bsdf_hemisphere)
    {
        LOGMSG("begin create_bsdf_layer_perforated_screen(product_data, " + method.name + ")");
        auto material =
            create_material(product_data->material_optical_data, method, number_of_layers);
        LOGMSG("before if in create_bsdf_layer_perforated_screen");
        if(product_data->geometry.perforation_type == wincalc::Perforated_Geometry::Type::CIRCULAR)
        {
            LOGMSG("in circular in create_bsdf_layer_perforated_screen");
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
            LOGMSG("in rectangular in create_bsdf_layer_perforated_screen");
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
            LOGMSG("in square in create_bsdf_layer_perforated_screen");
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
            LOGMSG("in else of create_bsdf_layer_perforated_screen");
            std::stringstream msg;
            msg << "Unsupported perforation type for optical bsdf layer: "
                << static_cast<std::underlying_type<Perforated_Geometry::Type>::type>(
                    product_data->geometry.perforation_type);
            throw std::runtime_error(msg.str());
        }
    }


    std::shared_ptr<SingleLayerOptics::CBSDFLayer>
        create_bsdf_layer(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
                          window_standards::Optical_Standard_Method const & method,
                          size_t number_of_layers,
                          SingleLayerOptics::BSDFHemisphere const & bsdf_hemisphere)
    {
        LOGMSG("begin create_bsdf_layer(product_data, " + method.name + ")");
        std::shared_ptr<SingleLayerOptics::CBSDFLayer> layer;
        if(std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Perfectly_Diffuse>(product_data))
        {
            layer = create_bsdf_layer_perfectly_diffuse(
                std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Perfectly_Diffuse>(
                    product_data)
                ->material_optical_data,
                method,
                number_of_layers,
                bsdf_hemisphere);
        }
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Uniform_Diffuse>(
            product_data))
        {
            layer = create_bsdf_layer_homogeneous_diffuse(
                std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Uniform_Diffuse>(
                    product_data)
                ->material_optical_data,
                method,
                number_of_layers,
                bsdf_hemisphere);
        }
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Venetian>(product_data))
        {
            layer = create_bsdf_layer_venetian(
                std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Venetian>(product_data),
                method,
                number_of_layers,
                bsdf_hemisphere);
        }
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Woven_Shade>(product_data))
        {
            layer = create_bsdf_layer_woven_shade(
                std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Woven_Shade>(product_data),
                method,
                number_of_layers,
                bsdf_hemisphere);
        }
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Perforated_Screen>(
            product_data))
        {
            layer = create_bsdf_layer_perforated_screen(
                std::dynamic_pointer_cast<wincalc::Product_Data_Optical_Perforated_Screen>(
                    product_data),
                method,
                number_of_layers,
                bsdf_hemisphere);
        }
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_Optical_With_Material>(
            product_data))
        {
            layer = create_bsdf_layer_direct_diffuse(
                std::dynamic_pointer_cast<wincalc::Product_Data_Optical_With_Material>(product_data)
                ->material_optical_data,
                method,
                number_of_layers,
                bsdf_hemisphere);
        }
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_BSDF>(
            product_data))
        {
            LOGMSG("in dual band in if in create_bsdf_layer");
            if(method.name == "THERMAL IR")
            {
                LOGMSG("in if(method.name == THERMAL IR)");
                // Thermal IR is a special case that can be calculated despite a lack of
                // BSDF data.  Since there is no BSDF for the IR range yet the IR range
                // is instead modeled as a perfectly diffusing shade
                layer = create_bsdf_layer_perfectly_diffuse(
                    product_data,
                    method,
                    number_of_layers,
                    bsdf_hemisphere);
            }
            else
            {
                LOGMSG("in else of if(method.name == THERMAL IR)");
                layer = create_bsdf_layer_preloaded_matrices(
                    std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_BSDF>(
                        product_data),
                    method,
                    number_of_layers,
                    bsdf_hemisphere);
            }
        }
        else
        {
            LOGMSG("in else in create_bsdf_layer");
            layer =
                create_bsdf_layer_direct_diffuse(product_data, method, number_of_layers, bsdf_hemisphere);
        }

        LOGMSG("end create_bsdf_layer(product_data, " + method.name + ")");
        return layer;
    }

    std::unique_ptr<MultiLayerOptics::CMultiPaneBSDF> create_multi_pane_bsdf(
        std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & products,
        window_standards::Optical_Standard_Method const & method,
        SingleLayerOptics::BSDFHemisphere const & bsdf_hemisphere,
        Spectal_Data_Wavelength_Range_Method const & type,
        int number_visible_bands,
        int number_solar_bands)
    {
        LOGMSG("begin create_multi_pane_bsdf(products, " + method.name + ")");
        std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> layers;
        std::vector<std::vector<double>> wavelengths;
        LOGMSG("before number_of_layers");
        auto number_of_layers = products.size();
        for(auto const & product : products)
        {
            LOGMSG("in for(auto const & product : products)");
            layers.push_back(create_bsdf_layer(product, method, number_of_layers, bsdf_hemisphere));
            LOGMSG("before wavelengths.push_back(product->wavelengths());");
            wavelengths.push_back(product->wavelengths());
        }

        LOGMSG("before source_spectrum");
        auto source_spectrum = get_spectum_values(method.source_spectrum, method, wavelengths);
        LOGMSG("before detector_spectrum");
        auto detector_spectrum = get_spectum_values(method.detector_spectrum, method, wavelengths);
        LOGMSG("before combined_layer_wavelength_range_factory");
        auto combined_layer_wavelengths = combined_layer_wavelength_range_factory(
            wavelengths,
            type,
            number_visible_bands,
            number_solar_bands);

        LOGMSG("before MultiLayerOptics::CMultiPaneBSDF::create");
        auto layer = MultiLayerOptics::CMultiPaneBSDF::create(layers);
        LOGMSG("before standard_wavelengths = optical_standard_wavelength_set");
        auto standard_wavelengths = optical_standard_wavelength_set(method, wavelengths);
        if(type == Spectal_Data_Wavelength_Range_Method::CONDENSED
           && ((method.name == "SOLAR") || (method.name == "PHOTOPIC")))
        {
            standard_wavelengths = combined_layer_wavelength_range_factory(
                wavelengths,
                type,
                number_visible_bands,
                number_solar_bands);
        }
        LOGMSG("before SingleLayerOptics::CalculationProperties input");
        const SingleLayerOptics::CalculationProperties input{
            source_spectrum, standard_wavelengths, detector_spectrum};
        LOGMSG("before layer->setCalculationProperties");
        layer->setCalculationProperties(input);

        LOGMSG("end create_multi_pane_bsdf(products, " + method.name + ")");
        return layer;
    }

    std::unique_ptr<SingleLayerOptics::IScatteringLayer> create_multi_pane(
        std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
        window_standards::Optical_Standard_Method const & method,
        std::optional<SingleLayerOptics::BSDFHemisphere> bsdf_hemisphere,
        Spectal_Data_Wavelength_Range_Method const & type,
        int number_visible_bands,
        int number_solar_bands)
    {
        LOGMSG("begin create_multi_pane(product_data, " + method.name + ")");
        bool as_bsdf = false;
        for(auto product : product_data)
        {
            LOGMSG("in for(auto product : product_data)");
            if(std::dynamic_pointer_cast<wincalc::Product_Data_Optical_With_Material>(product)
               || std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical_BSDF>(product))
            {
                as_bsdf = true;
                break;
            }

            auto nband = std::dynamic_pointer_cast<wincalc::Product_Data_N_Band_Optical>(product);
            if(nband)
            {
                for(auto wl_row : nband->wavelength_data)
                {
                    if(wl_row.diffuseComponent.has_value())
                    {
                        as_bsdf = true;
                        break;
                    }
                }
            }
            if(as_bsdf)
            {
                break;
            }
        }

        LOGMSG("before if(as_bsdf && !bsdf_hemisphere.has_value())");
        if(as_bsdf && !bsdf_hemisphere.has_value())
        {
            LOGMSG("in if(as_bsdf && !bsdf_hemisphere.has_value())");
            throw std::runtime_error(
                "No BSDF hemisphere provided for a system with at least one bsdf type.");
        }

        as_bsdf =
            as_bsdf || bsdf_hemisphere.has_value(); // Use bsdf method if at least one product is
        // bsdf type or a bsdf hemisphere was provided

        if(as_bsdf)
        {
            LOGMSG("in if(as_bsdf)");
            return create_multi_pane_bsdf(product_data,
                                          method,
                                          bsdf_hemisphere.value(),
                                          type,
                                          number_visible_bands,
                                          number_solar_bands);
        }
        else
        {
            LOGMSG("in else of if(as_bsdf)");
            return create_multi_pane_specular(
                product_data,
                method,
                type,
                number_visible_bands,
                number_solar_bands);
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
                   std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> const & gaps,
                   double width,
                   double height,
                   double tilt,
                   window_standards::Optical_Standard const & standard)
    {
        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUSolidLayer>> tarcog_solid_layers;
        auto ir_method = standard.methods.at("THERMAL IR");

        for(auto const & layer : layers)
        {
            if(!layer.thermal_data->conductivity.has_value())
            {
                throw std::runtime_error("Missing conductivity");
            }
            auto ir_results = calc_thermal_ir(standard, layer);

            auto effective_thermal_values = layer.optical_data->effective_thermal_values(
                width,
                height,
                layer.thermal_data->opening_top,
                layer.thermal_data->opening_bottom,
                layer.thermal_data->opening_left,
                layer.thermal_data->opening_right,
                layer.thermal_data->effective_front_thermal_openness_area,
                layer.thermal_data->permeability_factor);

            auto tarcog_layer =
                Tarcog::ISO15099::Layers::shading(effective_thermal_values->effectiveThickness(),
                                                  layer.thermal_data->conductivity.value(),
                                                  effective_thermal_values->getEffectiveOpenness(),
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

        return create_igu(tarcog_solid_layers, gaps, width, height, tilt);
    }

    Tarcog::ISO15099::CSystem create_system(Tarcog::ISO15099::CIGU & igu,
                                            Environments const & environments)
    {
        auto indoor = create_indoor_environment(environments.inside);
        auto outdoor = create_outdoor_environment(environments.outside);
        auto system = Tarcog::ISO15099::CSystem(igu, indoor, outdoor);
        return system;
    }

    Gases::CGas create_gas(
        std::vector<std::pair<double, std::variant<Gases::GasDef, Gases::CGasData>>> const &
        components)
    {
        auto gas = Gases::CGas();
        for(const auto & item : components)
        {
            auto percent = item.first;
            std::visit([&gas, percent](auto arg) {
                           gas.addGasItem(percent, arg);
                       },
                       item.second);
        }

        return gas;
    }
} // namespace wincalc