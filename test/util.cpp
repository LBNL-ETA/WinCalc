#include "util.h"
#include <fstream>
#include <filesystem>
#include <gtest/gtest.h>
#include "paths.h"

const double TEST_TOLARANCE = 1e-6;

nlohmann::json & get_json_field(nlohmann::json & json, std::string const & field, bool update)
{
    if(update && json.count(field) == 0)
    {
        json[field] = nlohmann::json::object();
    }
    return json.at(field);
}

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
    if(!std::filesystem::exists(path))
    {
        return nlohmann::json::object();
    }
    std::ifstream fin(path);
    std::string content((std::istreambuf_iterator<char>(fin)), (std::istreambuf_iterator<char>()));

    nlohmann::json expected_results_json = nlohmann::json::parse(content);
    return expected_results_json;
}

void update_expected_results(std::string const & test_name, nlohmann::json const & updated_results)
{
    auto path = expected_results_path(test_name);
    auto directory = path.parent_path();
    if(!std::filesystem::exists(directory))
    {
        std::filesystem::create_directories(directory);
    }
    std::ofstream fout(path);
    fout << std::setw(4) << updated_results << std::endl;
}

void compare_vectors(std::vector < double > const& v1, std::vector<double> const& v2)
{
	EXPECT_EQ(v1.size(), v2.size());
	if(v1.size() != v2.size())
	{
		return;
	}
	for(size_t i = 0; i < v1.size(); ++i)
	{
		EXPECT_NEAR(v1[i], v2[i], TEST_TOLARANCE);
	}
}

