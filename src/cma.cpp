#include "cma.h"
#include <optional>
#include <map>
#include <sstream>
#include <iostream>

#include "util.h"

namespace wincalc
{
    CMA::Option get_cma_option(std::string const & s)
    {
        if(to_lower(s) == "low")
        {
            return CMA::Option::Best;
        }
        else if(to_lower(s) == "high")
        {
            return CMA::Option::Worst;
        }

        std::stringstream msg;
        msg << "Unknown CMA option: " << s;
        throw std::runtime_error(msg.str());
    }

    std::map<BestWorstSpacerIGU, Tarcog::ISO15099::FrameData>
      cma_frame_parameters(thmxParser::ThmxFileContents const & frame)
    {
        const double absorptance = 0.3;   // Absorptance is alwasys 0.3 for now at least
        std::map<BestWorstSpacerIGU, Tarcog::ISO15099::FrameData> results;

        for(auto const & result : frame.results)
        {
            std::optional<double> u_value;
            std::optional<double> u_edge;
            std::optional<double> projected_frame_dimension;
            std::optional<double> wetted_length;

            if(to_lower(result.modelType) != "cma")
            {
                // skip non-cma results
                continue;
            }
            auto spacer_option = get_cma_option(result.spacerCase);
            auto glazing_option = get_cma_option(result.glazingCase);
            for(auto const & u_result : result.ufactorResults)
            {
                if(to_lower(u_result.tag) == "frame")
                {
                    for(auto const & projection_result : u_result.projectionResults)
                    {
                        if(to_lower(projection_result.lengthType) == "projected in glass plane")
                        {
                            u_value = std::stod(projection_result.ufactor);
                            projected_frame_dimension = projection_result.length;
                        }
                        else if(to_lower(projection_result.lengthType) == "total length")
                        {
                            wetted_length = projection_result.length;
                        }
                    }
                }
                else if(to_lower(u_result.tag) == "edge")
                {
                    for(auto const & projection_result : u_result.projectionResults)
                    {
                        if(to_lower(projection_result.lengthType) == "projected in glass plane")
                        {
                            u_edge = std::stod(projection_result.ufactor);
                        }
                    }
                }
            }
            results[std::make_pair(spacer_option, glazing_option)] =
              Tarcog::ISO15099::FrameData(u_value.value(),
                                          u_edge.value(),
                                          projected_frame_dimension.value() / 1000,
                                          wetted_length.value() / 1000,
                                          absorptance);
        }

        return results;
    }

    struct BestWorstUFactors
    {
        CMA::CMABestWorstUFactors best;
        CMA::CMABestWorstUFactors worst;
    };

    BestWorstUFactors get_best_worst_u_factors(thmxParser::ThmxFileContents const & thmx)
    {
        auto options = thmx.cmaOptions.value();
        auto best = options.bestWorstOptions.at("Low");
        auto worst = options.bestWorstOptions.at("High");
        CMA::CMABestWorstUFactors best_u_factors(best.insideConvectiveFilmCoefficient,
                                                 best.outsideConvectiveFilmCoefficient,
                                                 best.glazingGapConductance,
                                                 options.interiorLayerThickness / 1000.0,
                                                 options.interiorLayerConductivity,
                                                 options.interiorLayerEmissivity,
                                                 options.exteriorLayerThickness / 1000.0,
                                                 options.exteriorLayerConductivity,
                                                 options.exteriorLayerEmissivity,
                                                 options.interiorTemperature,
                                                 options.exteriorTemperature);

        CMA::CMABestWorstUFactors worst_u_factors(worst.insideConvectiveFilmCoefficient,
                                                  worst.outsideConvectiveFilmCoefficient,
                                                  worst.glazingGapConductance,
                                                  options.interiorLayerThickness / 1000.0,
                                                  options.interiorLayerConductivity,
                                                  options.interiorLayerEmissivity,
                                                  options.exteriorLayerThickness / 1000.0,
                                                  options.exteriorLayerConductivity,
                                                  options.exteriorLayerEmissivity,
                                                  options.interiorTemperature,
                                                  options.exteriorTemperature);

        return BestWorstUFactors{best_u_factors, worst_u_factors};
    }


