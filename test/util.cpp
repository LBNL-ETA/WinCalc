#include "util.h"
#include <math.h>
#include <fstream>
#include <filesystem>
#include <string>
#include <gtest/gtest.h>
#include "paths.h"
#include "thermal_ir.h"

void logMsg(std::string const & msg)
{
    // std::cerr << msg << std::endl;
}

const double TEST_TOLARANCE = 1e-6;

nlohmann::json & get_json_field(nlohmann::json & json, std::string const & field, bool )
{
    if(json.count(field) == 0)
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
#ifdef _WIN32
    // Ensure long path support only on Windows
    return std::filesystem::path(R"(\\?\)") / std::filesystem::absolute(path);
#else
    return path;
#endif
}

#include <cerrno>

nlohmann::json parse_expected_results(std::string const & test_name)
{
    logMsg("start parse_expected_results: " + test_name);
    auto path = expected_results_path(test_name);

    if(!std::filesystem::exists(path))
    {
        logMsg("******************************************************************");
        logMsg("Returning empty object because !std::filesystem::exists(path)");
        logMsg("******************************************************************");
        return nlohmann::json::object();
    }

    std::ifstream fin(path);

    if(fin.eof())
    {
        throw std::runtime_error("fin eof before content");
    }

    std::string content((std::istreambuf_iterator<char>(fin)), (std::istreambuf_iterator<char>()));

    nlohmann::json expected_results_json = nlohmann::json::parse(content);

    if(expected_results_json.is_discarded())
    {
        throw std::runtime_error("expected_results_json.is_discarded()");
    }

    logMsg("end parse_expected_results: " + test_name);
    return expected_results_json;
}


void update_expected_results(std::string const & test_name, nlohmann::json const & updated_results)
{
    logMsg("start update_expected_results: " + test_name);
    auto path = expected_results_path(test_name);
    auto directory = path.parent_path();
    if(!std::filesystem::exists(directory))
    {
        std::filesystem::create_directories(directory);
    }
    std::ofstream fout(path);
    fout << std::setw(4) << updated_results << std::endl;
    logMsg("end update_expected_results: " + test_name);
}

void compare_vectors(std::vector<double> const & v1, std::vector<double> const & v2)
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

