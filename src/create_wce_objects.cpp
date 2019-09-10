#include "create_wce_objects.h"

namespace wincalc
{
    std::shared_ptr<Tarcog::ISO15099::CIndoorEnvironment>
      create_indoor_environment(Environment const & environment)
    {
        std::shared_ptr<Tarcog::ISO15099::CIndoorEnvironment> indoor =
          Tarcog::ISO15099::Environments::indoor(environment.air_tempareature,
                                                 environment.pressure);

        indoor->setHCoeffModel(environment.coefficient_model, environment.convection_coefficient);

        return indoor;
    }

    std::shared_ptr<Tarcog::ISO15099::COutdoorEnvironment>
      create_outdoor_environment(Environment const & environment)
    {
        std::shared_ptr<Tarcog::ISO15099::COutdoorEnvironment> outdoor =
          Tarcog::ISO15099::Environments::outdoor(environment.air_tempareature,
                                                  environment.air_speed,
                                                  environment.direct_solar_radiation,
                                                  environment.radiation_temperature,
                                                  Tarcog::ISO15099::SkyModel::AllSpecified,
                                                  environment.pressure);

        outdoor->setHCoeffModel(environment.coefficient_model, environment.convection_coefficient);
        return outdoor;
    }

#if 0
    SpectralAveraging::MeasuredRow convert(OpticsParser::WLData const & data)
    {
        SpectralAveraging::MeasuredRow converted(data.wavelength, data.T, data.frontR, data.backR);
        return converted;
    }
#endif

    SpectralAveraging::MeasuredRow convert(wincalc::Wavelength_Data const & data)
    {
        SpectralAveraging::MeasuredRow converted(
          data.wavelength, data.transmittance_front, data.reflectance_front, data.reflectance_back);
        return converted;
    }

#if 0
    std::vector<SpectralAveraging::MeasuredRow>
      convert(std::vector<OpticsParser::WLData> const & data)
    {
        std::vector<SpectralAveraging::MeasuredRow> converted;

        for(OpticsParser::WLData const & row : data)
        {
            converted.push_back(convert(row));
        }
        return converted;
    }
#endif
    std::vector<SpectralAveraging::MeasuredRow>
      convert(std::vector<wincalc::Wavelength_Data> const & data)
    {
        std::vector<SpectralAveraging::MeasuredRow> converted;

        for(wincalc::Wavelength_Data const & row : data)
        {
            converted.push_back(convert(row));
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


    template<typename T>
    std::vector<double> get_first_val(std::vector<T> const & vec)
    {
        // returns a vector of the first of each tuple in the input vector
        std::vector<double> res;
        for(T const & val : vec)
        {
            res.push_back(std::get<0>(val));
        }
        return res;
    }

#if 0
    template<>
    std::vector<double> get_first_val(std::vector<OpticsParser::WLData> const & wl_data)
    {
        std::vector<double> res;

        for(OpticsParser::WLData const & row : wl_data)
        {
            res.push_back(row.wavelength);
        }

        return res;
    }
#endif

    template<>
    std::vector<double> get_first_val(std::vector<wincalc::Wavelength_Data> const & wl_data)
    {
        std::vector<double> res;

        for(wincalc::Wavelength_Data const & row : wl_data)
        {
            res.push_back(row.wavelength);
        }

        return res;
    }

#if 0
    double get_minimum_wavelength(window_standards::Optical_Standard_Method const & method,
                                  OpticsParser::ProductData const & product_data,
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
                result = product_data.measurements[0].wavelength;
            }
        }
        else if(method.min_wavelength.type == window_standards::Wavelength_Boundary_Type::NUMBER)
        {
            result = method.min_wavelength.value;
        }

        return result;
    }
#endif

    double get_minimum_wavelength(window_standards::Optical_Standard_Method const & method,
                                  wincalc::Product_Data_N_Band_Optical const & product_data,
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
                result = product_data.wavelength_data[0].wavelength;
            }
        }
        else if(method.min_wavelength.type == window_standards::Wavelength_Boundary_Type::NUMBER)
        {
            result = method.min_wavelength.value;
        }

        return result;
    }

