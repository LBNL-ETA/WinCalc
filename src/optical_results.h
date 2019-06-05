//
// Created by ubuntu-2 on 2/14/18.
//

#ifndef WINCALC_RESULTS_H_
#define WINCALC_RESULTS_H_

template<typename T>
struct WCE_Optical_Result_By_Transmittance
{
    T tf;
    T tb;
    T rf;
    T rb;
};


template<typename T>
struct WCE_Optical_Result
{
    WCE_Optical_Result_By_Transmittance<T> direct_direct;
    WCE_Optical_Result_By_Transmittance<T> direct_diffuse;
    WCE_Optical_Result_By_Transmittance<T> diffuse_diffuse;
};

using WCE_Simple_Result = WCE_Optical_Result<double>;

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

struct RGB
{
    RGB() = default;
    template<typename T>
    RGB(T const & other) : R(other.R), G(other.G), B(other.B)
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
    RGB rgb;
    Lab lab;
};

using WCE_Color_Result = WCE_Optical_Result<Color_Result>;


#endif
