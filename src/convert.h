#ifndef WINCALC_CONVERT_H_
#define WINCALC_CONVERT_H_

#include <vector>
#include <memory>

#include <WCESpectralAveraging.hpp>
#include <WCECommon.hpp>
#include <OpticsParser.hpp>
#include <windows_standards/integration_rule.h>

SpectralAveraging::MeasuredRow convert(OpticsParser::WLData const & data);
std::vector<SpectralAveraging::MeasuredRow> convert(std::vector<OpticsParser::WLData> const & data);
std::shared_ptr<FenestrationCommon::CSeries>
  convert(std::vector<std::pair<double, double>> const & v);
std::shared_ptr<SpectralAveraging::CSpectralSampleData>
  convert(std::vector<std::tuple<double, double, double, double>> const & measured_data);
FenestrationCommon::IntegrationType convert(Integration_Rule_Type integration_rule_type);

#endif
