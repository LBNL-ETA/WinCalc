#ifndef WINCALC_GAP_H
#define WINCALC_GAP_H

#include <WCEGases.hpp>

namespace wincalc
{
#if 0
    enum class Predefined_Gas_Type
    {
        AIR,
        ARGON,
        KRYPTON,
        XENON
    };
#endif

    struct Predefined_Gas_Mixture_Component
    {
        Gases::GasDef gas;
        double percent;
    };

#if 0
    struct Gap_Data
    {
        Gap_Data(Gases::GasDef const & gas, double thickness);
        Gap_Data(std::vector<Predefined_Gas_Mixture_Component> const& gases, double thickness);
        std::vector<Predefined_Gas_Mixture_Component> gases;
        double thickness;
    };
#endif

    struct Engine_Gas_Mixture_Component
    {
        Gases::CGasData gas;
        double percent;
    };

    struct Engine_Gap_Info
    {
        Engine_Gap_Info(Gases::CGasData const & gas, double thickness);
        Engine_Gap_Info(std::vector<Engine_Gas_Mixture_Component> const & gases, double thickness);
        Engine_Gap_Info(Gases::GasDef const & gas, double thickness);
        Engine_Gap_Info(std::vector<Predefined_Gas_Mixture_Component> const & gases,
                        double thickness);

        std::vector<Engine_Gas_Mixture_Component> gases;
        double thickness;
    };
}   // namespace wincalc
#endif
