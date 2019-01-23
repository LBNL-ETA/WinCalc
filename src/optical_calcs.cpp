
#include <algorithm>
#include <iostream>
#include <sstream>
#include <functional>

#include "optical_calcs.h"
#include "convert.h"
/**/
#include "FenestrationCommon.hpp"
#include "WCESingleLayerOptics.hpp"
#include "WCESpectralAveraging.hpp"
/**/

template<typename T>
std::vector<double> get_first_val(std::vector<T> const & v)
{
    // returns a vector of the first of each tuple in the input vector
    std::vector<double> res;
    std::for_each(v.begin(), v.end(), [&res](T const & p) { res.push_back(std::get<0>(p)); });
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
                              std::shared_ptr<FenestrationCommon::CSeries> const & source_spectrum)
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
            result = source_spectrum->getXArray().front();
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
                              std::shared_ptr<FenestrationCommon::CSeries> const & source_spectrum)
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
            result = source_spectrum->getXArray().back();
        }
        if(method.wavelength_set.type == Wavelength_Set_Type::DATA)
        {
            result = product_data.measurements.back().wavelength;
        }
    }
    else if(method.min_wavelength.type == Wavelength_Boundary_Type::NUMBER)
    {
        result = method.min_wavelength.value;
    }

    return result;
}

#if 0
double get_minimum_wavelength(MethodInStandard const & method)
{
    std::cout << "get_min_wavelength: " << method.min_wavelength << std::endl;
    // NOTE:  One of the options is to get the minimum wavelength
    // from the wavelength set.  Wavelength set can rely on other
    // fields that require loading like source spectrum or measured data.
    // So rather than deal with building in orders of operations now I am just
    // calling this after prep_method
    if(method.min_wavelength == "WAVELENGTH SET")
    {
        double val = method.wavelength_set[0];
        std::cout << "Returning the first in wavelength_set: " << val << std::endl;
        return val;
    }
    // The other valid options have already been checked.  So the only thing left is to assume
    // it is a number.  If not then stod will throw and should be handled elsewhere since it
    // indicates something is invalid in the method
    std::cout << "Not WAVELENGTH SET returning sdod of min_wavelength: " << method.min_wavelength
              << std::endl;
    return std::stod(method.min_wavelength);
}

double get_maximum_wavelength(MethodInStandard const & method)
{
    std::cout << "get_max_wavelength: " << method.max_wavelength << std::endl;
    // NOTE:  One of the options is to get the maximum wavelength
    // from the wavelength set.  Wavelength set can rely on other
    // fields that require loading like source spectrum or measured data.
    // So rather than deal with building in orders of operations now I am just
    // calling this after prep_method
    if(method.max_wavelength == "WAVELENGTH SET")
    {
        double val = method.wavelength_set.back();
        std::cout << "Returning the last element in wavelength_set: " << val << std::endl;
        return val;
    }
    // The other valid options have already been checked.  So the only thing left is to assume
    // it is a number.  If not then stod will throw and should be handled elsewhere since it
    // indicates something is invalid in the method
    std::cout << "Not WAVELENGTH SET returning stod of: " << method.max_wavelength << std::endl;
    return std::stod(method.max_wavelength);
}

// TODO: Refactor the below two vector_to_string functions into something clever or already
// existing
std::string vector_to_string(std::vector<std::tuple<double, double, double, double>> const & v)
{
    std::stringstream msg;
    if(v.empty())
    {
        msg << "Empty";
    }
    else
    {
        msg << "Size: " << v.size();
        auto first_elem = v[0];
        msg << " First: (" << std::get<0>(first_elem) << ", " << std::get<1>(first_elem) << ", "
            << std::get<2>(first_elem) << ", " << std::get<3>(first_elem) << ")";
        if(!v.empty())
        {
            auto last_elem = v.back();
            msg << " Last: (" << std::get<0>(last_elem) << ", " << std::get<1>(last_elem) << ", "
                << std::get<2>(last_elem) << ", " << std::get<3>(last_elem) << ")";
        }
    }
    return msg.str();
}

std::string vector_to_string(std::vector<std::pair<double, double>> const & v)
{
    std::stringstream msg;
    if(v.empty())
    {
        msg << "Empty";
    }
    else
    {
        msg << "Size: " << v.size();
        msg << " First: (" << v[0].first << ", " << v[0].second << ")";
        if(!v.empty())
        {
            auto last_elem = v.back();
            msg << " Last: (" << last_elem.first << ", " << last_elem.second << ")";
        }
    }
    return msg.str();
}

