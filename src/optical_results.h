
#ifndef WINCALC_RESULTS_H_
#define WINCALC_RESULTS_H_

namespace wincalc
{
#if 0
    template<typename T>
    struct WCE_Optical_Result_By_Transmittance
    {
        T tf;
        T tb;
        T rf;
        T rb;
		std::vector<T> absorptances_front;
		std::vector<T> absorptances_back;
    };


    template<typename T>
    struct WCE_Optical_Result
    {
        WCE_Optical_Result_By_Transmittance<T> direct_direct;
        WCE_Optical_Result_By_Transmittance<T> direct_diffuse;
        WCE_Optical_Result_By_Transmittance<T> diffuse_diffuse;
    };
#endif

    template<typename T>
    struct WCE_Optical_Result_Simple
    {
        T direct_direct;
        T direct_diffuse;
        T diffuse_diffuse;
    };

    template<typename T>
    struct WCE_Optical_Result_Absorptance
    {
        T direct;
        T diffuse;
    };

    template<typename T>
    struct WCE_Optical_Result_By_Side
    {
        WCE_Optical_Result_Simple<T> tf;
        WCE_Optical_Result_Simple<T> tb;
        WCE_Optical_Result_Simple<T> rf;
        WCE_Optical_Result_Simple<T> rb;
        std::vector<WCE_Optical_Result_Absorptance<T>> absorptances_front;
        std::vector<WCE_Optical_Result_Absorptance<T>> absorptances_back;
    };


    using WCE_Optical_Results = WCE_Optical_Result_By_Side<double>;

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
    struct WCE_Optical_Result_By_Side<Color_Result>
    {
        WCE_Optical_Result_Simple<Color_Result> tf;
        WCE_Optical_Result_Simple<Color_Result> tb;
        WCE_Optical_Result_Simple<Color_Result> rf;
        WCE_Optical_Result_Simple<Color_Result> rb;
    };

    using WCE_Color_Results = WCE_Optical_Result_By_Side<Color_Result>;

}   // namespace wincalc

#endif