    CMA::CMAFrame
      get_cma_frame(std::map<BestWorstSpacerIGU, Tarcog::ISO15099::FrameData> const & frame_data)
    {
        auto best_best = frame_data.at(std::make_pair(CMA::Option::Best, CMA::Option::Best));
        auto best_worst = frame_data.at(std::make_pair(CMA::Option::Best, CMA::Option::Worst));
        auto worst_best = frame_data.at(std::make_pair(CMA::Option::Worst, CMA::Option::Best));
        auto worst_worst = frame_data.at(std::make_pair(CMA::Option::Worst, CMA::Option::Worst));

        return CMA::CMAFrame(best_best, best_worst, worst_best, worst_worst);
    }

    double get_spacer_keff(thmxParser::ThmxFileContents const & spacer)
    {
        std::optional<double> u_factor;
        std::optional<double> spacer_width;

        for(auto const & result : spacer.results)
        {
            if(to_lower(result.modelType) != "u-factor")
            {
                // only interested in u-factor model results for spacer in CMA
                continue;
            }

            for(auto const & u_result : result.ufactorResults)
            {
                if(to_lower(u_result.tag) == "spacerwidth")
                {
                    for(auto const & projection_result : u_result.projectionResults)
                    {
                        if(to_lower(projection_result.lengthType) == "total length")
                        {
                            spacer_width = projection_result.length;
                        }
                    }
                }
                else if(to_lower(u_result.tag) == "spacerinteriortag")
                {
                    for(auto const & projection_result : u_result.projectionResults)
                    {
                        if(to_lower(projection_result.lengthType) == "total length")
                        {
                            u_factor = std::stod(projection_result.ufactor);
                        }
                    }
                }
            }
        }

        // outdoor heat transfer is fixed to 9999 w/m2k for spacer keff calculation
        double ho = 9999;
        // indoor heat transfer is fixed to 9999 w/m2k for spacer keff calculation
        double hi = 9999;
        double spacer_width_m = spacer_width.value() / 1000;

        return spacer_width_m / (1 / u_factor.value() - 1 / ho - 1 / hi);
    }

    std::shared_ptr<CMA::CMAWindowSingleVision>
      get_cma_window_single_vision(thmxParser::ThmxFileContents const & top_frame,
                                   thmxParser::ThmxFileContents const & bottom_frame,
                                   thmxParser::ThmxFileContents const & left_frame,
                                   thmxParser::ThmxFileContents const & right_frame,
                                   double window_width,
                                   double window_height)
    {
        auto top_cma_frame = get_cma_frame(cma_frame_parameters(top_frame));
        auto bottom_cma_frame = get_cma_frame(cma_frame_parameters(bottom_frame));
        auto left_cma_frame = get_cma_frame(cma_frame_parameters(left_frame));
        auto right_cma_frame = get_cma_frame(cma_frame_parameters(right_frame));

        auto best_worst_u_factors = get_best_worst_u_factors(top_frame);
        auto best_spacer_keff =
          top_frame.cmaOptions.value().bestWorstOptions.at("Low").spacerConductance;
        auto worst_spacer_keff =
          top_frame.cmaOptions.value().bestWorstOptions.at("High").spacerConductance;

        std::shared_ptr<CMA::CMAWindowSingleVision> cma_window(
          new CMA::CMAWindowSingleVision(window_width,
                                         window_height,
                                         best_spacer_keff,
                                         worst_spacer_keff,
                                         best_worst_u_factors.best,
                                         best_worst_u_factors.worst));
        cma_window->setFrameTop(top_cma_frame);
        cma_window->setFrameBottom(bottom_cma_frame);
        cma_window->setFrameLeft(left_cma_frame);
        cma_window->setFrameRight(right_cma_frame);
        return cma_window;
    }

    std::shared_ptr<CMA::CMAWindowSingleVision>
      get_cma_window_single_vision(CMA::CMAFrame const & top_frame,
                                   CMA::CMAFrame const & bottom_frame,
                                   CMA::CMAFrame const & left_frame,
                                   CMA::CMAFrame const & right_frame,
                                   double window_width,
                                   double window_height,
                                   double best_spacer_keff,
                                   double worst_spacer_keff,
                                   CMA::CMABestWorstUFactors const & best_u_factors,
                                   CMA::CMABestWorstUFactors const & worst_u_factors)
    {
        std::shared_ptr<CMA::CMAWindowSingleVision> cma_window(
          new CMA::CMAWindowSingleVision(window_width,
                                         window_height,
                                         best_spacer_keff,
                                         worst_spacer_keff,
                                         best_u_factors,
                                         worst_u_factors));
        cma_window->setFrameTop(top_frame);
        cma_window->setFrameBottom(bottom_frame);
        cma_window->setFrameLeft(left_frame);
        cma_window->setFrameRight(right_frame);
        return cma_window;
    }

