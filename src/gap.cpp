#include "gap.h"

#if 0
wincalc::Gap_Data::Gap_Data(Gases::GasDef const & gas, double thickness) :
    gases{{gas, 1.0}},
    thickness(thickness)
{}

wincalc::Gap_Data::Gap_Data(std::vector<Predefined_Gas_Mixture_Component> const & gases,
                            double thickness) :
    gases(gases),
    thickness(thickness)
{}
#endif

wincalc::Engine_Gap_Info::Engine_Gap_Info(Gases::CGasData const & gas, double thickness) :
    gases{{gas, 1.0}},
    thickness(thickness)
{}

wincalc::Engine_Gap_Info::Engine_Gap_Info(std::vector<Engine_Gas_Mixture_Component> const & gases,
                                          double thickness) :
    gases(gases),
    thickness(thickness)
{}

wincalc::Engine_Gap_Info::Engine_Gap_Info(Gases::GasDef const & gas, double thickness) :
    gases{{Gases::Gas::intance().get(gas), 1.0}},
    thickness(thickness)
{}

wincalc::Engine_Gap_Info::Engine_Gap_Info(
  std::vector<Predefined_Gas_Mixture_Component> const & gases, double thickness) :
    thickness(thickness)
{
    for(auto const & gas : gases)
    {
        this->gases.push_back({Gases::Gas::intance().get(gas.gas), gas.percent});
    }
}