#if 0
    double get_maximum_wavelength(window_standards::Optical_Standard_Method const & method,
                                  OpticsParser::ProductData const & product_data,
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
                result = product_data.measurements.back().wavelength;
            }
        }
        else if(method.max_wavelength.type == window_standards::Wavelength_Boundary_Type::NUMBER)
        {
            result = method.max_wavelength.value;
        }

        return result;
    }

#endif

    double get_maximum_wavelength(window_standards::Optical_Standard_Method const & method,
                                  wincalc::Product_Data_N_Band_Optical const & product_data,
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
                result = product_data.wavelength_data.back().wavelength;
            }
        }
        else if(method.max_wavelength.type == window_standards::Wavelength_Boundary_Type::NUMBER)
        {
            result = method.max_wavelength.value;
        }

        return result;
    }

#if 0
    FenestrationCommon::CSeries
      get_spectum_values(window_standards::Spectrum const & spectrum,
                         window_standards::Optical_Standard_Method const & method,
                         OpticsParser::ProductData const & product_data)
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
                        result = convert(SpectralAveraging::UVAction(
                          get_first_val(product_data.measurements), spectrum.a, spectrum.b));
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
                        result = convert(
                          SpectralAveraging::Krochmann(get_first_val(product_data.measurements)));
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
                        result = convert(SpectralAveraging::BlackBodySpectrum(
                          get_first_val(product_data.measurements), spectrum.t));
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
#endif

    FenestrationCommon::CSeries
      get_spectum_values(window_standards::Spectrum const & spectrum,
                         window_standards::Optical_Standard_Method const & method,
                         wincalc::Product_Data_N_Band_Optical const & product_data)
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
                        result = convert(SpectralAveraging::UVAction(
                          get_first_val(product_data.wavelength_data), spectrum.a, spectrum.b));
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
                        result = convert(SpectralAveraging::Krochmann(
                          get_first_val(product_data.wavelength_data)));
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
                        result = convert(SpectralAveraging::BlackBodySpectrum(
                          get_first_val(product_data.wavelength_data), spectrum.t));
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


#if 0
    FenestrationCommon::CSeries
      get_spectum_values(window_standards::Spectrum const & spectrum,
                         window_standards::Optical_Standard_Method const & method,
                         std::vector<OpticsParser::ProductData> const & product_data)
    {
        return get_spectum_values(spectrum, method, product_data[0]);
    }
#endif

	FenestrationCommon::CSeries get_spectum_values(
      window_standards::Spectrum const & spectrum,
      window_standards::Optical_Standard_Method const & method,
      std::shared_ptr<wincalc::Product_Data_Optical> const & product_data)
    {        
        if(std::dynamic_pointer_cast<wincalc::Product_Data_N_Band_Optical>(product_data))
        {
            return get_spectum_values(
              spectrum,
              method,
              *std::dynamic_pointer_cast<wincalc::Product_Data_N_Band_Optical>(product_data));
        }
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical>(product_data))
        {
            throw std::runtime_error("Dual band products not yet supported");
        }
        else
        {
            throw std::runtime_error("Unsupported optical data structure.");
        }
        return FenestrationCommon::CSeries();
    }

    FenestrationCommon::CSeries get_spectum_values(
      window_standards::Spectrum const & spectrum,
      window_standards::Optical_Standard_Method const & method,
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data)
    {
        if(product_data.size() > 1)
        {
            for(size_t i = 0; i < product_data.size() - 1; ++i)
            {
                if(static_cast<bool>(
                     std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical>(
                       product_data[i]))
                   != static_cast<bool>(
                     std::dynamic_pointer_cast<wincalc::Product_Data_N_Band_Optical>(
                       product_data[i + 1])))
                {
                    throw std::runtime_error("Not sure how to get spectrum values for systems that "
                                             "are a mix of nband and dual band");
                }
            }
        }
        return get_spectum_values(spectrum, method, product_data[0]);

    }

#if 0
    std::vector<double>
      get_wavelength_set_to_use(window_standards::Optical_Standard_Method const & method,
                                OpticsParser::ProductData const & product_data)
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
                result = get_first_val(product_data.measurements);
            }
        }

        return result;
    }
