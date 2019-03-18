#include <algorithm>
#include <locale>
#include <windows_standards/windows_standard.h>
#include <OpticsParser.hpp>

#include "thermal_calcs_from_measured_data_source.h"
#include "thermal_calcs.h"

Gap_Info convert(Gap_Data const & data)
{
    std::map<Gas_Type, Gas_Choice> type_to_wce_type;
    // Air, Argon, Krypton, Xenon
    type_to_wce_type[Gas_Type::AIR] = Gas_Choice::Air;
    type_to_wce_type[Gas_Type::ARGON] = Gas_Choice::Argon;
    type_to_wce_type[Gas_Type::KRYPTON] = Gas_Choice::Krypton;
    type_to_wce_type[Gas_Type::XENON] = Gas_Choice::Xenon;

    return Gap_Info{type_to_wce_type.at(data.gas), data.thickness};
}

std::vector<Gap_Info> convert(std::vector<Gap_Data> const & data)
{
    std::vector<Gap_Info> result;
    for(Gap_Data const & d : data)
    {
        result.push_back(convert(d));
    }
    return result;
}


Thermal_Result calc_u(std::vector<OpticsParser::ProductData> const & products,
                      std::vector<Gap_Data> const & gap_values,
                      Standard const & standard,
                      double height,
                      double width)
{
    std::vector<Gap_Info> gaps = convert(gap_values);
    return calc_u_iso15099(products, gaps, width, height, standard);
}

Thermal_Result calc_shgc(std::vector<OpticsParser::ProductData> const & products,
                         std::vector<Gap_Data> const & gap_values,
                         Standard const & standard,
                         double height,
                         double width)
{
    std::vector<Gap_Info> gaps = convert(gap_values);
    return calc_shgc_iso15099(products, gaps, width, height, standard);    
}
