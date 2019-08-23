#include "create_wce_objects.h"

SpectralAveraging::MeasuredRow convert(OpticsParser::WLData const & data)
{
    SpectralAveraging::MeasuredRow converted(data.wavelength, data.T, data.frontR, data.backR);
    return converted;
}

std::vector<SpectralAveraging::MeasuredRow> convert(std::vector<OpticsParser::WLData> const & data)
{
    std::vector<SpectralAveraging::MeasuredRow> converted;

    for(OpticsParser::WLData const & row : data)
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

FenestrationCommon::IntegrationType convert(Integration_Rule_Type integration_rule_type)
{
    std::map<Integration_Rule_Type, FenestrationCommon::IntegrationType> integration_types;
    integration_types[Integration_Rule_Type::RECTANGULAR] =
      FenestrationCommon::IntegrationType::Rectangular;
    integration_types[Integration_Rule_Type::TRAPEZOIDAL] =
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
std::vector<double> get_first_val(std::vector<OpticsParser::WLData> const & wl_data)
{
    std::vector<double> res;

    for(OpticsParser::WLData const & row : wl_data)
    {
        res.push_back(row.wavelength);
    }

    return res;
}

double get_minimum_wavelength(Method const & method,
                              OpticsParser::ProductData const & product_data,
                              FenestrationCommon::CSeries const & source_spectrum)
{
    double result = std::numeric_limits<double>::quiet_NaN();

    if(method.min_wavelength.type == Wavelength_Boundary_Type::WAVELENGTH_SET)
    {
        if(method.wavelength_set.type == Wavelength_Set_Type::FILE)
        {
            result = method.wavelength_set.values[0];
        }
        else if(method.wavelength_set.type == Wavelength_Set_Type::SOURCE)
        {
            result = source_spectrum.getXArray().front();
        }
        if(method.wavelength_set.type == Wavelength_Set_Type::DATA)
        {
            result = product_data.measurements[0].wavelength;
        }
    }
    else if(method.min_wavelength.type == Wavelength_Boundary_Type::NUMBER)
    {
        result = method.min_wavelength.value;
    }

    return result;
}

double get_maximum_wavelength(Method const & method,
                              OpticsParser::ProductData const & product_data,
                              FenestrationCommon::CSeries const & source_spectrum)
{
    double result = std::numeric_limits<double>::quiet_NaN();

    if(method.max_wavelength.type == Wavelength_Boundary_Type::WAVELENGTH_SET)
    {
        if(method.wavelength_set.type == Wavelength_Set_Type::FILE)
        {
            result = method.wavelength_set.values.back();
        }
        else if(method.wavelength_set.type == Wavelength_Set_Type::SOURCE)
        {
            result = source_spectrum.getXArray().back();
        }
        if(method.wavelength_set.type == Wavelength_Set_Type::DATA)
        {
            result = product_data.measurements.back().wavelength;
        }
    }
    else if(method.max_wavelength.type == Wavelength_Boundary_Type::NUMBER)
    {
        result = method.max_wavelength.value;
    }

    return result;
}


FenestrationCommon::CSeries get_spectum_values(Spectrum const & spectrum,
                                               Method const & method,
                                               OpticsParser::ProductData const & product_data)
{
    FenestrationCommon::CSeries result;

    switch(spectrum.type)
    {
        case Spectrum_Type::UV_ACTION:
            switch(method.wavelength_set.type)
            {
                case Wavelength_Set_Type::DATA:
                    // Wavelengths come from the measured data.  Extract first column and pass those
                    result = convert(SpectralAveraging::UVAction(
                      get_first_val(product_data.measurements), spectrum.a, spectrum.b));
                    break;
                case Wavelength_Set_Type::SOURCE:
                    // Wavelengths come from the source spectrum.  Extract first column and pass
                    // those
                    result = convert(SpectralAveraging::UVAction(
                      get_first_val(method.source_spectrum.values), spectrum.a, spectrum.b));
                    break;
                case Wavelength_Set_Type::FILE:
                    // Wavelengths should already be loaded into the wavelength_set
                    result = convert(SpectralAveraging::UVAction(
                      method.wavelength_set.values, spectrum.a, spectrum.b));
                    break;
            }
            break;
        case Spectrum_Type::KROCHMANN:
            switch(method.wavelength_set.type)
            {
                case Wavelength_Set_Type::DATA:
                    // Wavelengths come from the measured data.  Extract first column and pass those
                    result = convert(
                      SpectralAveraging::Krochmann(get_first_val(product_data.measurements)));
                    break;
                case Wavelength_Set_Type::SOURCE:
                    // Wavelengths come from the source spectrum.  Extract first column and pass
                    // those
                    result = convert(
                      SpectralAveraging::Krochmann(get_first_val(method.source_spectrum.values)));
                    break;
                case Wavelength_Set_Type::FILE:
                    // Wavelengths should already be loaded into the wavelength_set
                    result = convert(SpectralAveraging::Krochmann(method.wavelength_set.values));
                    break;
            }
            break;
        case Spectrum_Type::BLACKBODY:
            switch(method.wavelength_set.type)
            {
                case Wavelength_Set_Type::DATA:
                    // Wavelengths come from the measured data.  Extract first column and pass those
                    result = convert(SpectralAveraging::BlackBodySpectrum(
                      get_first_val(product_data.measurements), spectrum.t));
                    break;
                case Wavelength_Set_Type::SOURCE:
                    // Wavelengths come from the source spectrum.  Extract first column and pass
                    // those
                    result = convert(SpectralAveraging::BlackBodySpectrum(
                      get_first_val(method.source_spectrum.values), spectrum.t));
                    break;
                case Wavelength_Set_Type::FILE:
                    // Wavelengths should already be loaded into the wavelength_set
                    result = convert(SpectralAveraging::BlackBodySpectrum(
                      method.wavelength_set.values, spectrum.t));
                    break;
            }
            break;
        case Spectrum_Type::FILE:
            result = convert(spectrum.values);
            break;
        case Spectrum_Type::NONE:
            // if spectrum is none just use empty CSeries
            break;
        default:
            throw std::runtime_error("Unknown spectrum type.");
            break;
    }

    return result;
}

FenestrationCommon::CSeries
  get_spectum_values(Spectrum const & spectrum,
                     Method const & method,
                     std::vector<OpticsParser::ProductData> const & product_data)
{
    return get_spectum_values(spectrum, method, product_data[0]);
}


std::vector<double> get_wavelength_set_to_use(Method const & method,
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
        if(method.wavelength_set.type == Wavelength_Set_Type::SOURCE)
        {
            if(method.source_spectrum.values.empty())
            {
                throw std::runtime_error("Wavelength set to source but no source data loaded");
            }
            // Get the wavelengths from the source curve
            result = get_first_val(method.source_spectrum.values);
        }
        else if(method.wavelength_set.type == Wavelength_Set_Type::DATA)
        {
            result = get_first_val(product_data.measurements);
        }
    }

    return result;
}


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
        spectral_sample->setWavelengths(SpectralAveraging::WavelengthSet::Custom, wavelength_set);
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

SingleLayerOptics::SpecularLayer
  create_specular_layer(OpticsParser::ProductData const & product_data, Method const & method)
{
    auto wavelength_set = get_wavelength_set_to_use(method, product_data);

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

    SingleLayerOptics::SpecularLayer specular_layer =
      SingleLayerOptics::SpecularLayer::createLayer(material);
    return specular_layer;
}


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

std::unique_ptr<MultiLayerOptics::CMultiPaneSpecular>
  create_multi_pane_specular(std::vector<OpticsParser::ProductData> const & product_data,
                             Method const & method)
{
    std::vector<SingleLayerOptics::SpecularLayer> layers;
    for(OpticsParser::ProductData const & product : product_data)
    {
        layers.push_back(create_specular_layer(product, method));
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