#endif

#if 0
    std::vector<double>
      get_wavelength_set_to_use(window_standards::Optical_Standard_Method const & method,
                                OpticsParser::ProductData const & product_data)
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
                result = get_first_val(product_data.measurements);
            }
        }

        return result;
    }
#endif

    std::vector<double>
      get_wavelength_set_to_use(window_standards::Optical_Standard_Method const & method,
                                wincalc::Product_Data_N_Band_Optical const & product_data)
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
                result = get_first_val(product_data.wavelength_data);
            }
        }

        return result;
    }

	std::vector<double>
      get_wavelength_set_to_use(window_standards::Optical_Standard_Method const & method,
                                std::shared_ptr<wincalc::Product_Data_Optical> const & product_data)
    {
            if(std::dynamic_pointer_cast<wincalc::Product_Data_N_Band_Optical>(product_data))
            {
                return get_wavelength_set_to_use(                  
                  method,
                  *std::dynamic_pointer_cast<wincalc::Product_Data_N_Band_Optical>(product_data));
            }
            else if(std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical>(
                      product_data))
            {
                throw std::runtime_error("Dual band products not yet supported");
            }
            else
            {
                throw std::runtime_error("Unsupported optical data structure.");
            }

			return std::vector<double>();
		}

#if 0   // removing scattering layers for now
    SingleLayerOptics::CScatteringLayer
      create_scattering_layer(OpticsParser::ProductData const & product_data, Method const & method)
    {
        auto source_spectrum = get_spectum_values(method.source_spectrum, method, product_data);

        auto wavelength_set = get_wavelength_set_to_use(method, product_data);

        auto detector_spectrum = get_spectum_values(method.detector_spectrum, method, product_data);

        std::shared_ptr<std::vector<double>> converted_wavelengths =
          std::make_shared<std::vector<double>>(wavelength_set);

        auto integration_rule = convert(method.integration_rule.type);

        auto measured_wavelength_data = convert(product_data.measurements);
        auto spectral_sample_data =
          SpectralAveraging::CSpectralSampleData::create(measured_wavelength_data);

        std::shared_ptr<SpectralAveraging::CSpectralSample> spectral_sample =
          std::make_shared<SpectralAveraging::CSpectralSample>(
            spectral_sample_data, source_spectrum, integration_rule, method.integration_rule.k);

        if(detector_spectrum.size())
        {
            spectral_sample->setDetectorData(detector_spectrum);
        }

        if(!wavelength_set.empty())
        {
            spectral_sample->setWavelengths(SpectralAveraging::WavelengthSet::Custom,
                                            wavelength_set);
        }

        double min_wavelength = get_minimum_wavelength(method, product_data, source_spectrum);
        double max_wavelength = get_maximum_wavelength(method, product_data, source_spectrum);

        double thickness_meters = product_data.thickness / 1000.0;

        std::shared_ptr<SingleLayerOptics::CMaterialSample> material =
          std::make_shared<SingleLayerOptics::CMaterialSample>(
            spectral_sample,
            thickness_meters,
            FenestrationCommon::MaterialType::Monolithic,
            min_wavelength,
            max_wavelength);

        // having to specify createSpecularLayer here is going to be problematic
        // when we have to deal with other things like venetian, woven, etc...
        auto scattering_layer = SingleLayerOptics::CScatteringLayer::createSpecularLayer(material);
        return scattering_layer;
    }
#endif

#if 0
    std::vector<double>
      wavelength_range_factory(OpticsParser::ProductData const & product_data,
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
            return get_wavelength_set_to_use(method, product_data);
        }
        else
        {
            throw std::runtime_error("Unknown wavelength method.");
        }
    }