void test_trichromatic_result(nlohmann::json & expected,
                              wincalc::Trichromatic const & results,
                              bool update)
{
    EXPECT_NEAR(results.X, expected.value("X", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(results.Y, expected.value("Y", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(results.Z, expected.value("Z", -1.0), TEST_TOLARANCE);
    if(update)
    {
        expected["X"] = results.X;
        expected["Y"] = results.Y;
        expected["Z"] = results.Z;
    }
}

void test_lab_result(nlohmann::json & expected, wincalc::Lab const & results, bool update)
{
    EXPECT_NEAR(results.L, expected.value("L", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(results.a, expected.value("a", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(results.b, expected.value("b", -1.0), TEST_TOLARANCE);
    if(update)
    {
        expected["L"] = results.L;
        expected["a"] = results.a;
        expected["b"] = results.b;
    }
}

void test_rgb_result(nlohmann::json & expected, wincalc::WinCalc_RGB const & results, bool update)
{
    EXPECT_NEAR(results.R, expected.value("R", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(results.G, expected.value("G", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(results.B, expected.value("B", -1.0), TEST_TOLARANCE);
    if(update)
    {
        expected["R"] = results.R;
        expected["G"] = results.G;
        expected["B"] = results.B;
    }
}

void test_wce_optical_result_simple(nlohmann::json & expected,
                                    wincalc::WCE_Optical_Result_Simple<double> const & results,
                                    bool update)
{
    EXPECT_NEAR(results.direct_direct, expected.value("direct_direct", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(results.direct_diffuse, expected.value("direct_diffuse", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(
      results.direct_hemispherical, expected.value("direct_hemispherical", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(results.diffuse_diffuse, expected.value("diffuse_diffuse", -1.0), TEST_TOLARANCE);
    if(update)
    {
        expected["direct_direct"] = results.direct_direct;
        expected["direct_diffuse"] = results.direct_diffuse;
        expected["direct_hemispherical"] = results.direct_hemispherical;
        expected["diffuse_diffuse"] = results.diffuse_diffuse;
    }
}

void test_wce_color_result(nlohmann::json & expected,
                           wincalc::Color_Result const & results,
                           bool update)
{
    auto & expected_rgb = get_json_field(expected, "rgb", update);
    auto & expected_lab = get_json_field(expected, "lab", update);
    auto & expected_trichromatic = get_json_field(expected, "trichromatic", update);
    test_rgb_result(expected_rgb, results.rgb, update);
    test_trichromatic_result(expected_trichromatic, results.trichromatic, update);
    test_lab_result(expected_lab, results.lab, update);
}

void test_wce_optical_result_simple(
  nlohmann::json & expected,
  wincalc::WCE_Optical_Result_Simple<wincalc::Color_Result> const & results,
  bool update)
{
    auto & expected_direct_direct = get_json_field(expected, "direct_direct", update);
    auto & expected_direct_diffuse = get_json_field(expected, "direct_diffuse", update);
    auto & expected_direct_hemispheric = get_json_field(expected, "direct_hemispheric", update);
    auto & expected_diffuse_diffuse = get_json_field(expected, "diffuse_diffuse", update);

    test_wce_color_result(expected_direct_direct, results.direct_direct, update);
    test_wce_color_result(expected_direct_diffuse, results.direct_diffuse, update);
    test_wce_color_result(expected_direct_hemispheric, results.direct_hemispherical, update);
    test_wce_color_result(expected_diffuse_diffuse, results.diffuse_diffuse, update);
}


template<typename T>
void test_wce_side_result(
  nlohmann::json & expected,
  wincalc::WCE_Optical_Transmission_Result<wincalc::WCE_Optical_Result_Simple<T>> const & results,
  bool update)
{
    auto & expected_transmittance = get_json_field(expected, "transmittance", update);
    auto & expected_reflectance = get_json_field(expected, "reflectance", update);
    test_wce_optical_result_simple(expected_transmittance, results.transmittance, update);
    test_wce_optical_result_simple(expected_reflectance, results.reflectance, update);
}


void test_wce_absorptances(nlohmann::json & expected,
                           wincalc::WCE_Optical_Result_Absorptance<double> const & results,
                           bool update)
{
    auto expected_direct = expected.value("direct", -1.0);
    auto expected_diffuse = expected.value("diffuse", -1.0);
    EXPECT_NEAR(results.direct, expected_direct, TEST_TOLARANCE);
    EXPECT_NEAR(results.diffuse, expected_diffuse, TEST_TOLARANCE);
    if(update)
    {
        expected["direct"] = results.direct;
        expected["diffuse"] = results.diffuse;
    }
}

template<typename T>
void test_wce_side_result(nlohmann::json & expected,
                          wincalc::WCE_Optical_Result_Layer<T> const & results,
                          bool update)
{
    auto & absorptance = get_json_field(expected, "absorptance", update);
    test_wce_absorptances(absorptance, results.absorptance, update);
}

template<typename T>
void test_wce_optical_result_by_side(nlohmann::json & expected,
                                     wincalc::WCE_Optical_Result_By_Side<T> const & results,
                                     bool update)
{
    auto & front = get_json_field(expected, "front", update);
    auto & back = get_json_field(expected, "back", update);
    test_wce_side_result(front, results.front, update);
    test_wce_side_result(back, results.back, update);
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
        auto & layer_result = get_json_field(expected, "layer " + std::to_string(i), update);
        test_wce_optical_result_by_side(layer_result, results.at(i), update);
    }
}

void test_wce_optical_results_template_double(
  nlohmann::json & expected,
  wincalc::WCE_Optical_Results_Template<double> const & results,
  bool update)
{
    auto & system_results = get_json_field(expected, "system_results", update);
    test_wce_optical_result_by_side(system_results, results.system_results, update);
    auto & layer_results = get_json_field(expected, "layer_results", update);
    test_layer_results(layer_results, results.layer_results, update);
}

void test_wce_optical_results_template_color(
  nlohmann::json & expected,
  wincalc::WCE_Optical_Results_Template<wincalc::Color_Result> const & results,
  bool update)
{
    auto & system_results = get_json_field(expected, "system_results", update);
    test_wce_optical_result_by_side(system_results, results.system_results, update);
}

void test_optical_results(std::string const & test_name,
                          wincalc::WCE_Optical_Results const & results,
                          bool update)
{
    auto expected = parse_expected_results(test_name);
    test_wce_optical_results_template_double(expected, results, update);
    if(update)
    {
        update_expected_results(test_name, expected);
    }
}

void test_optical_results(std::string const & test_name,
                          wincalc::WCE_Color_Results const & results,
                          bool update)
{
    auto expected = parse_expected_results(test_name);
    test_wce_optical_results_template_color(expected, results, update);
    if(update)
    {
        update_expected_results(test_name, expected);
    }
}

void test_optical_results(std::string const & system_name,
                          std::shared_ptr<wincalc::Glazing_System> const & glazing_system,
                          bool update)
{
    auto solar_results = glazing_system->optical_method_results("SOLAR");
    test_optical_results(system_name + "/solar", solar_results, update_results);

    auto photopic_results = glazing_system->optical_method_results("PHOTOPIC");
    test_optical_results(system_name + "/photopic", photopic_results, update_results);

    EXPECT_THROW(glazing_system->optical_method_results("SPF"), std::runtime_error);

    auto tdw_results = glazing_system->optical_method_results("TDW");
    test_optical_results(system_name + "/tdw", tdw_results, update_results);

    auto tkr_results = glazing_system->optical_method_results("TKR");
    test_optical_results(system_name + "/tkr", tdw_results, update_results);

    auto tuv_results = glazing_system->optical_method_results("TUV");
    test_optical_results(system_name + "/tuv", tdw_results, update_results);

    auto color_results = glazing_system->color();
    test_optical_results(system_name + "/color", color_results, update_results);
}

void test_deflection_results(std::string const & results_name,
                             std::shared_ptr<wincalc::Glazing_System> const & glazing_system,
                             bool update)
{
    auto expected = parse_expected_results(results_name);

    auto deflection_results =
      glazing_system->calc_deflection_properties(Tarcog::ISO15099::System::Uvalue);

    auto const & expected_max = expected.value("max_deflection_system_u", std::vector<double>());
    auto const & expected_mean = expected.value("mean_deflection_system_u", std::vector<double>());
    auto const & expected_panes_load = expected.value("panes_load_system_u", std::vector<double>());

    compare_vectors(expected_max, deflection_results.deflection_max);
	compare_vectors(expected_mean, deflection_results.deflection_mean);
	compare_vectors(expected_panes_load, deflection_results.panes_load);

    auto const & expected_layer_temperatures =
      expected.value("layer_temperatures_system_u", std::vector<double>());
    auto temperatures = glazing_system->layer_temperatures(Tarcog::ISO15099::System::Uvalue);
	compare_vectors(expected_layer_temperatures, temperatures);

    if(update)
    {
        expected["max_deflection_system_u"] = deflection_results.deflection_max;
        expected["mean_deflection_system_u"] = deflection_results.deflection_mean;
        expected["panes_load_system_u"] = deflection_results.panes_load;
        expected["layer_temperatures_system_u"] = temperatures;
        update_expected_results(results_name, expected);
    }
}

void test_thermal_results(std::string const & results_name,
                          std::shared_ptr<wincalc::Glazing_System> const & glazing_system,
                          bool update)
{
    auto expected = parse_expected_results(results_name);
    auto u = glazing_system->u();
    auto shgc = glazing_system->shgc();
    auto system_effective_conductivity_u =
      glazing_system->system_effective_conductivity(Tarcog::ISO15099::System::Uvalue);
    auto system_effective_conductivity_shgc =
      glazing_system->system_effective_conductivity(Tarcog::ISO15099::System::SHGC);
    auto relative_heat_gain = glazing_system->relative_heat_gain();

    auto expected_u = expected.value("U", -1.0);
    EXPECT_NEAR(u, expected_u, TEST_TOLARANCE);

    if(expected.count("SHGC"))
    {
        if(expected.at("SHGC").is_null())
        {
            // The json library serializes Nan, inf, and -inf to null
            // Trying to load those values back into a double causes an exception
            // So instead if it is null then check that the calculated SHGC is inf.
            EXPECT_TRUE(isinf(shgc));
        }
        else
        {
            auto expected_shgc = expected.value("SHGC", -1.0);
            EXPECT_NEAR(shgc, expected_shgc, TEST_TOLARANCE);
        }
    }


    EXPECT_NEAR(system_effective_conductivity_u,
                expected.value("system_effective_conductivity_u", -1.0),
                TEST_TOLARANCE);
    EXPECT_NEAR(system_effective_conductivity_shgc,
                expected.value("system_effective_conductivity_shgc", -1.0),
                TEST_TOLARANCE);

    if(expected.count("relative_heat_gain"))
    {
        if(expected.at("relative_heat_gain").is_null())
        {
            // The json library serializes Nan, inf, and -inf to null
            // Trying to load those values back into a double causes an exception
            // So instead if it is null then check that the calculated SHGC is inf.
            EXPECT_TRUE(isinf(relative_heat_gain));
        }
        else
        {
            auto expected_relative_heat_gain = expected.value("relative_heat_gain", -1.0);
            EXPECT_NEAR(relative_heat_gain, expected_relative_heat_gain, TEST_TOLARANCE);
        }
    }
    auto solid_layer_effective_conductivities_u =
      glazing_system->solid_layers_effective_conductivities(Tarcog::ISO15099::System::Uvalue);
    auto solid_layer_effective_conductivities_shgc =
      glazing_system->solid_layers_effective_conductivities(Tarcog::ISO15099::System::SHGC);

    std::vector<double> expected_solid_layer_effective_conductivities_u =
      expected.value("solid_layer_effective_conductivities_u", std::vector<double>());
    std::vector<double> expected_solid_layer_effective_conductivities_shgc =
      expected.value("solid_layer_effective_conductivities_shgc", std::vector<double>());
	compare_vectors(solid_layer_effective_conductivities_u,
              expected_solid_layer_effective_conductivities_u);
	compare_vectors(solid_layer_effective_conductivities_shgc,
              expected_solid_layer_effective_conductivities_shgc);

    auto gap_layer_effective_conductivities_u =
      glazing_system->gap_layers_effective_conductivities(Tarcog::ISO15099::System::Uvalue);
    auto gap_layer_effective_conductivities_shgc =
      glazing_system->gap_layers_effective_conductivities(Tarcog::ISO15099::System::SHGC);

    std::vector<double> expected_gap_layer_effective_conductivities_u =
      expected.value("gap_layer_effective_conductivities_u", std::vector<double>());
    std::vector<double> expected_gap_layer_effective_conductivities_shgc =
      expected.value("gap_layer_effective_conductivities_shgc", std::vector<double>());
	compare_vectors(gap_layer_effective_conductivities_u, expected_gap_layer_effective_conductivities_u);
	compare_vectors(gap_layer_effective_conductivities_shgc,
              expected_gap_layer_effective_conductivities_shgc);

    auto layer_temperatures_u =
      glazing_system->layer_temperatures(Tarcog::ISO15099::System::Uvalue);
    auto layer_temperatures_shgc =
      glazing_system->layer_temperatures(Tarcog::ISO15099::System::SHGC);

    std::vector<double> expected_layer_temperatures_u =
      expected.value("layer_temperatures_u", std::vector<double>());
    std::vector<double> expected_layer_temperatures_shgc =
      expected.value("layer_temperatures_shgc", std::vector<double>());
	compare_vectors(layer_temperatures_u, expected_layer_temperatures_u);
	compare_vectors(layer_temperatures_shgc, expected_layer_temperatures_shgc);


    if(update)
    {
        expected["U"] = u;
        expected["SHGC"] = shgc;
        expected["system_effective_conductivity_u"] = system_effective_conductivity_u;
        expected["system_effective_conductivity_shgc"] = system_effective_conductivity_shgc;
        expected["relative_heat_gain"] = relative_heat_gain;
        expected["solid_layer_effective_conductivities_u"] = solid_layer_effective_conductivities_u;
        expected["solid_layer_effective_conductivities_shgc"] =
          solid_layer_effective_conductivities_shgc;
        expected["gap_layer_effective_conductivities_u"] = gap_layer_effective_conductivities_u;
        expected["gap_layer_effective_conductivities_shgc"] =
          gap_layer_effective_conductivities_shgc;
        expected["layer_temperatures_u"] = layer_temperatures_u;
        expected["layer_temperatures_shgc"] = layer_temperatures_shgc;
        update_expected_results(results_name, expected);
    }
}
