#include "glazing_system.h"
#include "thermal_calcs_from_measured_data_source.h"

Glazing_System::Glazing_System(std::vector<OpticsParser::ProductData> const & solid_layers,
                               std::vector<Gap_Data> const & gap_layers,
                               Standard const & standard,
                               double width,
                               double height) :
    solid_layers(solid_layers),
    gap_layers(gap_layers),
    standard(standard),
    width(width),
	height(height)
{}

Thermal_Result Glazing_System::u() const
{
    return calc_u(solid_layers, gap_layers, standard, width, height);
}

Thermal_Result Glazing_System::shgc() const
{
    return calc_shgc(solid_layers, gap_layers, standard, width, height);
}