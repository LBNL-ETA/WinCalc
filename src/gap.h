#ifndef WINCALC_GAP_H
#define WINCALC_GAP_H

#include <variant>
#include <WCEGases.hpp>
#include <WCETarcog.hpp>


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

    struct Pillar
    {
        explicit Pillar(double conductivity);

        virtual ~Pillar() = default;
        virtual std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>
          createGapPillar(const std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer> & gap) = 0;
        double conductivity;        
    };

    struct Pillar_Circular : Pillar
    {
        Pillar_Circular(double conductivity, double spacing, double radius);

        std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>
          createGapPillar(const std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer> & gap) override;
        double spacing;
        double radius;
    };

    struct Engine_Gap_Info
    {
        Engine_Gap_Info(Gases::CGasData const & gas,
                        double thickness,
                        double pressure = Gases::DefaultPressure,
                        std::shared_ptr<Pillar> pillar = nullptr);
        Engine_Gap_Info(std::vector<Engine_Gas_Mixture_Component> const & gases,
                        double thickness,
                        double pressure = Gases::DefaultPressure,
                        std::shared_ptr<Pillar> pillar = nullptr);
        Engine_Gap_Info(Gases::GasDef const & gas,
                        double thickness,
                        double pressure = Gases::DefaultPressure,
                        std::shared_ptr<Pillar> pillar = nullptr);
        Engine_Gap_Info(std::vector<Predefined_Gas_Mixture_Component> const & gases,
                        double thickness,
                        double pressure = Gases::DefaultPressure,
                        std::shared_ptr<Pillar> pillar = nullptr);
        Engine_Gap_Info(std::vector<std::variant<Predefined_Gas_Mixture_Component,
                                                 Engine_Gas_Mixture_Component>> const & gases,
                        double thickness,
                        double pressure = Gases::DefaultPressure,
                        std::shared_ptr<Pillar> pillar = nullptr);


        std::vector<Engine_Gas_Mixture_Component> gases;
        double thickness;
        double pressure;
        std::shared_ptr<Pillar> pillar;
    };
}   // namespace wincalc
#endif