    std::shared_ptr<CMA::CMAWindowDualVisionVertical>
      get_cma_window_double_vision_vertical(thmxParser::ThmxFileContents const & top_frame,
                                            thmxParser::ThmxFileContents const & bottom_frame,
                                            thmxParser::ThmxFileContents const & top_left_frame,
                                            thmxParser::ThmxFileContents const & top_right_frame,
                                            thmxParser::ThmxFileContents const & bottom_left_frame,
                                            thmxParser::ThmxFileContents const & bottom_right_frame,
                                            thmxParser::ThmxFileContents const & meeting_rail,
                                            double window_width,
                                            double window_height)
    {
        auto top_cma_frame = get_cma_frame(cma_frame_parameters(top_frame));
        auto bottom_cma_frame = get_cma_frame(cma_frame_parameters(bottom_frame));
        auto top_left_cma_frame = get_cma_frame(cma_frame_parameters(top_left_frame));
        auto top_right_cma_frame = get_cma_frame(cma_frame_parameters(top_right_frame));
        auto bottom_left_cma_frame = get_cma_frame(cma_frame_parameters(bottom_left_frame));
        auto bottom_right_cma_frame = get_cma_frame(cma_frame_parameters(bottom_right_frame));
        auto meeting_rail_cma_frame = get_cma_frame(cma_frame_parameters(meeting_rail));

        auto best_worst_u_factors = get_best_worst_u_factors(top_frame);
        auto best_spacer_keff =
          top_frame.cmaOptions.value().bestWorstOptions.at("Low").spacerConductance;
        auto worst_spacer_keff =
          top_frame.cmaOptions.value().bestWorstOptions.at("High").spacerConductance;

        std::shared_ptr<CMA::CMAWindowDualVisionVertical> cma_window(
          new CMA::CMAWindowDualVisionVertical(window_width,
                                               window_height,
                                               best_spacer_keff,
                                               worst_spacer_keff,
                                               best_worst_u_factors.best,
                                               best_worst_u_factors.worst));
        cma_window->setFrameTop(top_cma_frame);
        cma_window->setFrameBottom(bottom_cma_frame);
        cma_window->setFrameTopLeft(top_left_cma_frame);
        cma_window->setFrameTopRight(top_right_cma_frame);
        cma_window->setFrameBottomLeft(bottom_left_cma_frame);
        cma_window->setFrameBottomRight(bottom_right_cma_frame);
        cma_window->setFrameMeetingRail(meeting_rail_cma_frame);
        return cma_window;
    }

    std::shared_ptr<CMA::CMAWindowDualVisionVertical>
      get_cma_window_double_vision_vertical(CMA::CMAFrame const & top_frame,
                                            CMA::CMAFrame const & bottom_frame,
                                            CMA::CMAFrame const & top_left_frame,
                                            CMA::CMAFrame const & top_right_frame,
                                            CMA::CMAFrame const & bottom_left_frame,
                                            CMA::CMAFrame const & bottom_right_frame,
                                            CMA::CMAFrame const & meeting_rail,
                                            double window_width,
                                            double window_height,
                                            double best_spacer_keff,
                                            double worst_spacer_keff,
                                            CMA::CMABestWorstUFactors const & best_u_factors,
                                            CMA::CMABestWorstUFactors const & worst_u_factors)
    {
        std::shared_ptr<CMA::CMAWindowDualVisionVertical> cma_window(
          new CMA::CMAWindowDualVisionVertical(window_width,
                                               window_height,
                                               best_spacer_keff,
                                               worst_spacer_keff,
                                               best_u_factors,
                                               worst_u_factors));
        cma_window->setFrameTop(top_frame);
        cma_window->setFrameBottom(bottom_frame);
        cma_window->setFrameTopLeft(top_left_frame);
        cma_window->setFrameTopRight(top_right_frame);
        cma_window->setFrameBottomLeft(bottom_left_frame);
        cma_window->setFrameBottomRight(bottom_right_frame);
        cma_window->setFrameMeetingRail(meeting_rail);
        return cma_window;
    }