std::string vector_to_string(std::vector<double> const & v)
{
    std::stringstream msg;
    if(v.empty())
    {
        msg << "Empty";
    }
    else
    {
        msg << "Size: " << v.size();
        msg << " First: " << v[0];
        if(!v.empty())
        {
            auto last_elem = v.back();
            msg << " Last: " << last_elem;
        }
    }
    return msg.str();
}

std::string method_to_string(MethodInStandard const & method)
{
    std::stringstream msg;
    msg << "Method:" << std::endl;
    msg << "\tsource spectrum name: " << method.source_spectrum_name;
    msg << "\tsource spectrum: " << vector_to_string(method.source_spectrum) << std::endl;
    msg << "\tdetector spectrum name: " << method.detector_spectrum_name;
    msg << "\tdetector spectrum: " << vector_to_string(method.detector_spectrum) << std::endl;
    msg << "\twavelength set name: " << method.wavelength_set_name;
    msg << "\twavelength set: " << vector_to_string(method.wavelength_set) << std::endl;
    msg << "\tintegration rule: " << method.integration_rule;
    msg << "\tintegration_normalization_coefficient: "
        << method.integration_normalization_coefficient;
    msg << "\tmin wavelength: " << method.min_wavelength;
    msg << "\tmax wavelength: " << method.max_wavelength;
    msg << "\tsource a: " << method.source_spectrum_val_a
        << "\tsource b: " << method.source_spectrum_val_b
        << "\tsource temperature: " << method.source_spectrum_temperature << std::endl;
    msg << "\tdetector a: " << method.detector_spectrum_val_a
        << "\tdetector b: " << method.detector_spectrum_val_b
        << "\tdetector temperature: " << method.detector_spectrum_temperature << std::endl;

    return msg.str();
}
#endif
#if 0
MethodInStandard::MethodInStandard(std::string source_spectrum_name,
                                   std::vector<std::pair<double, double>> const & source_spectrum,
                                   std::string detector_spectrum_name,
                                   std::vector<std::pair<double, double>> const & detector_spectrum,
                                   std::string wavelength_set_name,
                                   std::vector<double> const & wavelength_set,
                                   std::string integration_rule,
                                   double integration_normalization_coefficient,
                                   std::string min_wavelength,
                                   std::string max_wavelength,
                                   double source_spectrum_val_a,
                                   double source_spectrum_val_b,
                                   double source_spectrum_temperature,
                                   double detector_spectrum_val_a,
                                   double detector_spectrum_val_b,
                                   double detector_spectrum_temperature) :
    source_spectrum_name(source_spectrum_name),
    source_spectrum(source_spectrum),
    detector_spectrum_name(detector_spectrum_name),
    detector_spectrum(detector_spectrum),
    wavelength_set_name(wavelength_set_name),
    wavelength_set(wavelength_set),
    integration_rule(integration_rule),
    integration_normalization_coefficient(integration_normalization_coefficient),
    min_wavelength(min_wavelength),
    max_wavelength(max_wavelength),
    source_spectrum_val_a(source_spectrum_val_a),
    source_spectrum_val_b(source_spectrum_val_b),
    source_spectrum_temperature(source_spectrum_temperature),
    detector_spectrum_val_a(detector_spectrum_val_a),
    detector_spectrum_val_b(detector_spectrum_val_b),
    detector_spectrum_temperature(detector_spectrum_temperature)
{}
#endif

std::shared_ptr<FenestrationCommon::CSeries>
  get_spectum_values(Spectrum const & spectrum,
                     Wavelength_Set const & wavelength_set,
                     OpticsParser::ProductData const & product_data)
{
    std::shared_ptr<FenestrationCommon::CSeries> result;

    if(spectrum.type == Spectrum_Type::UV_ACTION)
    {
        if(wavelength_set.type == Wavelength_Set_Type::DATA)
        {
            result = convert(SpectralAveraging::UVAction(
              get_first_val(product_data.measurements), spectrum.a, spectrum.b));
        }
        else
        {
            result =
              convert(SpectralAveraging::UVAction(wavelength_set.values, spectrum.a, spectrum.b));
        }
    }
    else if(spectrum.type == Spectrum_Type::KROCHMANN)
    {
        if(wavelength_set.type == Wavelength_Set_Type::DATA)
        {
            result =
              convert(SpectralAveraging::Krochmann(get_first_val(product_data.measurements)));
        }
        else
        {
            result = convert(SpectralAveraging::Krochmann(wavelength_set.values));
        }
    }
    else if(spectrum.type == Spectrum_Type::BLACKBODY)
    {
        if(wavelength_set.type == Wavelength_Set_Type::DATA)
        {
            result = convert(SpectralAveraging::BlackBodySpectrum(
              get_first_val(product_data.measurements), spectrum.t));
        }
        else
        {
            result =
              convert(SpectralAveraging::BlackBodySpectrum(wavelength_set.values, spectrum.t));
        }
    }
    else if(spectrum.type == Spectrum_Type::NONE)
    {
        // if spectrum is none just use blank CSeries
        result = std::make_shared<FenestrationCommon::CSeries>(FenestrationCommon::CSeries());
    }
    else
    {
        throw std::runtime_error("Unknown spectrum type.");
    }

    return result;
}

