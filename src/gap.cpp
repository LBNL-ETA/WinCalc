#include "gap.h"

wincalc::Pillar::Pillar(double const conductivity):
    conductivity(conductivity)
{}

wincalc::Pillar_Circular::Pillar_Circular(double const conductivity, double const spacing, double const radius):
    Pillar(conductivity),
    spacing(spacing),
    radius(radius)
{}

std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer> wincalc::Pillar_Circular::createGapPillar(
    const std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer> & gap)
{
    return Tarcog::ISO15099::Layers::addCircularPillar(
          gap, conductivity, spacing, radius);
}

wincalc::Engine_Gap_Info::Engine_Gap_Info(Gases::CGasData const & gas,
                                          double thickness,
                                          double pressure,
                                          std::shared_ptr<Pillar> pillar) :
    gases{{gas, 1.0}}, thickness(thickness), pressure(pressure), pillar(pillar)
{}

wincalc::Engine_Gap_Info::Engine_Gap_Info(
  std::vector<wincalc::Engine_Gas_Mixture_Component> const & gases,
  double thickness,
  double pressure,
  std::shared_ptr<Pillar> pillar) :
    gases(gases), thickness(thickness), pressure(pressure), pillar(pillar)
{}

wincalc::Engine_Gap_Info::Engine_Gap_Info(Gases::GasDef const & gas,
                                          double thickness,
                                          double pressure,
                                          std::shared_ptr<Pillar> pillar) :
    gases{{Gases::Gas::intance().get(gas), 1.0}},
    thickness(thickness),
    pressure(pressure),
    pillar(pillar)
{}

wincalc::Engine_Gap_Info::Engine_Gap_Info(
  std::vector<wincalc::Predefined_Gas_Mixture_Component> const & gases,
  double thickness,
  double pressure,
  std::shared_ptr<Pillar> pillar) :
    thickness(thickness), pressure(pressure), pillar(pillar)
{
    for(auto const & gas : gases)
    {
        this->gases.push_back({Gases::Gas::intance().get(gas.gas), gas.percent});
    }
}

wincalc::Engine_Gap_Info::Engine_Gap_Info(
  std::vector<std::variant<wincalc::Predefined_Gas_Mixture_Component,
                           wincalc::Engine_Gas_Mixture_Component>> const & gases,
  double thickness,
  double pressure,
  std::shared_ptr<Pillar> pillar) :
    thickness(thickness), pressure(pressure), pillar(pillar)
{
    for(auto gas : gases)
    {
        wincalc::Engine_Gas_Mixture_Component * gap_layer =
          std::get_if<wincalc::Engine_Gas_Mixture_Component>(&gas);
        if(gap_layer)
        {
            // If the variant was already holding an Engine_Gas_Mixture_Component object use it
            this->gases.push_back(*gap_layer);
        }
        else
        {
            // Otherwise the variant was holding Predefined_Gas_Mixture_Component
            // Convert that and use it
            wincalc::Predefined_Gas_Mixture_Component * predefined_gas =
              std::get_if<wincalc::Predefined_Gas_Mixture_Component>(&gas);
            this->gases.push_back(
              {Gases::Gas::intance().get(predefined_gas->gas), predefined_gas->percent});
        }
    }
}