    std::shared_ptr<CMA::CMAWindowDualVisionHorizontal> get_cma_window_double_vision_horizontal(
      thmxParser::ThmxFileContents const & top_left_frame,
      thmxParser::ThmxFileContents const & top_right_frame,
      thmxParser::ThmxFileContents const & bottom_left_frame,
      thmxParser::ThmxFileContents const & bottom_right_frame,
      thmxParser::ThmxFileContents const & left_frame,
      thmxParser::ThmxFileContents const & right_frame,
      thmxParser::ThmxFileContents const & meeting_rail,
      double window_width,
      double window_height)
    {
        auto top_left_cma_frame = get_cma_frame(cma_frame_parameters(top_left_frame));
        auto top_right_cma_frame = get_cma_frame(cma_frame_parameters(top_right_frame));
        auto bottom_left_cma_frame = get_cma_frame(cma_frame_parameters(bottom_left_frame));
        auto bottom_right_cma_frame = get_cma_frame(cma_frame_parameters(bottom_right_frame));
        auto meeting_rail_cma_frame = get_cma_frame(cma_frame_parameters(meeting_rail));
        auto left_cma_frame = get_cma_frame(cma_frame_parameters(left_frame));
        auto right_cma_frame = get_cma_frame(cma_frame_parameters(right_frame));

        auto best_worst_u_factors = get_best_worst_u_factors(top_left_frame);
        auto best_spacer_keff =
          top_left_frame.cmaOptions.value().bestWorstOptions.at("Low").spacerConductance;
        auto worst_spacer_keff =
          top_left_frame.cmaOptions.value().bestWorstOptions.at("High").spacerConductance;

        std::shared_ptr<CMA::CMAWindowDualVisionHorizontal> cma_window(
          new CMA::CMAWindowDualVisionHorizontal(window_width,
                                                 window_height,
                                                 best_spacer_keff,
                                                 worst_spacer_keff,
                                                 best_worst_u_factors.best,
                                                 best_worst_u_factors.worst));
        cma_window->setFrameTopLeft(top_left_cma_frame);
        cma_window->setFrameTopRight(top_right_cma_frame);
        cma_window->setFrameBottomLeft(bottom_left_cma_frame);
        cma_window->setFrameBottomRight(bottom_right_cma_frame);
        cma_window->setFrameLeft(left_cma_frame);
        cma_window->setFrameRight(right_cma_frame);
        cma_window->setFrameMeetingRail(meeting_rail_cma_frame);
        return cma_window;
    }

        std::shared_ptr<CMA::CMAWindowDualVisionHorizontal>
		get_cma_window_double_vision_horizontal(CMA::CMAFrame const & top_left_frame,
			CMA::CMAFrame const & top_right_frame,
			CMA::CMAFrame const & bottom_left_frame,
			CMA::CMAFrame const & bottom_right_frame,
			CMA::CMAFrame const & left_frame,
			CMA::CMAFrame const & right_frame,
			CMA::CMAFrame const & meeting_rail,
			double window_width,
			double window_height,
			double best_spacer_keff,
			double worst_spacer_keff,
			CMA::CMABestWorstUFactors const & best_u_factors,
			CMA::CMABestWorstUFactors const & worst_u_factors)
	{
        std::shared_ptr<CMA::CMAWindowDualVisionHorizontal> cma_window(
          new CMA::CMAWindowDualVisionHorizontal(window_width,
                                                 window_height,
                                                 best_spacer_keff,
                                                 worst_spacer_keff,
                                                 best_u_factors,
                                                 worst_u_factors));
        cma_window->setFrameTopLeft(top_left_frame);
        cma_window->setFrameTopRight(top_right_frame);
        cma_window->setFrameBottomLeft(bottom_left_frame);
        cma_window->setFrameBottomRight(bottom_right_frame);
        cma_window->setFrameLeft(left_frame);
        cma_window->setFrameRight(right_frame);
        cma_window->setFrameMeetingRail(meeting_rail);
        return cma_window;
	}

    CMAResult calc_cma(std::shared_ptr<CMA::ICMAWindow> window,
                       double glazing_system_u,
                       double glazing_system_shgc,
                       double glazing_system_visible_front_direct_hemispheric_transmittance,
                       double spacer_keff)
    {
        auto tvis = window->vt(glazing_system_visible_front_direct_hemispheric_transmittance);
        auto u = window->uValue(glazing_system_u, spacer_keff);
        auto shgc = window->shgc(glazing_system_shgc, spacer_keff);
        return CMAResult{u, shgc, tvis};
    }

}   // namespace wincalc
