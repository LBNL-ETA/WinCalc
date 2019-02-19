#include <algorithm>

#include <windows_standards/windows_standard.h>
#include <OpticsParser.hpp>

#include "thermal_calcs_disk.h"
#include "thermal_calcs.h"

Gap_Info convert(Gap_Data const & data)
{
    std::map<std::string, Gas_Choice> name_to_enum;
    // Air, Argon, Krypton, Xenon
    name_to_enum["air"] = Gas_Choice::Air;
    name_to_enum["argon"] = Gas_Choice::Argon;
    name_to_enum["krypton"] = Gas_Choice::Krypton;
    name_to_enum["xenon"] = Gas_Choice::Xenon;

    std::string name = data.name;
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    return Gap_Info{name_to_enum.at(name), data.thickness};
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

std::vector<OpticsParser::ProductData>
read_products(std::vector<std::string> const & product_file_paths)
{
    std::vector<OpticsParser::ProductData> result;
	for(std::string const& path : product_file_paths)
	{
        result.push_back(OpticsParser::parseFile(path));
	}
    return result;
}

struct Data_From_Disk
{
    std::vector<OpticsParser::ProductData> products;
    std::vector<Gap_Info> gaps;
    Standard standard;
};

Data_From_Disk read_from_disk(std::vector<std::string> const & product_file_paths,
              std::vector<Gap_Data> const & gap_values,
              std::string const & standard_path)
{
    std::vector<OpticsParser::ProductData> products = read_products(product_file_paths);
    std::vector<Gap_Info> gaps = convert(gap_values);
    Standard standard = load_standard(standard_path);
    return Data_From_Disk{products, gaps, standard};
}

double calc_u(std::vector<std::string> const & product_file_paths,
              std::vector<Gap_Data> const & gap_values,
              std::string const & standard_path,
              double height,
              double width)
{
    Data_From_Disk data = read_from_disk(product_file_paths, gap_values, standard_path);
    return calc_u_iso15099(data.products, data.gaps, width, height, data.standard);
}

double calc_shgc(std::vector<std::string> const & product_file_paths,
                 std::vector<Gap_Data> const & gap_values,
                 std::string const & standard_path,
                 double height,
                 double width)
{
    Data_From_Disk data = read_from_disk(product_file_paths, gap_values, standard_path);
    auto shgc = calc_shgc_iso15099(data.products, data.gaps, width, height, data.standard);
    return shgc;
}
