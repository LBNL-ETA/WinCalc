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
    }   // namespace Helper

    std::optional<Tarcog::ISO15099::FrameData> load_frame_data(std::string_view file_name,
                                                               const Tags & tags)
    {
        auto resultsOpt = ThermFile::loadSteadyStateResultsFromZipFile(file_name.data());
        auto modelOpt = ThermFile::loadThermModelFromZipFile(file_name.data());

        if(!resultsOpt || !modelOpt)
        {
            return std::nullopt;
        }

        const auto & results = *resultsOpt;
        const auto & model = *modelOpt;

        const auto uCaseOpt =
          lbnl::find_element(results.cases, [](const ThermFile::SteadyStateResultsCase & c) {
              return c.modelType == ThermFile::RunType::UFactor;
          });

        if(!uCaseOpt)
        {
            return std::nullopt;
        }

        auto frameFactor = Helper::find_u_factor_by_tag(uCaseOpt->steadyStateUFactors, tags.frame);
        auto edgeFactor = Helper::find_u_factor_by_tag(uCaseOpt->steadyStateUFactors, tags.edge);

        if(!frameFactor || !edgeFactor)
        {
            return std::nullopt;
        }

        const auto frameProj = Helper::find_projection(
          frameFactor->projections, ThermFile::UValueDimensionType::GlassRotationProjected);
        const auto edgeProj = Helper::find_projection(
          edgeFactor->projections, ThermFile::UValueDimensionType::GlassRotationProjected);

        if(!frameProj || !edgeProj || !frameProj->uFactor || !edgeProj->uFactor)
            return std::nullopt;

        double uValue = *frameProj->uFactor;
        double edgeUValue = *edgeProj->uFactor;

        if(!frameProj->length)
            return std::nullopt;

        double projectedLength = *frameProj->length;

        double wettedLength = 0.0;
        bool hasValidSHGCWettedLength = false;

        if(auto shgcFactor = Helper::find_u_factor_by_tag(uCaseOpt->steadyStateUFactors, tags.shgc))
        {
            if(auto shgcProj = Helper::find_projection(shgcFactor->projections,
                                                       ThermFile::UValueDimensionType::TotalLength))
            {
                if(shgcProj->length && *shgcProj->length >= 0.0)
                {
                    wettedLength = *shgcProj->length;
                    hasValidSHGCWettedLength = true;
                }
            }
        }

        if(!hasValidSHGCWettedLength)
        {
            if(auto frameTotalProj = Helper::find_projection(
                 frameFactor->projections, ThermFile::UValueDimensionType::TotalLength))
            {
                if(frameTotalProj->length)
                {
                    wettedLength = *frameTotalProj->length;
                }
                else
                {
                    return std::nullopt;
                }
            }
            else
            {
                return std::nullopt;
            }
        }

        const auto & glazingSystems = model.glazingSystems;
        if(glazingSystems.empty())
        {
            return std::nullopt;
        }

        const auto & properties = glazingSystems[0].properties;
        const auto winter =
          lbnl::find_element(properties, [](const ThermFile::GlazingSystemProperties & p) {
              return p.environmentalCondition == ThermFile::EnvironmentalCondition::Winter;
          });

        if(!winter)
        {
            return std::nullopt;
        }

        const double centerU = winter->uValue;
        const double thickness = Helper::glz_sys_thickness(glazingSystems.front());

        return Tarcog::ISO15099::FrameData{.UValue = uValue,
                                           .EdgeUValue = edgeUValue,
                                           .ProjectedFrameDimension = projectedLength,
                                           .WettedLength = wettedLength,
                                           .Absorptance = 0.3,
                                           .iguData =
                                             Tarcog::ISO15099::IGUData{centerU, thickness}};
    }
}   // namespace wincalc