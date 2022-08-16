#ifndef WINCALC_RESULTS_H_
#define WINCALC_RESULTS_H_

#include <vector>
#include <optional>
#include <WCEMultiLayerOptics.hpp>

namespace wincalc
{
#if 0
    template<typename T>
    struct WCE_Optical_Result_Simple_ORIG
    {
        T direct_direct;
        T direct_diffuse;
        T diffuse_diffuse;
        T direct_hemispherical;
        std::optional<std::vector<std::vector<T>>> matrix;
    };

    template<typename T>
    struct OpticalResultFunctionWrapper
    {
        using Theta = double;
        using Phi = double;

        std::function<T(Theta, Phi)> results_f;
        T operator()(double theta = 0, double = 0)
        {
            return results_f(theta, phi);
        }
    };

    template<typename T>
    struct WCE_Optical_Result_Simple_Angular_Dependent
    {
        OpticalResultFunctionWrapper<T> direct_direct;
        OpticalResultFunctionWrapper<T> direct_diffuse;
        OpticalResultFunctionWrapper<T> direct_hemispherical;
    };

    template<typename T>
    struct WCE_Optical_Result_Simple
    {
        WCE_Optical_Result_Simple_Angular_Dependent<T> angular_dependent;
        T diffuse_diffuse;
        std::optional<std::vector<std::vector<T>>> matrix;
    };

    template<typename T>
    struct WCE_Optical_Result_Absorptance_ORIG
    {
        T total_direct;
        T total_diffuse;
        T heat_direct;
        T heat_diffuse;
        T electricity_direct;
        T electricity_diffuse;
        // T hemispherical;
    };

    template<typename T>
    struct WCE_Optical_Result_Absorptance
    {
        using Theta = double;
        using Phi = double;
        std::function<T(Theta, Phi)> total_direct;
        T total_diffuse;
        std::function<T(Theta, Phi)> heat_direct;
        T heat_diffuse;
        std::function<T(Theta, Phi)> electricity_direct;
        T electricity_diffuse;
        // T hemispherical;
    };

    template<typename T>
    struct WCE_Optical_Result_Layer
    {
        WCE_Optical_Result_Absorptance<T> absorptance;
    };

    template<typename T>
    struct WCE_Optical_Transmission_Result
    {
        T transmittance;
        T reflectance;
    };

    template<typename T>
    struct WCE_Optical_Result_By_Side
    {
        T front;
        T back;
    };
#endif

    template<typename T>
    struct WCE_Optical_Results_System
    {
        std::function<T(FenestrationCommon::PropertySimple, FenestrationCommon::Side, FenestrationCommon::Scattering, double, double)>
          optical_results_f;
        std::optional<std::function<std::vector<std::vector<double>>(
          FenestrationCommon::Side, FenestrationCommon::PropertySimple)>>
          matrix_results_f;
    };

    template<typename T>
    struct WCE_Optical_Results_Layers
    {
        std::function<std::vector<double>(
          FenestrationCommon::Side, FenestrationCommon::ScatteringSimple, double, double)>
          layer_absorptances_total_f;
        std::function<std::vector<double>(
          FenestrationCommon::Side, FenestrationCommon::ScatteringSimple, double, double)>
          layer_absorptances_heat_f;
        std::function<std::vector<double>(
          FenestrationCommon::Side, FenestrationCommon::ScatteringSimple, double, double)>
          layer_absorptances_electricity_f;
    };

#if 0
    template<typename T>
    struct WCE_Optical_Results_Template
    {
        WCE_Optical_Result_By_Side<WCE_Optical_Transmission_Result<WCE_Optical_Result_Simple<T>>>
          system_results;
        std::vector<WCE_Optical_Result_By_Side<WCE_Optical_Result_Layer<T>>> layer_results;
    };
#endif

    template<typename T>
    struct WCE_Optical_Results_Template
    {
        WCE_Optical_Results_System<T> system_results;
        WCE_Optical_Results_Layers<T> layer_results;
    };

    using WCE_Optical_Results = WCE_Optical_Results_Template<double>;

    struct Trichromatic
    {
        Trichromatic() = default;
        template<typename T>
        Trichromatic(T const & other) : X(other.X), Y(other.Y), Z(other.Z)
        {}

        double X;
        double Y;
        double Z;
    };


    struct WinCalc_RGB
    {
        WinCalc_RGB() = default;
        template<typename T>
        WinCalc_RGB(T const & other) : R(other.R), G(other.G), B(other.B)
        {}

        int R;
        int G;
        int B;
    };

    struct Lab
    {
        Lab() = default;
        template<typename T>
        Lab(T const & other) : L(other.L), a(other.a), b(other.b)
        {}

        double L;
        double a;
        double b;
    };

    struct Color_Result
    {
        Color_Result() = default;

        template<typename T, typename R, typename L>
        Color_Result(T const & t, R const & r, L const & l) : trichromatic(t), rgb(r), lab(l)
        {}

        Trichromatic trichromatic;
        WinCalc_RGB rgb;
        Lab lab;
    };

    template<>
    struct WCE_Optical_Results_System<Color_Result>
    {
        std::function<SingleLayerOptics::Trichromatic(
          FenestrationCommon::PropertySimple, FenestrationCommon::Side, FenestrationCommon::Scattering, double, double)>
          trichromatic_results_f;
        std::function<SingleLayerOptics::aRGB(
          FenestrationCommon::PropertySimple, FenestrationCommon::Side, FenestrationCommon::Scattering, double, double)>
          rgb_results_f;
        std::function<SingleLayerOptics::CIE_LAB(
          FenestrationCommon::PropertySimple, FenestrationCommon::Side, FenestrationCommon::Scattering, double, double)>
          lab_results_f;
    };

    template<>
    struct WCE_Optical_Results_Template<Color_Result>
    {
        WCE_Optical_Results_System<Color_Result> system_results;
    };

    using WCE_Color_Results = WCE_Optical_Results_Template<Color_Result>;

}   // namespace wincalc

#endif
