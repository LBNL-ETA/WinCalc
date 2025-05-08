#include <THMZ/Model/THMX.hxx>
#include <THMZ/Model/DB.hxx>
#include <THMZ/SteadyStateResults/Results.hxx>
#include <THMZ/SteadyStateResults/DB.hxx>

#include <lbnl/algorithm.hxx>

#include "thmz.h"

namespace wincalc
{
    namespace Helper
    {
        std::optional<ThermFile::SteadyStateUFactors>
          find_u_factor_by_tag(const std::vector<ThermFile::SteadyStateUFactors> & factors,
                               std::string_view tag)
        {
            return lbnl::find_element(factors, [&](const auto & f) { return f.tag == tag; });
        }

        std::optional<ThermFile::Projection>
          find_projection(const std::vector<ThermFile::Projection> & projections,
                          ThermFile::UValueDimensionType type)
        {
            return lbnl::find_element(projections,
                                      [&](const auto & p) { return p.lengthType == type; });
        }

        double glz_sys_thickness(const ThermFile::GlazingSystem & g)
        {
            const auto sum = [](auto && rng) {
                return std::accumulate(rng.begin(), rng.end(), 0.0, [](double acc, const auto & x) {
                    return acc + x.thickness;
                });
            };
            return sum(g.layers) + sum(g.gases);
        }

        std::optional<ThermFile::SteadyStateResultsCase>
          get_u_case(const ThermFile::SteadyStateResults & results)
        {
            return lbnl::find_element(results.cases, [](const auto & c) {
                return c.modelType == ThermFile::RunType::UFactor;
            });
        }

        std::optional<double> get_projected_uvalue(const ThermFile::SteadyStateUFactors & factors)
        {
            const auto proj = Helper::find_projection(
              factors.projections, ThermFile::UValueDimensionType::GlassRotationProjected);

            if(proj && proj->uFactor)
            {
                return *proj->uFactor;
            }

            return std::nullopt;
        }

        std::optional<double> get_projected_length(const ThermFile::SteadyStateUFactors & factors)
        {
            const auto proj = Helper::find_projection(
              factors.projections, ThermFile::UValueDimensionType::GlassRotationProjected);

            if(proj && proj->length)
            {
                return *proj->length;
            }

            return std::nullopt;
        }

        std::optional<double> get_wetted_length(const ThermFile::SteadyStateResultsCase & ucase,
                                                const Tags & tags)
        {
            if(auto shgc = Helper::find_u_factor_by_tag(ucase.steadyStateUFactors, tags.shgc))
            {
                auto p = Helper::find_projection(shgc->projections,
                                                 ThermFile::UValueDimensionType::TotalLength);
                if(p && p->length && *p->length >= 0.0)
                {
                    return *p->length;
                }
            }

            if(auto frame = Helper::find_u_factor_by_tag(ucase.steadyStateUFactors, tags.frame))
            {
                auto p = Helper::find_projection(frame->projections,
                                                 ThermFile::UValueDimensionType::TotalLength);
                if(p && p->length)
                {
                    return *p->length;
                }
            }

            return std::nullopt;
        }

        std::optional<Tarcog::ISO15099::IGUData> get_igu_data(const ThermFile::ThermModel & model)
        {
            if(model.glazingSystems.empty())
            {
                return std::nullopt;
            }

            const auto & glz = model.glazingSystems.front();

            auto winter = lbnl::find_element(glz.properties, [](const auto & p) {
                return p.environmentalCondition == ThermFile::EnvironmentalCondition::Winter;
            });

            if(!winter)
            {
                return std::nullopt;
            }

            return Tarcog::ISO15099::IGUData{.UValue = winter->uValue,
                                             .Thickness = Helper::glz_sys_thickness(glz)};
        }

    }   // namespace Helper

    std::optional<Tarcog::ISO15099::FrameData> load_frame_data(std::string_view file_name,
                                                               const Tags & tags)
    {
        auto resultsOpt = ThermFile::loadSteadyStateResultsFromZipFile(file_name.data());
        auto modelOpt = ThermFile::loadThermModelFromZipFile(file_name.data());

        if(!resultsOpt || !modelOpt)
            return std::nullopt;

        const auto & results = *resultsOpt;
        const auto & model = *modelOpt;

        const auto ucaseOpt = Helper::get_u_case(results);
        if(!ucaseOpt)
            return std::nullopt;

        const auto & ucase = *ucaseOpt;

        auto frameFactor = Helper::find_u_factor_by_tag(ucase.steadyStateUFactors, tags.frame);
        auto edgeFactor = Helper::find_u_factor_by_tag(ucase.steadyStateUFactors, tags.edge);
        if(!frameFactor || !edgeFactor)
            return std::nullopt;

        auto uValue = Helper::get_projected_uvalue(*frameFactor);
        auto edgeUValue = Helper::get_projected_uvalue(*edgeFactor);
        auto projected = Helper::get_projected_length(*frameFactor);
        auto wetted = Helper::get_wetted_length(ucase, tags);
        auto igu = Helper::get_igu_data(model);

        if(!uValue || !edgeUValue || !projected || !wetted || !igu)
            return std::nullopt;

        return Tarcog::ISO15099::FrameData{.UValue = *uValue,
                                           .EdgeUValue = *edgeUValue,
                                           .ProjectedFrameDimension = *projected,
                                           .WettedLength = *wetted,
                                           .Absorptance = 0.3,
                                           .iguData = *igu};
    }

}   // namespace wincalc