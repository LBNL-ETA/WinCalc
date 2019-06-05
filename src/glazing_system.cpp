#include <sstream>

#include "glazing_system.h"
#include "thermal_calcs_from_measured_data_source.h"
#include "optical_calcs.h"

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


WCE_Simple_Result Glazing_System::all_method_values(Method_Type const & method_type) const
{
    Method method = get_method(method_type);
    return calc_all(solid_layers, method);
}

WCE_Color_Result Glazing_System::color() const
{
    Method tristim_x = get_method(Method_Type::COLOR_TRISTIMX);
    Method tristim_y = get_method(Method_Type::COLOR_TRISTIMY);
    Method tristim_z = get_method(Method_Type::COLOR_TRISTIMZ);
    return calc_color(solid_layers, tristim_x, tristim_y, tristim_z);
}

Method Glazing_System::get_method(Method_Type const & method_type) const
{
    auto method_itr = standard.methods.find(method_type);
    if(method_itr == standard.methods.end())
    {
        std::map<Method_Type, std::string> method_names = method_type_to_name();
        auto method_name = method_names.find(method_type);
        if(method_name != method_names.end())
        {
            std::stringstream err_msg;
            err_msg << "Standard " << standard.name << " does not include a " << method_name->second
                    << " method";
            throw std::runtime_error(err_msg.str());
        }
        else
        {
            std::stringstream err_msg;
            err_msg << "Unknown method type: "
                    << static_cast<std::underlying_type<Method_Type>::type>(method_type);
            throw std::runtime_error(err_msg.str());
        }
    }
    return method_itr->second;
}
