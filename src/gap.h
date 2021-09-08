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
        Engine_Gap_Info(Gases::CGasData const & gas, double thickness, double pressure = Gases::DefaultPressure);
        Engine_Gap_Info(std::vector<Engine_Gas_Mixture_Component> const & gases,
                        double thickness,
                        double pressure = Gases::DefaultPressure);
        Engine_Gap_Info(Gases::GasDef const & gas, double thickness, double pressure = Gases::DefaultPressure);
        Engine_Gap_Info(std::vector<Predefined_Gas_Mixture_Component> const & gases,
                        double thickness,
                        double pressure = Gases::DefaultPressure);
        Engine_Gap_Info(std::vector<std::variant<Predefined_Gas_Mixture_Component,
                                                 Engine_Gas_Mixture_Component>> const & gases,
                        double thickness,
                        double pressure = Gases::DefaultPressure);


        std::vector<Engine_Gas_Mixture_Component> gases;
        double thickness;
        double pressure;
    };
}   // namespace wincalc
#endif
