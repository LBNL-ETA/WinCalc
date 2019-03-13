#ifndef WINCALC_THERMAL_CALCS_DISK_H_
#define WINCALC_THERMAL_CALCS_DISK_H_

#include <string>
#include <vector>

#include "thermal_results.h"

enum class Gas_Type
{
	AIR, ARGON, KRYPTON, XENON
};

struct Gap_Data
{
    Gas_Type gas;
    double thickness;
};

Thermal_Result calc_u_optics_file(std::vector<std::string> const & product_file_paths,
			std::vector<Gap_Data> const& gap_values,
			std::string const& standard_path,
			double height,
			double width);

Thermal_Result calc_shgc_optics_file(std::vector<std::string> const & product_file_paths,
              std::vector<Gap_Data> const & gap_values,
              std::string const & standard_path,
              double height,
              double width);

Thermal_Result calc_u_json_data(std::vector<std::string> const & product_json,
                                  std::vector<Gap_Data> const & gap_values,
                                  std::string const & standard_path,
                                  double height,
                                  double width);

Thermal_Result calc_shgc_json_data(std::vector<std::string> const & product_json,
                                     std::vector<Gap_Data> const & gap_values,
                                     std::string const & standard_path,
                                     double height,
                                     double width);

#endif