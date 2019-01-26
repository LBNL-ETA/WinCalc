#ifndef WINCALC_THERMAL_CALCS_DISK_H_
#define WINCALC_THERMAL_CALCS_DISK_H_

#include <string>
#include <vector>

struct Gap_Data
{
    std::string name;
    double thickness;
};

double calc_u(std::vector<std::string> const& product_file_paths,
			std::vector<Gap_Data> const& gap_values,
			std::string const& standard_path,
			double height,
			double width);

double calc_shgc(std::vector<std::string> const & product_file_paths,
              std::vector<Gap_Data> const & gap_values,
              std::string const & standard_path,
              double height,
              double width);

#endif