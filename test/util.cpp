#include "util.h"
#include <fstream>
#include <filesystem>
#include <gtest/gtest.h>
#include "paths.h"

std::filesystem::path expected_results_path(std::string const & test_name)
{
    std::filesystem::path path(test_dir);
    path /= "expected_results";
    path /= test_name + ".json";
    return path;
}
nlohmann::json parse_expected_results(std::string const & test_name)
{
    auto path = expected_results_path(test_name);
    std::ifstream fin(path);
    std::string content((std::istreambuf_iterator<char>(fin)), (std::istreambuf_iterator<char>()));

    nlohmann::json expected_results_json = nlohmann::json::parse(content);
    return expected_results_json;
}

void update_expected_results(std::string const & test_name, nlohmann::json const & updated_results)
{
    auto path = expected_results_path(test_name);
    std::ofstream fout(path);
    fout << std::setw(4) << updated_results << std::endl;
}

void test_trichromatic_result(nlohmann::json & expected,
	wincalc::Trichromatic const & results,
	bool update)
{
	EXPECT_EQ(results.X, expected.at("X").get<double>());
	EXPECT_EQ(results.Y, expected.at("Y").get<double>());
	EXPECT_EQ(results.Z, expected.at("Z").get<double>());
	if(update)
	{
		expected["X"] = results.X;
		expected["Y"] = results.Y;
		expected["Z"] = results.Z;
	}
}

void test_lab_result(nlohmann::json & expected,
	wincalc::Lab const & results,
	bool update)
{
	EXPECT_EQ(results.L, expected.at("L").get<double>());
	EXPECT_EQ(results.a, expected.at("a").get<double>());
	EXPECT_EQ(results.b, expected.at("b").get<double>());
	if(update)
	{
		expected["L"] = results.L;
		expected["a"] = results.a;
		expected["b"] = results.b;
	}
}

void test_wce_optical_result_simple(nlohmann::json & expected,
                                    wincalc::WCE_Optical_Result_Simple<double> const & results,
                                    bool update)
{
    EXPECT_EQ(results.direct_direct, expected.at("direct_direct").get<double>());
    EXPECT_EQ(results.direct_diffuse, expected.at("direct_diffuse").get<double>());
    EXPECT_EQ(results.direct_hemispherical, expected.at("direct_hemispherical").get<double>());
    EXPECT_EQ(results.diffuse_diffuse, expected.at("diffuse_diffuse").get<double>());
    if(update)
    {
        expected["direct_direct"] = results.direct_direct;
        expected["direct_diffuse"] = results.direct_diffuse;
        expected["direct_hemispherical"] = results.direct_hemispherical;
        expected["diffuse_diffuse"] = results.diffuse_diffuse;
    }
}

template<typename T>
void test_wce_side_result(
  nlohmann::json & expected,
  wincalc::WCE_Optical_Transmission_Result<wincalc::WCE_Optical_Result_Simple<T>> const & results,
  bool update)
{
    test_wce_optical_result_simple(expected.at("transmittance"), results.transmittance, update);
    test_wce_optical_result_simple(expected.at("reflectance"), results.reflectance, update);
}


void test_wce_absorptances(nlohmann::json & expected,
	wincalc::WCE_Optical_Result_Absorptance<double> const & results,
	bool update)
{
	EXPECT_EQ(results.direct, expected.at("direct").get<double>());
	EXPECT_EQ(results.diffuse, expected.at("diffuse").get<double>());
	if(update)
	{
		expected["direct"] = results.direct;
		expected["diffuse"] = results.diffuse;
	}
}

template<typename T>
void test_wce_side_result(
	nlohmann::json & expected,
	wincalc::WCE_Optical_Result_Layer<T> const & results,
	bool update)
{
	test_wce_absorptance_layer(expected.at("absorptance"), results.absorptance, update);
}

template<typename T>
void test_wce_optical_result_by_side(nlohmann::json & expected,
                                     wincalc::WCE_Optical_Result_By_Side<T> const & results,
                                     bool update)
{
	test_wce_side_result(expected.at("front"), results.front, update);
	test_wce_side_result(expected.at("back"), results.back, update);
}


template<typename T>
void test_layer_results(
  nlohmann::json & expected,
  std::vector<wincalc::WCE_Optical_Result_By_Side<wincalc::WCE_Optical_Result_Layer<T>>> const &
    results,
  bool update)
{
    for(size_t i = 0; i < results.size(); ++i)
    {
		test_wce_optical_result_by_side(
          expected.at("layer " + std::to_string(i)), results.at(i), update);
    }
}

void test_wce_optical_results_template_double(
  nlohmann::json & expected,
  wincalc::WCE_Optical_Results_Template<double> const & results,
  bool update)
{
    test_wce_optical_result_by_side(expected.at("system_results"), results.system_results, update);
    test_layer_results(expected.at("layer_results"), results.layer_results, update);
}

void test_optical_results(std::string const & test_name,
                          wincalc::WCE_Optical_Results const& results,
                          bool update)
{
    auto expected = parse_expected_results(test_name);
	test_wce_optical_results_template_double(expected, results, update);
}
