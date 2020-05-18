#ifndef WINCALC_GAP_H
#define WINCALC_GAP_H

#include <WCEGases.hpp>
#include <variant>

namespace wincalc
{
    struct Predefined_Gas_Mixture_Component
    {
        Gases::GasDef gas;
        double percent;
    };

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
        Engine_Gap_Info(std::vector<std::variant<Predefined_Gas_Mixture_Component,
                                                 Engine_Gas_Mixture_Component>> const & gases,
                        double thickness);


        std::vector<Engine_Gas_Mixture_Component> gases;
        double thickness;
    };
}   // namespace wincalc
#endif
