#ifndef WINCALC_CMA_H_
#define WINCALC_CMA_H_

#include <thmxParser/thmxParser.hpp>

namespace wincalc
{
    struct CMAResult
    {
        double u;
        double shgc;
        double vt;
    };

	CMAResult cmaSingleVision(thmxParser::ThmxFileContents const & top_frame,
		thmxParser::ThmxFileContents const & bottom_frame,
		thmxParser::ThmxFileContents const & left_frame,
		thmxParser::ThmxFileContents const& right_frame,
		thmxParser::ThmxFileContents const& spacer,
		double window_width,
		double window_height,
		double glazing_system_u,
		double glazing_system_shgc,
		double glazing_system_visible_front_direct_hemispheric_transmittance,
		double glazing_system_solar_front_direct_hemispheric_transmittance);
}   // namespace wincalc
#endif
