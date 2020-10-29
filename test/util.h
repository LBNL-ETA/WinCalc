#pragma once

#include <nlohmann/json.hpp>
#include <wincalc/wincalc.h>

nlohmann::json parse_expected_results(std::string const& testName);
void update_expected_results(std::string const& testName, nlohmann::json const& updatedResults);
void test_optical_results(std::string const& test_name, wincalc::WCE_Optical_Results const& results, bool update = false);