#if 0
std::vector<std::pair<double, double>>
  name_to_spectrum(std::string const & name,
                   std::vector<double> const & wavelengths,
                   std::vector<std::tuple<double, double, double, double>> const & measured_data,
                   std::string const & wavelength_set_name,
                   double temperature = 0,
                   double a = 3.6,
                   double b = 12)
{
    std::cout << "START name_to_spectrum with name " << name << " and wavelength_set_name "
              << wavelength_set_name << std::endl;
    std::vector<std::pair<double, double>> result;
    if(name == "UV Action")
    {
        if(wavelength_set_name == "Data")
        {
            std::cout << "Returning UVAction(" << vector_to_string(get_first_val(measured_data))
                      << ", " << a << ", " << b << ")" << std::endl;
            result = SpectralAveraging::UVAction(get_first_val(measured_data), a, b);
        }
        else
        {
            std::cout << "Returning UVAction(" << vector_to_string(wavelengths) << ", " << a << ", "
                      << b << ")" << std::endl;
            result = SpectralAveraging::UVAction(wavelengths, a, b);
        }
    }
    else if(name == "Krochmann")
    {
        std::cout << "Found Krochmann for spectrum name" << std::endl;
        if(wavelength_set_name == "Data")
        {
            std::cout << "Returning Krochmann(" << vector_to_string(get_first_val(measured_data))
                      << ")" << std::endl;
            result = SpectralAveraging::Krochmann(get_first_val(measured_data));
        }
        else
        {
            std::cout << "Returning Krochmann(" << vector_to_string(wavelengths) << ")"
                      << std::endl;
            result = SpectralAveraging::Krochmann(wavelengths);
        }
    }
    else if(name == "Blackbody")
    {
        std::cout << "Found Blackbody for spectrum name" << std::endl;
        if(wavelength_set_name == "Data")
        {
            std::cout << "Returning BlackBodySpectrum("
                      << vector_to_string(get_first_val(measured_data)) << ", " << temperature
                      << ")" << std::endl;
            result =
              SpectralAveraging::BlackBodySpectrum(get_first_val(measured_data), temperature);
        }
        else
        {
            std::cout << "Returning BlackBodySpectrum(" << vector_to_string(wavelengths) << ", "
                      << temperature << ")" << std::endl;
            result = SpectralAveraging::BlackBodySpectrum(wavelengths, temperature);
        }
    }
    else
    {
        std::cout << "Unknown spectrum name: " << name << std::endl;
        throw std::runtime_error("Unknown spectrum name: " + name);
    }
    std::cout << vector_to_string(result) << std::endl;
    return result;
};
#endif

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

#if 0
std::vector<double> get_wavelength_set_to_use(
  MethodInStandard const & method,
  std::vector<std::tuple<double, double, double, double>> const & measured_data)
{
    std::cout << "START get_wavelength_set_to_use" << std::endl;
    // if there is anything in wavelength_set give that preference
    if(!method.wavelength_set.empty())
    {
        std::cout << "wavelength_set not empty, returning that "
                  << vector_to_string(method.wavelength_set) << std::endl;
        return method.wavelength_set;
    }

    if(method.wavelength_set_name == "Source")
    {
        std::cout << "wavelength_set_name == Source" << std::endl;
        if(method.source_spectrum.empty())
        {
            std::cout << "Wavelength set to source but no source data loaded" << std::endl;
            throw std::runtime_error("Wavelength set to source but no source data loaded");
        }
        // Get the wavelengths from the source curve
        std::cout << "END get_wavelength_set_to_use returning "
                  << vector_to_string(get_first_val(method.source_spectrum)) << std::endl;
        return get_first_val(method.source_spectrum);
    }
    if(method.wavelength_set_name == "Data")
    {
        // Get the wavelengths from the measured data
        std::cout << "END get_wavelength_set_to_use wavelength_set_name == Data returning "
                  << vector_to_string(get_first_val(measured_data)) << std::endl;
        return get_first_val(measured_data);
    }
    if(method.wavelength_set_name == "None" || method.wavelength_set_name == "")
    {
        std::cout << "END get_wavelength_set_to_use wavelength_set_name == None or empty "
                     "string returning empty array"
                  << std::endl;
        return std::vector<double>();
    }

    throw std::runtime_error("Unknown wavelength set name: " + method.wavelength_set_name);
    std::cout << "END get_wavelength_set_to_use" << std::endl;
}
#endif

