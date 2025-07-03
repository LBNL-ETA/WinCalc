#ifndef WINCALC_RESULTS_H_
#define WINCALC_RESULTS_H_

#include <vector>
#include <optional>

#include <WCECommon.hpp>

namespace wincalc
{
    template<typename T>
    struct WCE_Optical_Result_Simple
    {
        T direct_direct;
        T direct_diffuse;
        T diffuse_diffuse;
        T direct_hemispherical;
        std::optional<std::vector<std::vector<T>>> matrix;
        std::optional<std::vector<FenestrationCommon::SquareMatrix>> wavelength_matrices;
    };

    template<typename T>
    struct WCE_Optical_Result_Absorptance
    {
        T total_direct;
        T total_diffuse;
        T heat_direct;
        T heat_diffuse;
        T electricity_direct;
        T electricity_diffuse;
        std::optional<std::vector<T>> angular_total;
        std::optional<std::vector<T>> angular_heat;
        std::optional<std::vector<T>> angular_electricity;
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

    template<typename T>
    struct WCE_Optical_Results_Template
    {
        WCE_Optical_Result_By_Side<WCE_Optical_Transmission_Result<WCE_Optical_Result_Simple<T>>>
          system_results;
        std::vector<WCE_Optical_Result_By_Side<WCE_Optical_Result_Layer<T>>> layer_results;
        std::optional<WCE_Optical_Result_By_Side<WCE_Optical_Result_Layer<T>>> non_coplanar_attachment_exterior_results;
        std::optional<WCE_Optical_Result_By_Side<WCE_Optical_Result_Layer<T>>> non_coplanar_attachment_interior_results;
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
    struct WCE_Optical_Results_Template<Color_Result>
    {
        WCE_Optical_Result_By_Side<
          WCE_Optical_Transmission_Result<WCE_Optical_Result_Simple<Color_Result>>>
          system_results;
    };

    using WCE_Color_Results = WCE_Optical_Results_Template<Color_Result>;

}   // namespace wincalc

#endif