#endif

    std::vector<double>
      wavelength_range_factory(wincalc::Product_Data_N_Band_Optical const & product_data,
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
            return get_wavelength_set_to_use(method, product_data);
        }
        else
        {
            throw std::runtime_error("Unknown wavelength method.");
        }
    }

    std::shared_ptr<SingleLayerOptics::SpecularLayer> create_specular_layer(
      wincalc::Product_Data_Dual_Band_Optical const & product_data,
      window_standards::Optical_Standard_Method const & method,
      Spectal_Data_Wavelength_Range_Method const & type,
      int number_visible_bands,
      int number_solar_bands)
    {
        throw std::runtime_error("Dual band not yet supported.");
        return std::shared_ptr<SingleLayerOptics::SpecularLayer>();
    }

    std::shared_ptr<SingleLayerOptics::SpecularLayer>
      create_specular_layer(wincalc::Product_Data_N_Band_Optical const & product_data,
                            window_standards::Optical_Standard_Method const & method,
                            Spectal_Data_Wavelength_Range_Method const & type,
                            int number_visible_bands,
                            int number_solar_bands)
    {
        auto wavelength_set = wavelength_range_factory(
          product_data, method, type, number_visible_bands, number_solar_bands);

        std::shared_ptr<std::vector<double>> converted_wavelengths =
          std::make_shared<std::vector<double>>(wavelength_set);

        auto integration_rule = convert(method.integration_rule.type);

        auto measured_wavelength_data = convert(product_data.wavelength_data);
        auto spectral_sample_data =
          SpectralAveraging::CSpectralSampleData::create(measured_wavelength_data);

        auto source_spectrum = get_spectum_values(method.source_spectrum, method, product_data);

        double min_wavelength = get_minimum_wavelength(method, product_data, source_spectrum);
        double max_wavelength = get_maximum_wavelength(method, product_data, source_spectrum);

        std::shared_ptr<SingleLayerOptics::CMaterial> material =
          SingleLayerOptics::Material::nBandMaterial(spectral_sample_data,
                                                     product_data.thickness_meters,
                                                     FenestrationCommon::MaterialType::Monolithic,
                                                     min_wavelength,
                                                     max_wavelength,
                                                     integration_rule,
                                                     method.integration_rule.k);

        material->setBandWavelengths(wavelength_set);

        auto specular_layer = SingleLayerOptics::SpecularLayer::createLayer(material);
        return specular_layer;
    }

    std::shared_ptr<SingleLayerOptics::SpecularLayer>
      create_specular_layer(std::shared_ptr<wincalc::Product_Data_Optical> const & product_data,
                            window_standards::Optical_Standard_Method const & method,
                            Spectal_Data_Wavelength_Range_Method const & type,
                            int number_visible_bands,
                            int number_solar_bands)
    {
        std::shared_ptr<SingleLayerOptics::SpecularLayer> result;
        if(std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical>(product_data))
        {
            result = create_specular_layer(
              std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical>(product_data),
              method,
              type,
              number_visible_bands,
              number_solar_bands);
        }
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_N_Band_Optical>(product_data))
        {
            result = create_specular_layer(
              std::dynamic_pointer_cast<wincalc::Product_Data_N_Band_Optical>(product_data),
              method,
              type,
              number_visible_bands,
              number_solar_bands);
        }
        else
        {
            throw std::runtime_error("Unsupported optical data format");
        }

        return result;
    }   // namespace wincalc

#if 0
    std::shared_ptr<SingleLayerOptics::SpecularLayer>
      create_specular_layer(OpticsParser::ProductData const & product_data,
                            window_standards::Optical_Standard_Method const & method,
                            Spectal_Data_Wavelength_Range_Method const & type,
                            int number_visible_bands,
                            int number_solar_bands)
    {
        auto wavelength_set = wavelength_range_factory(
          product_data, method, type, number_visible_bands, number_solar_bands);

        std::shared_ptr<std::vector<double>> converted_wavelengths =
          std::make_shared<std::vector<double>>(wavelength_set);

        auto integration_rule = convert(method.integration_rule.type);

        auto measured_wavelength_data = convert(product_data.measurements);
        auto spectral_sample_data =
          SpectralAveraging::CSpectralSampleData::create(measured_wavelength_data);

        auto source_spectrum = get_spectum_values(method.source_spectrum, method, product_data);

        double min_wavelength = get_minimum_wavelength(method, product_data, source_spectrum);
        double max_wavelength = get_maximum_wavelength(method, product_data, source_spectrum);

        double thickness_meters = product_data.thickness / 1000.0;

        std::shared_ptr<SingleLayerOptics::CMaterial> material =
          SingleLayerOptics::Material::nBandMaterial(spectral_sample_data,
                                                     thickness_meters,
                                                     FenestrationCommon::MaterialType::Monolithic,
                                                     min_wavelength,
                                                     max_wavelength,
                                                     integration_rule,
                                                     method.integration_rule.k);

        material->setBandWavelengths(wavelength_set);

        auto specular_layer = SingleLayerOptics::SpecularLayer::createLayer(material);
        return specular_layer;
    }