#if 0
struct Spectrum_Values_For_WCE
{
    std::shared_ptr<FenestrationCommon::CSeries> source_spectrum;
    std::shared_ptr<FenestrationCommon::CSeries> detector_spectrum;
    std::vector<double> wavelength_set;
};

Values_For_WCE get_values_for_wce(Method const & method,
                                  OpticsParser::ProductData const & product_data)
{
    // Loads in all the data WCE expects, ie makes sure all arrays are populated

    Values_For_WCE result;

	result.source_spectrum =
       get_spectum_values(method.source_spectrum, method.wavelength_set, product_data);

	result.wavelength_set = get_wavelength_set_to_use(method, product_data);

    result.detector_spectrum =
          get_spectum_values(method.detector_spectrum, method.wavelength_set, product_data);

	return result;
#    if 0
    // These next three things need to happen in this order for now.  Will probably need to
    // evolve when more standards are used.
    if(method.source_spectrum.values.empty() && !method.source_spectrum_name.empty())
    {
        method.source_spectrum = name_to_spectrum(method.source_spectrum_name,
                                                  method.wavelength_set,
                                                  measured_data,
                                                  method.wavelength_set_name,
                                                  method.source_spectrum_temperature,
                                                  method.source_spectrum_val_a,
                                                  method.source_spectrum_val_b);
        std::cout << "method.source_spectrum = " << vector_to_string(method.source_spectrum)
                  << std::endl;
    }
    std::cout << "After setting source spectrum method looks like " << std::endl;
    std::cout << method_to_string(method);
    method.wavelength_set = get_wavelength_set_to_use(method, measured_data);
    if(method.detector_spectrum.empty() & !method.detector_spectrum_name.empty())
    {
        method.detector_spectrum = name_to_spectrum(method.detector_spectrum_name,
                                                    method.wavelength_set,
                                                    measured_data,
                                                    method.wavelength_set_name,
                                                    method.detector_spectrum_temperature,
                                                    method.detector_spectrum_val_a,
                                                    method.detector_spectrum_val_b);
        std::cout << "method.detector_spectrum = " << vector_to_string(method.detector_spectrum)
                  << std::endl;
    }

    std::cout << "END prep_method" << std::endl;
    return method;
#    endif
}

#endif
#if 0
MethodInStandard &
  prep_method(MethodInStandard & method,
              std::vector<std::tuple<double, double, double, double>> const & measured_data)
{
    std::cout << "START prep_method.  Measured data " << vector_to_string(measured_data)
              << std::endl;
    std::cout << "Method " << method_to_string(method) << std::endl;

    // Loads in all the data WCE expects, ie makes sure all arrays are populated

    // These next three things need to happen in this order for now.  Will probably need to
    // evolve when more standards are used.
    if(method.source_spectrum.empty() && !method.source_spectrum_name.empty())
    {
        method.source_spectrum = name_to_spectrum(method.source_spectrum_name,
                                                  method.wavelength_set,
                                                  measured_data,
                                                  method.wavelength_set_name,
                                                  method.source_spectrum_temperature,
                                                  method.source_spectrum_val_a,
                                                  method.source_spectrum_val_b);
        std::cout << "method.source_spectrum = " << vector_to_string(method.source_spectrum)
                  << std::endl;
    }
    std::cout << "After setting source spectrum method looks like " << std::endl;
    std::cout << method_to_string(method);
    method.wavelength_set = get_wavelength_set_to_use(method, measured_data);
    if(method.detector_spectrum.empty() & !method.detector_spectrum_name.empty())
    {
        method.detector_spectrum = name_to_spectrum(method.detector_spectrum_name,
                                                    method.wavelength_set,
                                                    measured_data,
                                                    method.wavelength_set_name,
                                                    method.detector_spectrum_temperature,
                                                    method.detector_spectrum_val_a,
                                                    method.detector_spectrum_val_b);
        std::cout << "method.detector_spectrum = " << vector_to_string(method.detector_spectrum)
                  << std::endl;
    }

    std::cout << "END prep_method" << std::endl;
    return method;
}
#endif

