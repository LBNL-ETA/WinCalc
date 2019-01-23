//
// Created by ubuntu-2 on 2/14/18.
//

#ifndef WINCALC_RESULTS_H_
#define WINCALC_RESULTS_H_


template<typename T>
struct WCEResultTemplate
{
    T tf_direct_direct;
    T tb_direct_direct;
    T rf_direct_direct;
    T rb_direct_direct;
    T tf_direct_diffuse;
    T tb_direct_diffuse;
    T rf_direct_diffuse;
    T rb_direct_diffuse;
    /*
    T tf_diffuse_direct;
    T tb_diffuse_direct;
    T rf_diffuse_direct;
    T rb_diffuse_direct;
    */
    T tf_diffuse_diffuse;
    T tb_diffuse_diffuse;
    T rf_diffuse_diffuse;
    T rb_diffuse_diffuse;
};

//typedef WCEResultTemplate<double> WCEResult;
using WCEResult = WCEResultTemplate<double>;

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

struct ColorResult
{
    ColorResult() = default;

    template<typename T, typename R, typename L>
    ColorResult(T const & t, R const & r, L const & l) : trichromatic(t), rgb(r), lab(l)
    {}

    Trichromatic trichromatic;
    RGB rgb;
    Lab lab;
};

typedef WCEResultTemplate<ColorResult> WCEColorResult;


#endif
