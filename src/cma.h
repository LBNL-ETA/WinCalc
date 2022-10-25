#ifndef WINCALC_CMA_H_
#define WINCALC_CMA_H_

#include <thmxParser/thmxParser.hpp>
#include <WCETarcog.hpp>
#include <memory>

namespace wincalc
{
    struct CMAResult
    {
        double u;
        double shgc;
        double vt;
    };

    using BestWorstSpacerIGU = std::pair<CMA::Option, CMA::Option>;

    std::map<BestWorstSpacerIGU, Tarcog::ISO15099::FrameData>
      cma_frame_parameters(thmxParser::ThmxFileContents const & frame);

    double get_spacer_keff(thmxParser::ThmxFileContents const & spacer);

    std::shared_ptr<CMA::CMAWindowSingleVision>
      get_cma_window_single_vision(thmxParser::ThmxFileContents const & top_frame,
                                   thmxParser::ThmxFileContents const & bottom_frame,
                                   thmxParser::ThmxFileContents const & left_frame,
                                   thmxParser::ThmxFileContents const & right_frame,
                                   double window_width,
                                   double window_height);

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
                                   CMA::CMABestWorstUFactors const & worst_u_factors);


    std::shared_ptr<CMA::CMAWindowDualVisionVertical>
      get_cma_window_double_vision_vertical(thmxParser::ThmxFileContents const & top_frame,
                                            thmxParser::ThmxFileContents const & bottom_frame,
                                            thmxParser::ThmxFileContents const & top_left_frame,
                                            thmxParser::ThmxFileContents const & top_right_frame,
                                            thmxParser::ThmxFileContents const & bottom_left_frame,
                                            thmxParser::ThmxFileContents const & bottom_right_frame,
                                            thmxParser::ThmxFileContents const & meeting_rail,
                                            double window_width,
                                            double window_height);

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
                                            CMA::CMABestWorstUFactors const & worst_u_factors);

    std::shared_ptr<CMA::CMAWindowDualVisionHorizontal> get_cma_window_double_vision_horizontal(
      thmxParser::ThmxFileContents const & top_left_frame,
      thmxParser::ThmxFileContents const & top_right_frame,
      thmxParser::ThmxFileContents const & bottom_left_frame,
      thmxParser::ThmxFileContents const & bottom_right_frame,
      thmxParser::ThmxFileContents const & left_frame,
      thmxParser::ThmxFileContents const & right_frame,
      thmxParser::ThmxFileContents const & meeting_rail,
      double window_width,
      double window_height);

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
			CMA::CMABestWorstUFactors const & worst_u_factors);

    CMAResult calc_cma(CMA::ICMAWindow & window,
                       double glazing_system_u,
                       double glazing_system_shgc,
                       double glazing_system_visible_front_direct_hemispheric_transmittance,
                       double spacer_keff);

}   // namespace wincalc
#endif
