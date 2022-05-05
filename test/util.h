#pragma once

#include <nlohmann/json.hpp>
#include <wincalc/wincalc.h>

extern bool update_results;

nlohmann::json parse_expected_results(std::string const & testName);
void update_expected_results(std::string const & testName, nlohmann::json const & updatedResults);
void test_optical_results(std::string const & test_name,
                          wincalc::WCE_Optical_Results const & results,
                          bool update = false);
void test_optical_results(std::string const & test_name,
                          wincalc::WCE_Color_Results const & results,
                          bool update = false);

void test_optical_results(std::string const & system_name,
	std::shared_ptr<wincalc::Glazing_System> const & glazing_system,
	bool update = false);
void test_thermal_results(std::string const & system_name, std::string const & results_name,
	std::shared_ptr<wincalc::Glazing_System> const & glazing_system,
	bool update = false);
void test_deflection_results(std::string const & system_name, std::string const & results_name,
	std::shared_ptr<wincalc::Glazing_System> const & glazing_system,
	bool update);