#endif

#if 0   // removing scattering layers for now
    std::unique_ptr<MultiLayerOptics::CMultiLayerScattered>
      create_multi_layer_scattered(std::vector<OpticsParser::ProductData> const & product_data,
                                   Method const & method)
    {
        std::vector<SingleLayerOptics::CScatteringLayer> layers;
        for(OpticsParser::ProductData const & product : product_data)
        {
            layers.push_back(create_scattering_layer(product, method));
        }

        auto layer = MultiLayerOptics::CMultiLayerScattered::create(layers);

        auto source_spectrum = get_spectum_values(method.source_spectrum, method, product_data);

        layer->setSourceData(source_spectrum);
        return layer;
    }
#endif

#if 0
    std::unique_ptr<MultiLayerOptics::CMultiPaneSpecular>
      create_multi_pane_specular(std::vector<OpticsParser::ProductData> const & product_data,
                                 window_standards::Optical_Standard_Method const & method,
                                 Spectal_Data_Wavelength_Range_Method const & type,
                                 int number_visible_bands,
                                 int number_solar_bands)
    {
        std::vector<std::shared_ptr<SingleLayerOptics::SpecularLayer>> layers;
        for(OpticsParser::ProductData const & product : product_data)
        {
            layers.push_back(create_specular_layer(
              product, method, type, number_visible_bands, number_solar_bands));
        }

        auto source_spectrum = get_spectum_values(method.source_spectrum, method, product_data);

        auto detector_spectrum = get_spectum_values(method.detector_spectrum, method, product_data);

        auto layer =
          MultiLayerOptics::CMultiPaneSpecular::create(layers, source_spectrum, detector_spectrum);

        return layer;
    }
#endif

    std::unique_ptr<MultiLayerOptics::CMultiPaneSpecular>
      create_multi_pane_specular(std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
                                 window_standards::Optical_Standard_Method const & method,
                                 Spectal_Data_Wavelength_Range_Method const & type,
                                 int number_visible_bands,
                                 int number_solar_bands)
    {
        std::vector<std::shared_ptr<SingleLayerOptics::SpecularLayer>> layers;
        for(std::shared_ptr<wincalc::Product_Data_Optical> const & product : product_data)
        {
            layers.push_back(create_specular_layer(
              product, method, type, number_visible_bands, number_solar_bands));
        }

        auto source_spectrum = get_spectum_values(method.source_spectrum, method, product_data);

        auto detector_spectrum = get_spectum_values(method.detector_spectrum, method, product_data);

        auto layer =
          MultiLayerOptics::CMultiPaneSpecular::create(layers, source_spectrum, detector_spectrum);

        return layer;
    }

    Engine_Gap_Info convert(Gap_Data const & data)
    {
        std::map<Gas_Type, Gases::GasDef> type_to_wce_type;
        // Air, Argon, Krypton, Xenon
        type_to_wce_type[Gas_Type::AIR] = Gases::GasDef::Air;
        type_to_wce_type[Gas_Type::ARGON] = Gases::GasDef::Argon;
        type_to_wce_type[Gas_Type::KRYPTON] = Gases::GasDef::Krypton;
        type_to_wce_type[Gas_Type::XENON] = Gases::GasDef::Xenon;

        return Engine_Gap_Info{type_to_wce_type.at(data.gas), data.thickness};
    }

    std::vector<Engine_Gap_Info> convert(std::vector<Gap_Data> const & data)
    {
        std::vector<Engine_Gap_Info> result;
        for(Gap_Data const & d : data)
        {
            result.push_back(convert(d));
        }
        return result;
    }
}   // namespace wincalc
