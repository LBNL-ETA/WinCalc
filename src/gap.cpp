#include "gap.h"

wincalc::Gap_Data::Gap_Data(Predefined_Gas_Type const& gas, double thickness) :
    gases({gas, 1.0}),
    thickness(thickness)
{}

wincalc::Gap_Data::Gap_Data(std::vector<Predefined_Gas_Mixture_Component> const& gases, double thickness) :
    gases(gases),
    thickness(thickness)
{}

wincalc::Engine_Gap_Info::Engine_Gap_Info(Gases::CGasData const & gas, double thickness) :
    gases{gas, 1.0},
    thickness(thickness)
{}

wincalc::Engine_Gap_Info::Engine_Gap_Info(std::vector<Engine_Gas_Mixture_Component> const & gases,
                                          double thickness) :
    gases(gases),
    thickness(thickness)
{}