void compare_matrices(std::vector<std::vector<double>> const& m1, std::vector<std::vector<double>> const& m2)
{
    EXPECT_EQ(m1.size(), m2.size());
    if (m1.size() != m2.size())
    {
        return;
    }
    for (size_t i = 0; i < m1.size(); ++i)
    {
        compare_vectors(m1[i], m2[i]);
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

void test_dominant_wavelength_purity_result(nlohmann::json & expected,
                                            wincalc::DominantWavelengthPurity const & results,
                                            bool update)
{
    EXPECT_NEAR(results.dominant_wavelength,
                expected.value("dominant_wavelength", -1.0),
                TEST_TOLARANCE);
    EXPECT_NEAR(results.purity, expected.value("purity", -1.0), TEST_TOLARANCE);
    if(update)
    {
        expected["dominant_wavelength"] = results.dominant_wavelength;
        expected["purity"] = results.purity;
    }
}

double get_possible_nan(nlohmann::json & expected, std::string const & field_name)
{
    if(expected.count(field_name) == 0 || expected.at(field_name).is_null())
    {
        return -std::nan("ind");
    }
    else
    {
        return expected.value(field_name, -1.0);
    }
}

void compare_possible_nan(double given,
                          nlohmann::json & expected_json,
                          std::string const & field_name)
{
    auto expected = get_possible_nan(expected_json, field_name);
    if(!isnan(given) || !isnan(expected))
    {
        EXPECT_NEAR(given, expected, TEST_TOLARANCE);
    }
    else
    {
        // if both values are nan then the test passes and nothing needs to be checked
    }
}

void test_wce_optical_result_simple(nlohmann::json & expected,
                                    wincalc::WCE_Optical_Result_Simple<double> const & results,
                                    bool update)
{
    compare_possible_nan(results.direct_direct, expected, "direct_direct");
    compare_possible_nan(results.direct_diffuse, expected, "direct_diffuse");
    compare_possible_nan(results.direct_hemispherical, expected, "direct_hemispherical");
    compare_possible_nan(results.diffuse_diffuse, expected, "diffuse_diffuse");

    auto const& expected_matrix = expected.value("matrix", std::vector<std::vector<double>>());
    if(results.matrix)
    {
        compare_matrices(results.matrix.value(), expected_matrix);
    }
    else
    {
        EXPECT_TRUE(expected_matrix.empty());
    }
    if(update)
    {
        expected["direct_direct"] = results.direct_direct;
        expected["direct_diffuse"] = results.direct_diffuse;
        expected["direct_hemispherical"] = results.direct_hemispherical;
        expected["diffuse_diffuse"] = results.diffuse_diffuse;
        if(results.matrix)
        {
            expected["matrix"] = results.matrix.value();
        }
    }
}

void test_wce_color_result(nlohmann::json & expected,
                           wincalc::Color_Result const & results,
                           bool update)
{
    auto & expected_rgb = get_json_field(expected, "rgb", update);
    auto & expected_lab = get_json_field(expected, "lab", update);
    auto & expected_trichromatic = get_json_field(expected, "trichromatic", update);
    auto & expected_dominant_wavelength_purity =
      get_json_field(expected, "dominant_wavelength_purity", update);
    test_rgb_result(expected_rgb, results.rgb, update);
    test_trichromatic_result(expected_trichromatic, results.trichromatic, update);
    test_lab_result(expected_lab, results.lab, update);
    test_dominant_wavelength_purity_result(
      expected_dominant_wavelength_purity, results.dominant_wavelength_purity, update);
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
    compare_possible_nan(results.total_direct, expected, "total_direct");
    compare_possible_nan(results.total_diffuse, expected, "total_diffuse");
    compare_possible_nan(results.heat_direct, expected, "heat_direct");
    compare_possible_nan(results.heat_diffuse, expected, "heat_diffuse");
    compare_possible_nan(results.electricity_direct, expected, "electricity_direct");
    compare_possible_nan(results.electricity_diffuse, expected, "electricity_diffuse");
    if(update)
    {
        expected["total_direct"] = results.total_direct;
        expected["total_diffuse"] = results.total_diffuse;
        expected["heat_direct"] = results.heat_direct;
        expected["heat_diffuse"] = results.heat_diffuse;
        expected["electricity_direct"] = results.electricity_direct;
        expected["electricity_diffuse"] = results.electricity_diffuse;
    }
}

void test_thermal_ir(nlohmann::json & expected,
                     wincalc::ThermalIRResults const & results,
                     bool update)
{
    compare_possible_nan(
      results.emissivity_front_hemispheric, expected, "emissivity_front_hemispheric");
    compare_possible_nan(
      results.emissivity_back_hemispheric, expected, "emissivity_back_hemispheric");
    compare_possible_nan(
      results.transmittance_front_diffuse_diffuse, expected, "transmittance_front_diffuse_diffuse");
    compare_possible_nan(
      results.transmittance_back_diffuse_diffuse, expected, "transmittance_back_diffuse_diffuse");
    if(update)
    {
        expected["emissivity_front_hemispheric"] = results.emissivity_front_hemispheric;
        expected["emissivity_back_hemispheric"] = results.emissivity_back_hemispheric;
        expected["transmittance_front_diffuse_diffuse"] =
          results.transmittance_front_diffuse_diffuse;
        expected["transmittance_back_diffuse_diffuse"] = results.transmittance_back_diffuse_diffuse;
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
    std::optional<wincalc::WCE_Optical_Result_By_Side<wincalc::WCE_Optical_Result_Layer<T>>> const&
    noncoplanar_attachment_exterior_results,
    std::optional<wincalc::WCE_Optical_Result_By_Side<wincalc::WCE_Optical_Result_Layer<T>>> const&
    noncoplanar_attachment_interior_results,
  bool update)
{
    for(size_t i = 0; i < results.size(); ++i)
    {
        auto & layer_result = get_json_field(expected, "layer " + std::to_string(i), update);
        test_wce_optical_result_by_side(layer_result, results.at(i), update);
    }

    auto & expected_noncoplanar_attachment_exterior_results = get_json_field(expected, "noncoplanar_attachment_exterior", update);
    if (noncoplanar_attachment_exterior_results)
    {
        test_wce_optical_result_by_side(expected_noncoplanar_attachment_exterior_results, noncoplanar_attachment_exterior_results.value(), update);
    }
    else
    {
        EXPECT_TRUE(expected_noncoplanar_attachment_exterior_results.empty());
    }
    auto & expected_noncoplanar_attachment_interior_results = get_json_field(expected, "noncoplanar_attachment_interior", update);
    if (noncoplanar_attachment_interior_results)
    {
        test_wce_optical_result_by_side(expected_noncoplanar_attachment_interior_results, noncoplanar_attachment_interior_results.value(), update);
    }
    else
    {
        EXPECT_TRUE(expected_noncoplanar_attachment_interior_results.empty());
    }
}

void test_wce_optical_results_template_double(
  nlohmann::json & expected,
  wincalc::WCE_Optical_Results_Template<double> const & results,
  bool update)
{
    logMsg("start test_wce_optical_results_template_double");
    auto & system_results = get_json_field(expected, "system_results", update);
    logMsg("after system_results = get_json_field");
    test_wce_optical_result_by_side(system_results, results.system_results, update);
    logMsg("after test_wce_optical_result_by_side");
    auto & layer_results = get_json_field(expected, "layer_results", update);
    logMsg("after layer_results = get_json_field");
    test_layer_results(layer_results, results.layer_results, results.non_coplanar_attachment_exterior_results, results.non_coplanar_attachment_interior_results, update);
    logMsg("end test_wce_optical_results_template_double");
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
    logMsg("start test_optical_results: " + test_name);
    auto expected = parse_expected_results(test_name);
    logMsg("after parse_expected_results: " + test_name);
    test_wce_optical_results_template_double(expected, results, update);
    logMsg("after test_wce_optical_results_template_double: " + test_name);
    if(update)
    {
        update_expected_results(test_name, expected);
    }
    logMsg("end test_optical_results: " + test_name);
}

void test_ir_results(std::string const & test_name,
                     wincalc::ThermalIRResults const & results,
                     bool update)
{
    logMsg("start test_ir_results: " + test_name);
    auto expected = parse_expected_results(test_name);
    logMsg("after parse_expected_results: " + test_name);
    test_thermal_ir(expected, results, update);
    logMsg("after test_thermal_ir: " + test_name);
    if(update)
    {
        update_expected_results(test_name, expected);
    }
    logMsg("end test_ir_results: " + test_name);
}


void test_optical_results(std::string const & test_name,
                          wincalc::WCE_Color_Results const & results,
                          bool update)
{
    logMsg("start color test_optical_results: " + test_name);
    auto expected = parse_expected_results(test_name);
    test_wce_optical_results_template_color(expected, results, update);
    if(update)
    {
        update_expected_results(test_name, expected);
    }
    logMsg("end color test_optical_results: " + test_name);
}

bool isint(double v)
{
    double intpart;
    return modf(v, &intpart) == 0.0;
}

std::string get_angle_txt(double theta, double phi)
{
    std::stringstream s;
    s << std::fixed;
    if(isint(theta) && isint(phi))
    {
        s << std::setprecision(0);
    }
    else
    {
        s << std::setprecision(3);
    }
    s << "theta=" << theta << "_phi=" << phi;
    return s.str();
}

void test_all_optical_results(std::string const & system_name,
                              std::shared_ptr<wincalc::Glazing_System> const & glazing_system,
                              bool update,
                              double theta = 0,
                              double phi = 0)
{
    logMsg("start test_all_optical_results: " + system_name);
    std::string angle = "/" + get_angle_txt(theta, phi);
    logMsg("before optical_method_results SOLAR: " + system_name + angle);
    auto solar_results = glazing_system->optical_method_results("SOLAR", theta, phi);
    test_optical_results(system_name + angle + "/solar", solar_results, update_results);
    logMsg("before optical_method_results PHOTOPIC: " + system_name + angle);
    auto photopic_results = glazing_system->optical_method_results("PHOTOPIC", theta, phi);
    test_optical_results(system_name + angle + "/photopic", photopic_results, update_results);

    logMsg("before optical_method_results SPF: " + system_name + angle);
    EXPECT_THROW(glazing_system->optical_method_results("SPF"), std::runtime_error);

    logMsg("before optical_method_results TDW: " + system_name + angle);
    auto tdw_results = glazing_system->optical_method_results("TDW", theta, phi);
    test_optical_results(system_name + angle + "/tdw", tdw_results, update_results);

    logMsg("before optical_method_results TKR: " + system_name + angle);
    auto tkr_results = glazing_system->optical_method_results("TKR", theta, phi);
    test_optical_results(system_name + angle + "/tkr", tkr_results, update_results);

    logMsg("before optical_method_results TUV: " + system_name + angle);
    auto tuv_results = glazing_system->optical_method_results("TUV", theta, phi);
    test_optical_results(system_name + angle + "/tuv", tuv_results, update_results);
    logMsg("before glazing_system->solid_layers(): " + system_name + angle);
    auto solid_layers = glazing_system->solid_layers();
    for(size_t i = 0; i < solid_layers.size(); ++i)
    {
        logMsg("before calc_thermal_ir(): " + system_name + angle + "thermal ir layer "
               + std::to_string(i));
        auto ir_results =
          calc_thermal_ir(glazing_system->optical_standard(), glazing_system->solid_layers()[i]);
        test_ir_results(system_name + angle + "/thermal ir layer " + std::to_string(i),
                        ir_results,
                        update_results);
    }

    logMsg("before glazing_system->isBSDF(): " + system_name + angle);
    if(!glazing_system->isBSDF())
    {
        logMsg("before glazing_system->color(): " + system_name + angle);
        auto color_results = glazing_system->color(theta, phi);
        test_optical_results(system_name + angle + "/color", color_results, update_results);
    }
    logMsg("end test_all_optical_results: " + system_name);
}

void test_deflection_results(std::string const & results_name,
                             std::shared_ptr<wincalc::Glazing_System> const & glazing_system,
                             Tarcog::ISO15099::System system_type,
                             bool update,
                             double theta = 0,
                             double phi = 0)
{
    logMsg("start test_deflection_results: " + results_name);
    auto expected = parse_expected_results(results_name);

    auto deflection_results = glazing_system->calc_deflection_properties(system_type, theta, phi);

    auto const & expected_max = expected.value("max_deflection_system", std::vector<double>());
    auto const & expected_mean = expected.value("mean_deflection_system", std::vector<double>());
    auto const & expected_panes_load = expected.value("panes_load_system", std::vector<double>());
    auto const & expected_gaps_width_u_max =
      expected.value("deflected_gaps_width_system_max", std::vector<double>());
    auto const & expected_gaps_width_u_mean =
      expected.value("deflected_gaps_width_system_mean", std::vector<double>());

    compare_vectors(expected_max, deflection_results.layer_deflection_max);
    compare_vectors(expected_mean, deflection_results.layer_deflection_mean);
    compare_vectors(expected_panes_load, deflection_results.panes_load);
    compare_vectors(expected_gaps_width_u_max, deflection_results.gap_width_max);
    compare_vectors(expected_gaps_width_u_mean, deflection_results.gap_width_mean);

    auto const & expected_layer_temperatures =
      expected.value("layer_temperatures_system", std::vector<double>());
    auto temperatures = glazing_system->layer_temperatures(system_type, theta, phi);
    compare_vectors(expected_layer_temperatures, temperatures);

    if(update)
    {
        expected["max_deflection_system"] = deflection_results.layer_deflection_max;
        expected["mean_deflection_system"] = deflection_results.layer_deflection_mean;
        expected["panes_load_system"] = deflection_results.panes_load;
        expected["deflected_gaps_width_system_max"] = deflection_results.gap_width_max;
        expected["deflected_gaps_width_system_mean"] = deflection_results.gap_width_mean;
        expected["layer_temperatures_system"] = temperatures;
        update_expected_results(results_name, expected);
    }
    logMsg("end test_deflection_results: " + results_name);
}

void test_thermal_results(std::string const & results_name,
                          std::shared_ptr<wincalc::Glazing_System> const & glazing_system,
                          bool update,
                          double theta = 0,
                          double phi = 0)
{
    logMsg("start test_thermal_results: " + results_name);
    auto expected = parse_expected_results(results_name);
    logMsg("before u: " + results_name);
    auto u = glazing_system->u(theta, phi);
    logMsg("before shgc: " + results_name);
    auto shgc = glazing_system->shgc(theta, phi);
    logMsg("before system_effective_conductivity_u: " + results_name);
    auto system_effective_conductivity_u =
      glazing_system->system_effective_conductivity(Tarcog::ISO15099::System::Uvalue, theta, phi);
    logMsg("before system_effective_conductivity_shgc: " + results_name);
    auto system_effective_conductivity_shgc =
      glazing_system->system_effective_conductivity(Tarcog::ISO15099::System::SHGC, theta, phi);
    logMsg("before relative_heat_gain: " + results_name);
    auto relative_heat_gain = glazing_system->relative_heat_gain(theta, phi);
    logMsg("before heat_flow_u_indoor: " + results_name);
    auto heat_flow_u_indoor = glazing_system->heat_flow(
      Tarcog::ISO15099::System::Uvalue, Tarcog::ISO15099::Environment::Indoor, theta, phi);
    logMsg("before heat_flow_u_outdoor: " + results_name);
    auto heat_flow_u_outdoor = glazing_system->heat_flow(
      Tarcog::ISO15099::System::Uvalue, Tarcog::ISO15099::Environment::Outdoor, theta, phi);
    logMsg("before heat_flow_shgc_indoor: " + results_name);
    auto heat_flow_shgc_indoor = glazing_system->heat_flow(
      Tarcog::ISO15099::System::SHGC, Tarcog::ISO15099::Environment::Indoor, theta, phi);
    logMsg("before heat_flow_shgc_outdoor: " + results_name);
    auto heat_flow_shgc_outdoor = glazing_system->heat_flow(
      Tarcog::ISO15099::System::SHGC, Tarcog::ISO15099::Environment::Outdoor, theta, phi);
    logMsg("before H_u_indoor: " + results_name);
    auto H_u_indoor = glazing_system->h(
      Tarcog::ISO15099::System::Uvalue, Tarcog::ISO15099::Environment::Indoor, theta, phi);
    logMsg("before H_u_outdoor: " + results_name);
    auto H_u_outdoor = glazing_system->h(
      Tarcog::ISO15099::System::Uvalue, Tarcog::ISO15099::Environment::Outdoor, theta, phi);
    logMsg("before H_shgc_indoor: " + results_name);
    auto H_shgc_indoor = glazing_system->h(
      Tarcog::ISO15099::System::SHGC, Tarcog::ISO15099::Environment::Indoor, theta, phi);
    logMsg("before H_shgc_outdoor: " + results_name);
    auto H_shgc_outdoor = glazing_system->h(
      Tarcog::ISO15099::System::SHGC, Tarcog::ISO15099::Environment::Outdoor, theta, phi);
    logMsg("before radiosities_u: " + results_name);
    auto radiosities_u = glazing_system->radiosities(Tarcog::ISO15099::System::Uvalue);
    logMsg("before radiosities_shgc: " + results_name);
    auto radiosities_shgc = glazing_system->radiosities(Tarcog::ISO15099::System::SHGC);
    logMsg("after radiosities_shgc: " + results_name);
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

    EXPECT_NEAR(heat_flow_u_indoor, expected.value("heat_flow_u_indoor", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(heat_flow_u_indoor, expected.value("heat_flow_u_indoor", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(heat_flow_u_outdoor, expected.value("heat_flow_u_outdoor", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(heat_flow_u_outdoor, expected.value("heat_flow_u_outdoor", -1.0), TEST_TOLARANCE);

    EXPECT_NEAR(
      heat_flow_shgc_indoor, expected.value("heat_flow_shgc_indoor", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(
      heat_flow_shgc_indoor, expected.value("heat_flow_shgc_indoor", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(
      heat_flow_shgc_outdoor, expected.value("heat_flow_shgc_outdoor", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(
      heat_flow_shgc_outdoor, expected.value("heat_flow_shgc_outdoor", -1.0), TEST_TOLARANCE);

    EXPECT_NEAR(H_u_indoor, expected.value("H_u_indoor", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(H_u_indoor, expected.value("H_u_indoor", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(H_u_outdoor, expected.value("H_u_outdoor", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(H_u_outdoor, expected.value("H_u_outdoor", -1.0), TEST_TOLARANCE);

    EXPECT_NEAR(H_shgc_indoor, expected.value("H_shgc_indoor", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(H_shgc_indoor, expected.value("H_shgc_indoor", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(H_shgc_outdoor, expected.value("H_shgc_outdoor", -1.0), TEST_TOLARANCE);
    EXPECT_NEAR(H_shgc_outdoor, expected.value("H_shgc_outdoor", -1.0), TEST_TOLARANCE);

    std::vector<double> expected_radiosities_u =
      expected.value("radiosities_u", std::vector<double>());
    std::vector<double> expected_radiosities_shgc =
      expected.value("radiosities_shgc", std::vector<double>());
    compare_vectors(radiosities_u, expected_radiosities_u);
    compare_vectors(radiosities_shgc, expected_radiosities_shgc);

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
      glazing_system->solid_layers_effective_conductivities(
        Tarcog::ISO15099::System::Uvalue, theta, phi);
    auto solid_layer_effective_conductivities_shgc =
      glazing_system->solid_layers_effective_conductivities(
        Tarcog::ISO15099::System::SHGC, theta, phi);

    logMsg("before expected_solid_layer_effective_conductivities_u: " + results_name);
    std::vector<double> expected_solid_layer_effective_conductivities_u =
      expected.value("solid_layer_effective_conductivities_u", std::vector<double>());
    std::vector<double> expected_solid_layer_effective_conductivities_shgc =
      expected.value("solid_layer_effective_conductivities_shgc", std::vector<double>());
    compare_vectors(solid_layer_effective_conductivities_u,
                    expected_solid_layer_effective_conductivities_u);
    compare_vectors(solid_layer_effective_conductivities_shgc,
                    expected_solid_layer_effective_conductivities_shgc);

    logMsg("before gap_layer_effective_conductivities_u: " + results_name);
    auto gap_layer_effective_conductivities_u = glazing_system->gap_layers_effective_conductivities(
      Tarcog::ISO15099::System::Uvalue, theta, phi);
    logMsg("before gap_layer_effective_conductivities_shgc: " + results_name);
    auto gap_layer_effective_conductivities_shgc =
      glazing_system->gap_layers_effective_conductivities(
        Tarcog::ISO15099::System::SHGC, theta, phi);

    std::vector<double> expected_gap_layer_effective_conductivities_u =
      expected.value("gap_layer_effective_conductivities_u", std::vector<double>());
    std::vector<double> expected_gap_layer_effective_conductivities_shgc =
      expected.value("gap_layer_effective_conductivities_shgc", std::vector<double>());
    compare_vectors(gap_layer_effective_conductivities_u,
                    expected_gap_layer_effective_conductivities_u);
    compare_vectors(gap_layer_effective_conductivities_shgc,
                    expected_gap_layer_effective_conductivities_shgc);

    logMsg("before layer_temperatures_u: " + results_name);
    auto layer_temperatures_u =
      glazing_system->layer_temperatures(Tarcog::ISO15099::System::Uvalue, theta, phi);
    logMsg("before layer_temperatures_shgc: " + results_name);
    auto layer_temperatures_shgc =
      glazing_system->layer_temperatures(Tarcog::ISO15099::System::SHGC, theta, phi);
    logMsg("after layer_temperatures_shgc: " + results_name);
    std::vector<double> expected_layer_temperatures_u =
      expected.value("layer_temperatures_u", std::vector<double>());
    std::vector<double> expected_layer_temperatures_shgc =
      expected.value("layer_temperatures_shgc", std::vector<double>());
    compare_vectors(layer_temperatures_u, expected_layer_temperatures_u);
    compare_vectors(layer_temperatures_shgc, expected_layer_temperatures_shgc);
    logMsg("before if(update): " + results_name);

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
        expected["heat_flow_u_indoor"] = heat_flow_u_indoor;
        expected["heat_flow_u_outdoor"] = heat_flow_u_outdoor;
        expected["heat_flow_shgc_indoor"] = heat_flow_shgc_indoor;
        expected["heat_flow_shgc_outdoor"] = heat_flow_shgc_outdoor;
        expected["H_u_indoor"] = H_u_indoor;
        expected["H_u_outdoor"] = H_u_outdoor;
        expected["H_shgc_indoor"] = H_shgc_indoor;
        expected["H_shgc_outdoor"] = H_shgc_outdoor;
        expected["radiosities_u"] = radiosities_u;
        expected["radiosities_shgc"] = radiosities_shgc;

        update_expected_results(results_name, expected);
    }
    logMsg("end test_thermal_results: " + results_name);
}

void test_optical_results(std::string const & system_name,
                          std::shared_ptr<wincalc::Glazing_System> const & glazing_system,
                          bool update)
{
    std::cerr << "TEST OPTICAL RESULTS: " << system_name << std::endl;
    std::cerr << "--------------------------------------------------" << std::endl;

    double theta = 0;
    double phi = 0;

    std::cerr << "theta = " << theta << " and phi = " << phi << std::endl;
    std::cerr << "condensed spectrum" << std::endl;

    glazing_system->set_spectral_data_wavelength_range(
      wincalc::Spectal_Data_Wavelength_Range_Method::CONDENSED);
    test_all_optical_results(
      system_name + "/condensed_spectrum", glazing_system, update, theta, phi);

    std::cerr << "theta = " << theta << " and phi = " << phi << std::endl;
    std::cerr << "full spectrum" << std::endl;
    glazing_system->set_spectral_data_wavelength_range(
      wincalc::Spectal_Data_Wavelength_Range_Method::FULL);
    test_all_optical_results(system_name + "/full_spectrum", glazing_system, update, theta, phi);

    theta = 15;
    phi = 270;

    std::cerr << "theta = " << theta << " and phi = " << phi << std::endl;
    std::cerr << "condensed spectrum" << std::endl;

    glazing_system->set_spectral_data_wavelength_range(
      wincalc::Spectal_Data_Wavelength_Range_Method::CONDENSED);
    test_all_optical_results(
      system_name + "/condensed_spectrum", glazing_system, update, theta, phi);

    std::cerr << "theta = " << theta << " and phi = " << phi << std::endl;
    std::cerr << "full spectrum" << std::endl;
    glazing_system->set_spectral_data_wavelength_range(
      wincalc::Spectal_Data_Wavelength_Range_Method::FULL);
    test_all_optical_results(system_name + "/full_spectrum", glazing_system, update, theta, phi);
    logMsg("end test_optical_results: " + system_name);
}

void test_thermal_results(std::string const & system_name,
                          std::string const & results_name,
                          std::shared_ptr<wincalc::Glazing_System> const & glazing_system,
                          bool update)
{
    std::cerr << "TEST THERMAL RESULTS: " << system_name << std::endl;
    std::cerr << "--------------------------------------------------" << std::endl;

    double theta = 0;
    double phi = 0;

    std::cerr << "theta = " << theta << " and phi = " << phi << std::endl;
    std::cerr << "condensed spectrum" << std::endl;

    std::string angle = get_angle_txt(theta, phi);
    glazing_system->set_spectral_data_wavelength_range(
      wincalc::Spectal_Data_Wavelength_Range_Method::CONDENSED);
    test_thermal_results(system_name + "/condensed_spectrum/" + angle + "/" + results_name,
                         glazing_system,
                         update,
                         theta,
                         phi);

    std::cerr << "theta = " << theta << " and phi = " << phi << std::endl;
    std::cerr << "full spectrum" << std::endl;

    glazing_system->set_spectral_data_wavelength_range(
      wincalc::Spectal_Data_Wavelength_Range_Method::FULL);
    test_thermal_results(system_name + "/full_spectrum/" + angle + "/" + results_name,
                         glazing_system,
                         update,
                         theta,
                         phi);

    theta = 15;
    phi = 270;

    std::cerr << "theta = " << theta << " and phi = " << phi << std::endl;
    std::cerr << "condensed spectrum" << std::endl;

    angle = get_angle_txt(theta, phi);
    glazing_system->set_spectral_data_wavelength_range(
      wincalc::Spectal_Data_Wavelength_Range_Method::CONDENSED);
    test_thermal_results(system_name + "/condensed_spectrum/" + angle + "/" + results_name,
                         glazing_system,
                         update,
                         theta,
                         phi);

    std::cerr << "theta = " << theta << " and phi = " << phi << std::endl;
    std::cerr << "full spectrum" << std::endl;

    glazing_system->set_spectral_data_wavelength_range(
      wincalc::Spectal_Data_Wavelength_Range_Method::FULL);
    test_thermal_results(system_name + "/full_spectrum/" + angle + "/" + results_name,
                         glazing_system,
                         update,
                         theta,
                         phi);
    logMsg("end test_thermal_results: " + system_name);
}

void test_deflection_results(std::string const & system_name,
                             std::string const & results_name,
                             std::shared_ptr<wincalc::Glazing_System> const & glazing_system,
                             Tarcog::ISO15099::System system_type,
                             bool update)
{
    logMsg("start test_deflection_results: " + system_name + "/" + results_name);
    double theta = 0;
    double phi = 0;
    std::string angle = get_angle_txt(theta, phi);
    logMsg("before set_spectral_data_wavelength_range CONDENSED: " + system_name + angle);
    glazing_system->set_spectral_data_wavelength_range(
      wincalc::Spectal_Data_Wavelength_Range_Method::CONDENSED);
    logMsg("before test_deflection_results: " + system_name + angle);
    test_deflection_results(system_name + "/condensed_spectrum/" + angle + "/" + results_name,
                            glazing_system,
                            system_type,
                            update,
                            theta,
                            phi);

    logMsg("before set_spectral_data_wavelength_range FULL: " + system_name + "/" + angle);
    glazing_system->set_spectral_data_wavelength_range(
      wincalc::Spectal_Data_Wavelength_Range_Method::FULL);
    logMsg("before test_deflection_results: " + system_name + "/" + angle);
    test_deflection_results(system_name + "/full_spectrum/" + angle + "/" + results_name,
                            glazing_system,
                            system_type,
                            update,
                            theta,
                            phi);

    theta = 15;
    phi = 270;
    angle = get_angle_txt(theta, phi);
    logMsg("before set_spectral_data_wavelength_range CONDENSED: " + system_name + angle);
    glazing_system->set_spectral_data_wavelength_range(
      wincalc::Spectal_Data_Wavelength_Range_Method::CONDENSED);
    test_deflection_results(system_name + "/condensed_spectrum/" + angle + "/" + results_name,
                            glazing_system,
                            system_type,
                            update,
                            theta,
                            phi);

    logMsg("before set_spectral_data_wavelength_range FULL: " + system_name + angle);
    glazing_system->set_spectral_data_wavelength_range(
      wincalc::Spectal_Data_Wavelength_Range_Method::FULL);
    test_deflection_results(system_name + "/full_spectrum/" + angle + "/" + results_name,
                            glazing_system,
                            system_type,
                            update,
                            theta,
                            phi);
    logMsg("end test_deflection_results: " + system_name);
}
