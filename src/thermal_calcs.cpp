
#include <algorithm>
#include <iostream>
#include <sstream>

#include <WCEGases.hpp>
#include <WCETarcog.hpp>

#include "create_wce_objects.h"
#include "thermal_calcs.h"


struct Environmental_Conditions
{
    double air_temperature_inside;
    double effective_room_temperature;
    double effectice_room_emissivity;
    double pressure_room;

    double air_temperature_outside;
    double direct_solar_radiation;
    double convection_coefficient;
    double wind_speed_outside;
    double effective_sky_temperature;
    double effective_sky_emissivity;
    double pressure_outside;
};


Environmental_Conditions environmental_conditions_u()
{
    return Environmental_Conditions{
      294.15, 294.15, 1.0, 101325.0, 255.15, 0.0, 26.0, 5.5, 255.15, 1.0, 101325.0};
}

Environmental_Conditions environmental_conditions_shgc()
{
    return Environmental_Conditions{
      297.15, 297.15, 1.0, 101325.0, 305.15, 783.0, 15.0, 2.75, 305.15, 1.0, 101325.0};
}



Tarcog::ISO15099::CSystem create_system(Tarcog::ISO15099::CIGU & igu, Environmental_Conditions const& environment)
{
    std::shared_ptr<Tarcog::ISO15099::CIndoorEnvironment> indoor =
      Tarcog::ISO15099::Environments::indoor(environment.air_temperature_inside, 
											environment.pressure_room);

	std::shared_ptr<Tarcog::ISO15099::COutdoorEnvironment> outdoor =
      Tarcog::ISO15099::Environments::outdoor(environment.air_temperature_outside,
                                              environment.wind_speed_outside,
                                              environment.direct_solar_radiation,
                                              environment.effective_sky_temperature,
                                              Tarcog::ISO15099::SkyModel::AllSpecified,
                                              environment.pressure_outside);

	outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

	return Tarcog::ISO15099::CSystem(igu, indoor, outdoor);
}

double calc_u_iso15099(Tarcog::ISO15099::CIGU & igu)
{
    Tarcog::ISO15099::CSystem system = create_system(igu, environmental_conditions_u());
    return system.getUValue();
}

double calc_shgc_iso15099(Tarcog::ISO15099::CIGU & igu, double t_sol)
{
    Tarcog::ISO15099::CSystem system = create_system(igu, environmental_conditions_shgc());
    return system.getSHGC(t_sol);
}

Tarcog::ISO15099::CIGU
  create_igu(std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUSolidLayer>> const & solid_layers,
             std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> const & gaps,
             double width,
             double height)
{
    Tarcog::ISO15099::CIGU igu(width, height);
    igu.addLayer(solid_layers[0]);
    if(!gaps.empty())
    {
        for(size_t i = 0; i < gaps.size(); ++i)
        {
            // if there are gaps then there should be one more solid layer
            // than gap layer.  So we can just add a gap then a solid layer
            // at the next index until the end of the gaps
            igu.addLayer(gaps[i]);
            igu.addLayer(solid_layers[i + 1]);
        }
    }
    return igu;
}

struct IGU_Info
{
    Tarcog::ISO15099::CIGU igu;
    double t_sol;
};

IGU_Info create_igu(std::vector<OpticsParser::ProductData> const & layers,
                                        std::vector<Gap_Info> const & gaps,
                                        double width,
                                        double height,
                                        Standard const & standard)
{
    auto solar_method = standard.methods.at(Method_Type::SOLAR);
    auto multi_layer_scattering = create_multi_layer_scattered(layers, solar_method);

	double t_sol =
      multi_layer_scattering->getPropertySimple(FenestrationCommon::PropertySimple::T,
                                                FenestrationCommon::Side::Front,
                                                FenestrationCommon::Scattering::DirectDirect);

    std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUSolidLayer>> tarcog_solid_layers;

    for(size_t i = 0; i < layers.size(); ++i)
    {
        double absorbtance = multi_layer_scattering->getAbsorptanceLayer(
          i, FenestrationCommon::Side::Front, FenestrationCommon::ScatteringSimple::Direct, 0, 0);
        double thickness = layers[i].thickness;
        double conductivity = layers[i].conductivity;
        auto layer = Tarcog::ISO15099::Layers::solid(thickness, conductivity);
        layer->setSolarAbsorptance(absorbtance);
        tarcog_solid_layers.push_back(layer);
    }

    std::vector<std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>> tarcog_gaps;
    for(Gap_Info gap_info : gaps)
    {
        tarcog_gaps.push_back(
          Tarcog::ISO15099::Layers::gap(gap_info.thickness, Gases::CGas({{1.0, gap_info.gas}})));
    }

	return IGU_Info{create_igu(tarcog_solid_layers, tarcog_gaps, width, height), t_sol};
	
}

double calc_u_iso15099(std::vector<OpticsParser::ProductData> const & layers,
                       std::vector<Gap_Info> const & gaps,
                       double width,
                       double height,
                       Standard const & standard)
{
    IGU_Info igu_info = create_igu(layers, gaps, width, height, standard);
    return calc_u_iso15099(igu_info.igu);
}

double calc_shgc_iso15099(std::vector<OpticsParser::ProductData> const & layers,
                       std::vector<Gap_Info> const & gaps,
                       double width,
                       double height,
                       Standard const & standard)
{
    IGU_Info igu_info = create_igu(layers, gaps, width, height, standard);
    return calc_shgc_iso15099(igu_info.igu, igu_info.t_sol);
}
