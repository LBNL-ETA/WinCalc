#include "create_wce_objects.h"
#include "convert_optics_parser.h"
#include "util.h"

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


    SpectralAveraging::MeasuredRow convert(wincalc::Wavelength_Data const & data)
    {
        SpectralAveraging::MeasuredRow converted(
          data.wavelength, data.transmittance_front, data.reflectance_front, data.reflectance_back);
        return converted;
    }

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


    FenestrationCommon::CSeries
      get_spectum_values(window_standards::Spectrum const & spectrum,
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
    }

    FenestrationCommon::CSeries get_spectum_values(
      window_standards::Spectrum const & spectrum,
      window_standards::Optical_Standard_Method const & method,
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data)
    {
        if(!all_optical_layers_the_same(product_data))
        {
            throw std::runtime_error("Not sure how to get spectrum values for systems that "
                                     "are a mix of nband and dual band");
        }
        return get_spectum_values(spectrum, method, product_data[0]);
    }


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
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical>(product_data))
        {
            throw std::runtime_error("Dual band products not yet supported");
        }
        else
        {
            throw std::runtime_error("Unsupported optical data structure.");
        }
    }

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

#pragma warning(push)
#pragma warning(disable : 4100)
    std::shared_ptr<SingleLayerOptics::SpecularLayer>
      create_specular_layer(wincalc::Product_Data_Dual_Band_Optical const & product_data,
                            window_standards::Optical_Standard_Method const & method,
                            Spectal_Data_Wavelength_Range_Method const & type,
                            int number_visible_bands,
                            int number_solar_bands)
    {
        throw std::runtime_error("Dual band not yet supported.");
    }
#pragma warning(pop)

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
              *std::dynamic_pointer_cast<wincalc::Product_Data_Dual_Band_Optical>(product_data),
              method,
              type,
              number_visible_bands,
              number_solar_bands);
        }
        else if(std::dynamic_pointer_cast<wincalc::Product_Data_N_Band_Optical>(product_data))
        {
            result = create_specular_layer(
              *std::dynamic_pointer_cast<wincalc::Product_Data_N_Band_Optical>(product_data),
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


    std::unique_ptr<MultiLayerOptics::CMultiPaneSpecular> create_multi_pane_specular(
      std::vector<std::shared_ptr<wincalc::Product_Data_Optical>> const & product_data,
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

		auto gas = Gases::Gas::intance().get(type_to_wce_type.at(data.gas));

        return Engine_Gap_Info{gas, data.thickness};
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

    Tarcog::ISO15099::CIGU create_igu(
      std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUSolidLayer>> const & solid_layers,
      std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> const & gaps,
      double width,
      double height)
    {
        Tarcog::ISO15099::CIGU igu(width, height);
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


    IGU_Info create_igu(std::vector<wincalc::Product_Data_Optical_Thermal> const & layers,
                        std::vector<Engine_Gap_Info> const & gaps,
                        double width,
                        double height,
                        window_standards::Optical_Standard const & standard)

    {
        auto solar_method =
          standard.methods.at(window_standards::Optical_Standard_Method_Type::SOLAR);
        auto optical_layers = get_optical_layers(layers);


        auto multi_pane_specular = create_multi_pane_specular(optical_layers, solar_method);

        double t_sol =
          multi_pane_specular->getPropertySimple(FenestrationCommon::PropertySimple::T,
                                                 FenestrationCommon::Side::Front,
                                                 FenestrationCommon::Scattering::DirectDirect,
                                                 0,
                                                 0);

        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUSolidLayer>> tarcog_solid_layers;

        for(size_t i = 0; i < layers.size(); ++i)
        {
            double absorbtance =
              multi_pane_specular->getAbsorptanceLayer(i + 1,
                                                       FenestrationCommon::Side::Front,
                                                       FenestrationCommon::ScatteringSimple::Direct,
                                                       0,
                                                       0);
            double thickness = layers[i].thermal_data->thickness_meters;
            double conductivity = layers[i].thermal_data->conductivity;

            auto layer = Tarcog::ISO15099::Layers::solid(
              thickness,
              conductivity,
              layers[i].thermal_data->emissivity_front.value(),
              layers[i].thermal_data->ir_transmittance_front.value(),
              layers[i].thermal_data->emissivity_back.value(),
              layers[i].thermal_data->ir_transmittance_back.value());
            layer->setSolarAbsorptance(absorbtance);
            tarcog_solid_layers.push_back(layer);
        }

        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> tarcog_gaps;
        for(Engine_Gap_Info Engine_Gap_Info : gaps)
        {
            tarcog_gaps.push_back(Tarcog::ISO15099::Layers::gap(
              Engine_Gap_Info.thickness, Gases::CGas({{1.0, Engine_Gap_Info.gas}})));
        }

        return IGU_Info{create_igu(tarcog_solid_layers, tarcog_gaps, width, height), t_sol};
    }

    IGU_Info create_igu(std::vector<OpticsParser::ProductData> const & layers,
                        std::vector<Engine_Gap_Info> const & gaps,
                        double width,
                        double height,
                        window_standards::Optical_Standard const & standard)
    {
        auto converted_layers = convert(layers);
        return create_igu(converted_layers, gaps, width, height, standard);
    }
}   // namespace wincalc
