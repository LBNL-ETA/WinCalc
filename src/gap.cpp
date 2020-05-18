#include "gap.h"

wincalc::Engine_Gap_Info::Engine_Gap_Info(Gases::CGasData const & gas, double thickness) :
    gases{{gas, 1.0}}, thickness(thickness)
{}

wincalc::Engine_Gap_Info::Engine_Gap_Info(
  std::vector<wincalc::Engine_Gas_Mixture_Component> const & gases, double thickness) :
    gases(gases), thickness(thickness)
{}

wincalc::Engine_Gap_Info::Engine_Gap_Info(Gases::GasDef const & gas, double thickness) :
    gases{{Gases::Gas::intance().get(gas), 1.0}}, thickness(thickness)
{}

wincalc::Engine_Gap_Info::Engine_Gap_Info(
  std::vector<wincalc::Predefined_Gas_Mixture_Component> const & gases, double thickness) :
    thickness(thickness)
{
    for(auto const & gas : gases)
    {
        this->gases.push_back({Gases::Gas::intance().get(gas.gas), gas.percent});
    }
}

wincalc::Engine_Gap_Info::Engine_Gap_Info(
  std::vector<std::variant<wincalc::Predefined_Gas_Mixture_Component,
                           wincalc::Engine_Gas_Mixture_Component>> const & gases,
  double thickness) :
    thickness(thickness)
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
