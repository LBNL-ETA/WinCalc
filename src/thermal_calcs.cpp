
#include <algorithm>
#include <iostream>
#include <sstream>

#include <WCEGases.hpp>
#include <WCETarcog.hpp>

#include "create_wce_objects.h"
#include "thermal_calcs.h"
#include "environmental_conditions.h"
#include "convert_optics_parser.h"

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

    Thermal_Result
      calc_u_iso15099(std::vector<wincalc::Product_Data_Optical_Thermal> const & layers,
                                   std::vector<Engine_Gap_Info> const & gaps,
                                   double width,
                                   double height,
                                   window_standards::Optical_Standard const & standard,
                                   Environments const & environments)
    {
        IGU_Info igu_info = create_igu(layers, gaps, width, height, standard);
        double u = calc_u_iso15099(igu_info.igu, environments);
        return assemble_thermal_result(u, igu_info);
    }

    Thermal_Result
      calc_shgc_iso15099(std::vector<wincalc::Product_Data_Optical_Thermal> const & layers,
                                      std::vector<Engine_Gap_Info> const & gaps,
                                      double width,
                                      double height,
                                      window_standards::Optical_Standard const & standard,
                                      Environments const & environments)
    {
        IGU_Info igu_info = create_igu(layers, gaps, width, height, standard);
        double shgc = calc_shgc_iso15099(igu_info.igu, igu_info.t_sol, environments);
        return assemble_thermal_result(shgc, igu_info);
    }

	Thermal_Result calc_u(std::vector<wincalc::Product_Data_Optical_Thermal> const & products,
                          std::vector<Gap_Data> const & gap_values,
                          window_standards::Optical_Standard const & standard,
                          double width,
                          double height,
                          Environments const & environments)
    {
        std::vector<Engine_Gap_Info> gaps = convert(gap_values);
        return calc_u_iso15099(products, gaps, width, height, standard, environments);
    }

    Thermal_Result calc_shgc(std::vector<wincalc::Product_Data_Optical_Thermal> const & products,
                             std::vector<Gap_Data> const & gap_values,
                             window_standards::Optical_Standard const & standard,
                             double width,
                             double height,
                             Environments const & environments)
    {
        std::vector<Engine_Gap_Info> gaps = convert(gap_values);
        return calc_shgc_iso15099(products, gaps, width, height, standard, environments);
    }

    Thermal_Result calc_u(std::vector<OpticsParser::ProductData> const & products,
                          std::vector<Gap_Data> const & gap_values,
                          window_standards::Optical_Standard const & standard,
                          double width,
                          double height,
                          Environments const & environments)
    {
        auto converted_layers = convert(products);
        return calc_u(converted_layers, gap_values, standard, width, height, environments);
    }

    Thermal_Result calc_shgc(std::vector<OpticsParser::ProductData> const & products,
                             std::vector<Gap_Data> const & gap_values,
                             window_standards::Optical_Standard const & standard,
                             double width,
                             double height,
                             Environments const & environments)
    {
        auto converted_layers = convert(products);
        return calc_shgc(converted_layers, gap_values, standard, width, height, environments);
    }
}   // namespace wincalc