#if 0
FenestrationCommon::IntegrationType integration_type_from_string(std::string const & s)
{
    std::map<std::string, FenestrationCommon::IntegrationType> integration_types;
    integration_types["Rectangular"] = FenestrationCommon::IntegrationType::Rectangular;
    integration_types["Trapezoidal"] = FenestrationCommon::IntegrationType::Trapezoidal;

    return integration_types[s];
}
#endif


/*
std::shared_ptr<SingleLayerOptics::CScatteringLayer> create_scattering_layer(
  std::shared_ptr<SpectralAveraging::CSpectralSampleData> const & measured_data,
  double thickness,
  MethodInStandard const & method)
  */
std::shared_ptr<SingleLayerOptics::CScatteringLayer>
  create_scattering_layer(OpticsParser::ProductData const & product_data, Method const & method)
{
    auto source_spectrum =
      get_spectum_values(method.source_spectrum, method.wavelength_set, product_data);

    auto wavelength_set = get_wavelength_set_to_use(method, product_data);

    auto detector_spectrum =
      get_spectum_values(method.detector_spectrum, method.wavelength_set, product_data);

    //    auto converted_source_spectrum = convert_to_cseries(method.source_spectrum);
    //    auto converted_detector_spectrum = convert_to_cseries(method.detector_spectrum);
    std::shared_ptr<std::vector<double>> converted_wavelengths =
      std::make_shared<std::vector<double>>(wavelength_set);

    //	auto integration_rule = integration_type_from_string(method.integration_rule);
    auto integration_rule = convert(method.integration_rule.type);

    auto measured_wavelength_data = convert(product_data.measurements);
    auto spectral_sample_data = SpectralAveraging::CSpectralSampleData::create(measured_wavelength_data);

    std::shared_ptr<SpectralAveraging::CSpectralSample> spectral_sample =
      std::make_shared<SpectralAveraging::CSpectralSample>(
        spectral_sample_data, source_spectrum, integration_rule, method.integration_rule.k);

    if(detector_spectrum->size())
    {
        spectral_sample->setDetectorData(detector_spectrum);
    }

    if(!wavelength_set.empty())
    {
        spectral_sample->setWavelengths(SpectralAveraging::WavelengthSet::Custom, wavelength_set);
    }

    double min_wavelength = get_minimum_wavelength(method, product_data, source_spectrum);
    double max_wavelength = get_maximum_wavelength(method, product_data, source_spectrum);

    std::shared_ptr<SingleLayerOptics::CMaterialSample> material =
      std::make_shared<SingleLayerOptics::CMaterialSample>(
        spectral_sample,
        product_data.thickness,
        FenestrationCommon::MaterialType::Monolithic,
        min_wavelength,
        max_wavelength);

	//having to specify createSpecularLayer here is going to be problematic
	//when we have to deal with other things like venetian, woven, etc...
	auto scattering_layer = SingleLayerOptics::CScatteringLayer::createSpecularLayer(material);
    std::shared_ptr<SingleLayerOptics::CScatteringLayer> layer =
          std::make_shared<SingleLayerOptics::CScatteringLayer>(scattering_layer);

    return layer;
}

