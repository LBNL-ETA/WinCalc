#include "convert.h"

SpectralAveraging::MeasuredRow convert(OpticsParser::WLData const & data)
{
    SpectralAveraging::MeasuredRow converted(data.wavelength, data.T, data.frontR, data.backR);
    return converted;
}

std::vector<SpectralAveraging::MeasuredRow> convert(std::vector<OpticsParser::WLData> const & data)
{
    std::vector<SpectralAveraging::MeasuredRow> converted;

	for(OpticsParser::WLData const& row : data)
	{
        converted.push_back(convert(row));
	}
	return converted;
}

std::shared_ptr<FenestrationCommon::CSeries>convert(std::vector<std::pair<double, double>> const & v)
{
    std::shared_ptr<FenestrationCommon::CSeries> series =
      std::make_shared<FenestrationCommon::CSeries>();
    for(auto val : v)
    {
        series->addProperty(val.first, val.second);
    }
    return series;
}

std::shared_ptr<SpectralAveraging::CSpectralSampleData> convert(std::vector<std::tuple<double, double, double, double>> const & measured_data)
{
    std::shared_ptr<SpectralAveraging::CSpectralSampleData> spectral_data =
      std::make_shared<SpectralAveraging::CSpectralSampleData>();

    for(auto val : measured_data)
    {        
        spectral_data->addRecord(std::get<0>(val), std::get<1>(val), std::get<2>(val), std::get<3>(val));
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