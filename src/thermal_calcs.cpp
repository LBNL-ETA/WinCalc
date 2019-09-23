
#include <algorithm>
#include <iostream>
#include <sstream>

#include <WCEGases.hpp>
#include <WCETarcog.hpp>

#include "thermal_calcs.h"

#include "create_wce_objects.h"

#include "environmental_conditions.h"
#include "convert_optics_parser.h"
#include "util.h"
#include "optical_calcs.h"

namespace wincalc
{
    Tarcog::ISO15099::CSystem create_system(Tarcog::ISO15099::CIGU & igu,
                                            Environments const & environments)
    {
        auto indoor = create_indoor_environment(environments.inside);
        auto outdoor = create_outdoor_environment(environments.outside);
        auto system = Tarcog::ISO15099::CSystem(igu, indoor, outdoor);
        return system;
    }

    double calc_u_iso15099(Tarcog::ISO15099::CIGU & igu, Environments const & environments)
    {
        Tarcog::ISO15099::CSystem system = create_system(igu, environments);
        double u = system.getUValue();
        return u;
    }

    double calc_shgc_iso15099(Tarcog::ISO15099::CIGU & igu,
                              double t_sol,
                              Environments const & environments)
    {
        Tarcog::ISO15099::CSystem system = create_system(igu, environments);
        return system.getSHGC(t_sol);
    }


    Thermal_Result assemble_thermal_result(double result, IGU_Info const & igu_info)
    {
        double t_sol = igu_info.t_sol;
        std::vector<double> layer_solar_absorptances;
        std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUSolidLayer>> solid_layers =
          igu_info.igu.getSolidLayers();
        for(auto const & layer : solid_layers)
        {
            layer_solar_absorptances.push_back(layer->getSolarAbsorptance());
        }
        return Thermal_Result{result, t_sol, layer_solar_absorptances};
    }

    double calc_u_iso15099(
      std::vector<std::shared_ptr<wincalc::Product_Data_Thermal>> const & solid_layers,
      std::vector<double> const & layer_solar_absorptances,
      std::vector<Engine_Gap_Info> const & gaps,
      double width,
      double height,
      Environments const & environments)
    {
        auto igu = create_igu(solid_layers, layer_solar_absorptances, gaps, width, height);
        return calc_u_iso15099(igu, environments);
    }

    double calc_shgc_iso15099(
      std::vector<std::shared_ptr<wincalc::Product_Data_Thermal>> const & solid_layers,
      std::vector<double> const & layer_solar_absorptances,
      std::vector<Engine_Gap_Info> const & gaps,
      double solar_transmittance,
      double width,
      double height,
      Environments const & environments)
    {
        auto igu = create_igu(solid_layers, layer_solar_absorptances, gaps, width, height);
        return calc_shgc_iso15099(igu, solar_transmittance, environments);
    }

	double calc_u(std::vector<std::shared_ptr<wincalc::Product_Data_Thermal>> const & solid_layers,
		std::vector<double> const & layer_solar_absorptances,
		std::vector<Engine_Gap_Info> const & gap_values,
		double width,
		double height,
		Environments const & environments)
	{
            return calc_u_iso15099(
              solid_layers, layer_solar_absorptances, gap_values, width, height, environments);
	}

	double calc_shgc(std::vector<std::shared_ptr<wincalc::Product_Data_Thermal>> const & solid_layers,
		std::vector<double> const & layer_solar_absorptances,
		std::vector<Engine_Gap_Info> const & gap_values,
		double solar_transmittance,
		double width,
		double height,
		Environments const & environments)
	{
            return calc_shgc_iso15099(solid_layers,
                               layer_solar_absorptances,
                               gap_values,
                               solar_transmittance,
                               width,
                               height,
                               environments);
	}

    Thermal_Result calc_u(std::vector<wincalc::Product_Data_Optical_Thermal> const & products,
                          std::vector<Engine_Gap_Info> const & gap_values,
                          window_standards::Optical_Standard const & standard,
                          double width,
                          double height,
                          Environments const & environments,
                          double theta,
                          double phi)
    {
        auto optical_results = optical_results_needed_for_thermal_calcs(
          get_optical_layers(products), standard, theta, phi);

        double result = calc_u(get_thermal_layers(products),
                                   optical_results.layer_solar_absorptances,
                                   gap_values,
                                   width,
                                   height,
                                   environments);

		return Thermal_Result{result,
                              optical_results.total_solar_transmittance,
                              optical_results.layer_solar_absorptances};
    }

    Thermal_Result calc_shgc(std::vector<wincalc::Product_Data_Optical_Thermal> const & products,
                             std::vector<Engine_Gap_Info> const & gap_values,
                             window_standards::Optical_Standard const & standard,
                             double width,
                             double height,
                             Environments const & environments,
                             double theta,
                             double phi)
    {
        auto optical_results = optical_results_needed_for_thermal_calcs(
          get_optical_layers(products), standard, theta, phi);

        double result = calc_shgc(get_thermal_layers(products),
                                         optical_results.layer_solar_absorptances,
                                         gap_values,
                                         optical_results.total_solar_transmittance,
                                         width,
                                         height,
                                         environments);

        return Thermal_Result{result,
                              optical_results.total_solar_transmittance,
                              optical_results.layer_solar_absorptances};
    }

    Thermal_Result calc_u(std::vector<OpticsParser::ProductData> const & products,
                          std::vector<Engine_Gap_Info> const & gap_values,
                          window_standards::Optical_Standard const & standard,
                          double width,
                          double height,
                          Environments const & environments)
    {
        auto converted_layers = convert(products);
        return calc_u(converted_layers, gap_values, standard, width, height, environments);
    }

    Thermal_Result calc_shgc(std::vector<OpticsParser::ProductData> const & products,
                             std::vector<Engine_Gap_Info> const & gap_values,
                             window_standards::Optical_Standard const & standard,
                             double width,
                             double height,
                             Environments const & environments)
    {
        auto converted_layers = convert(products);
        return calc_shgc(converted_layers, gap_values, standard, width, height, environments);
    }
}   // namespace wincalc