#if 0
std::shared_ptr<SingleLayerOptics::CScatteringLayer> create_scattering_layer(
  std::shared_ptr<SpectralAveraging::CSpectralSampleData> const & measured_data,
  double thickness,
  MethodInStandard const & method)
{
    std::cout << "START create_scattering_layer" << std::endl;
    std::cout << "Converting method.source_spectrum" << std::endl;
    auto converted_source_spectrum = convert_to_cseries(method.source_spectrum);
    std::cout << "Converting method.detector_spectrum" << std::endl;
    auto converted_detector_spectrum = convert_to_cseries(method.detector_spectrum);
    std::shared_ptr<std::vector<double>> converted_wavelengths =
      std::make_shared<std::vector<double>>(method.wavelength_set);
    std::cout << "Converted wavelengths in create_scattering_layer "
              << vector_to_string(*converted_wavelengths) << std::endl;
    auto integration_rule = integration_type_from_string(method.integration_rule);
    std::shared_ptr<SpectralAveraging::CSpectralSample> spectral_sample =
      std::make_shared<SpectralAveraging::CSpectralSample>(
        measured_data,
        converted_source_spectrum,
        integration_rule,
        method.integration_normalization_coefficient);
    if(!method.detector_spectrum.empty())
    {
        std::cout << "Setting detector data" << std::endl;
        spectral_sample->setDetectorData(converted_detector_spectrum);
    }

    if(!method.wavelength_set.empty())
    {
        std::cout << "Setting wavelengths" << std::endl;
        spectral_sample->setWavelengths(SpectralAveraging::WavelengthSet::Custom,
                                        converted_wavelengths);
    }

    double min_wavelength = get_minimum_wavelength(method);
    double max_wavelength = get_maximum_wavelength(method);

    std::shared_ptr<SingleLayerOptics::CMaterialSample> material =
      std::make_shared<SingleLayerOptics::CMaterialSample>(
        spectral_sample,
        thickness,
        FenestrationCommon::MaterialType::Monolithic,
        min_wavelength,
        max_wavelength);

    std::cout << "Made material min wavelength" << min_wavelength << " max wavelength "
              << max_wavelength << std::endl;

    std::shared_ptr<SingleLayerOptics::CScatteringLayer> layer =
      std::make_shared<SingleLayerOptics::CScatteringLayer>(material);

    std::cout << "Made layer " << vector_to_string(layer->getWavelengths()) << std::endl;

    std::cout << "END create_scattering_layer" << std::endl;
    return layer;
}
#endif

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

double calc_optical_property(std::shared_ptr<SingleLayerOptics::CScatteringLayer> const & layer,
                             Calculated_Property_Choice property_choice,
                             Side_Choice side_choice,
                             Scattering_Choice scattering_choice)
{
    return layer->getPropertySimple(property_choice, side_choice, scattering_choice);
}

WCEResult calc(OpticsParser::ProductData const & product_data, Method const & method)
{
    auto layer = create_scattering_layer(product_data, method);

    auto calc_f = [&layer](const FenestrationCommon::PropertySimple prop,
                           const FenestrationCommon::Side side,
                           const FenestrationCommon::Scattering scattering) {
        return calc_optical_property(layer, prop, side, scattering);
    };

    return do_calcs<WCEResult, double>(calc_f);
}


ColorResult calc_color_properties(std::shared_ptr<SingleLayerOptics::ColorProperties> color_props,
                                  const FenestrationCommon::PropertySimple prop,
                                  const FenestrationCommon::Side side,
                                  const FenestrationCommon::Scattering scattering,
                                  double theta = 0,
                                  double phi = 0)
{
    auto trichromatic = color_props->getTrichromatic(prop, side, scattering, theta, phi);
    auto rgb = color_props->getRGB(prop, side, scattering, theta, phi);
    auto lab = color_props->getCIE_Lab(prop, side, scattering, theta, phi);
    return ColorResult(trichromatic, rgb, lab);
}

WCEColorResult
  calc_color_properties(std::shared_ptr<SingleLayerOptics::ColorProperties> color_props)
{
    auto calc_f = [&color_props](const FenestrationCommon::PropertySimple prop,
                                 const FenestrationCommon::Side side,
                                 const FenestrationCommon::Scattering scattering) -> ColorResult {
        return calc_color_properties(color_props, prop, side, scattering);
    };

    return do_calcs<WCEColorResult, ColorResult>(calc_f);
}


WCEColorResult calc_color(OpticsParser::ProductData const & product_data,
                          Method const & method_x,
                          Method const & method_y,
                          Method const & method_z)
{
    auto layer_x = create_scattering_layer(product_data, method_x);
    auto layer_y = create_scattering_layer(product_data, method_y);
    auto layer_z = create_scattering_layer(product_data, method_z);

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
                         product_data);   // All methods must have the same source
                                          // spectrum? (Should it be checked above?)

    std::vector<double> wavelength_set =
      get_wavelength_set_to_use(method_x, product_data);   // and the same wavelength set?

    std::shared_ptr<SingleLayerOptics::ColorProperties> color_props =
      std::make_shared<SingleLayerOptics::ColorProperties>(*layer_x,
                                                           *layer_y,
                                                           *layer_z,
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
    auto layer = create_scattering_layer(product_data, method);

	return calc_optical_property(layer, property_choice, side_choice, scattering_choice);
}
