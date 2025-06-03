#include <numeric>
#include <algorithm>
#include <string_view>

#include <THMZ/Model/THMX.hxx>
#include <THMZ/Model/DB.hxx>
#include <THMZ/SteadyStateResults/Results.hxx>
#include <THMZ/SteadyStateResults/DB.hxx>

#include <lbnl/algorithm.hxx>
#include <lbnl/optional.hxx>
#include <lbnl/expected.hxx>

#include "thmz.h"   // contains struct Tags and resource‑ID macros like IDS_NOTHERMRESULTS

namespace wincalc
{
    namespace Helper
    {
        template<class Vec, class Pred>
        inline auto expect_element(const Vec & v, Pred && p, FrameLoadErr err)
          -> lbnl::ExpectedExt<const typename Vec::value_type *, FrameLoadErr>
        {
            auto it = std::ranges::find_if(v, std::forward<Pred>(p));
            if(it != v.end())
                return &*it;
            return lbnl::make_unexpected<const typename Vec::value_type *, FrameLoadErr>(err);
        }

        inline double glz_sys_thickness(const ThermFile::GlazingSystem & g)
        {
            const auto sum = [](auto && rng) {
                return std::accumulate(rng.begin(), rng.end(), 0.0, [](double acc, const auto & x) {
                    return acc + x.thickness;
                });
            };
            return sum(g.layers) + sum(g.gases);
        }

        inline auto get_u_case(const ThermFile::SteadyStateResults & r)
          -> lbnl::ExpectedExt<const ThermFile::SteadyStateResultsCase *, FrameLoadErr>
        {
            return expect_element(
              r.cases,
              [](const auto & c) { return c.modelType == ThermFile::RunType::UFactor; },
              FrameLoadErr::NoUFactorCase);
        }

        inline auto expect_projected_uvalue(const ThermFile::SteadyStateUFactors * f)
          -> lbnl::ExpectedExt<double, FrameLoadErr>
        {
            auto proj = expect_element(
              f->projections,
              [](const auto & p) {
                  return p.lengthType == ThermFile::UValueDimensionType::GlassRotationProjected;
              },
              FrameLoadErr::ProjectedLenMissing);
            if(!proj.has_value())
                return lbnl::make_unexpected<double, FrameLoadErr>(proj.error());
            if(proj.value()->uFactor)
                return *proj.value()->uFactor;
            return lbnl::make_unexpected<double, FrameLoadErr>(FrameLoadErr::UValuesMissing);
        }

        inline auto expect_projected_length(const ThermFile::SteadyStateUFactors * f)
          -> lbnl::ExpectedExt<double, FrameLoadErr>
        {
            auto proj = expect_element(
              f->projections,
              [](const auto & p) {
                  return p.lengthType == ThermFile::UValueDimensionType::GlassRotationProjected;
              },
              FrameLoadErr::ProjectedLenMissing);
            if(!proj.has_value())
                return lbnl::make_unexpected<double, FrameLoadErr>(proj.error());
            if(proj.value()->length)
                return *proj.value()->length;
            return lbnl::make_unexpected<double, FrameLoadErr>(FrameLoadErr::ProjectedLenMissing);
        }

        inline auto expect_wetted_length(const ThermFile::SteadyStateResultsCase * ucase,
                                         const Tags & tags)
          -> lbnl::ExpectedExt<double, FrameLoadErr>
        {
            auto shgcF = expect_element(
              ucase->steadyStateUFactors,
              [&](const auto & f) { return f.tag == tags.shgc; },
              FrameLoadErr::FrameOrEdgeTagMissing);
            if(shgcF.has_value()) {
                auto proj = expect_element(
                  shgcF.value()->projections,
                  [](const auto & p) {
                      return p.lengthType == ThermFile::UValueDimensionType::TotalLength;
                  },
                  FrameLoadErr::WettedLenMissing);
                if(proj.has_value() && proj.value()->length && *proj.value()->length >= 0.0)
                    return *proj.value()->length;
            }

            auto frameF = expect_element(
              ucase->steadyStateUFactors,
              [&](const auto & f) { return f.tag == tags.frame; },
              FrameLoadErr::FrameOrEdgeTagMissing);
            if(frameF.has_value()) {
                auto proj = expect_element(
                  frameF.value()->projections,
                  [](const auto & p) {
                      return p.lengthType == ThermFile::UValueDimensionType::TotalLength;
                  },
                  FrameLoadErr::WettedLenMissing);
                if(proj.has_value() && proj.value()->length)
                    return *proj.value()->length;
            }

            return lbnl::make_unexpected<double, FrameLoadErr>(FrameLoadErr::WettedLenMissing);
        }

        inline auto expect_igu_data(const ThermFile::ThermModel & model)
          -> lbnl::ExpectedExt<Tarcog::ISO15099::IGUData, FrameLoadErr>
        {
            if(model.glazingSystems.empty())
                return lbnl::make_unexpected<Tarcog::ISO15099::IGUData>(FrameLoadErr::IGUDataMissing);

            const auto & glz = model.glazingSystems.front();
            auto winter = lbnl::find_element(glz.properties, [](const auto & p) {
                return p.environmentalCondition == ThermFile::EnvironmentalCondition::Winter;
            });

            if(!winter)
                return lbnl::make_unexpected<Tarcog::ISO15099::IGUData>(FrameLoadErr::IGUDataMissing);

            return Tarcog::ISO15099::IGUData{winter->uValue, glz_sys_thickness(glz)};
        }

    }   // namespace Helper

    FrameLoadResult load_frame_data(std::string_view fileName, const Tags & tags)
    {
        auto resultsOpt = ThermFile::loadSteadyStateResultsFromZipFile(fileName.data());
        if(!resultsOpt.has_value())
            return FrameLoadErr::MissingThermResults;

        auto modelOpt = ThermFile::loadThermModelFromZipFile(fileName.data());
        if(!modelOpt.has_value())
            return FrameLoadErr::MissingThermModel;

        const auto & results = *resultsOpt;
        const auto & model = *modelOpt;

        auto ucase = Helper::get_u_case(results);
        if(!ucase.has_value())
            return ucase.error();

        auto frameF = Helper::expect_element(
          ucase.value()->steadyStateUFactors,
          [&](const auto & f) { return f.tag == tags.frame; },
          FrameLoadErr::FrameOrEdgeTagMissing);
        if(!frameF.has_value())
            return frameF.error();

        auto edgeF = Helper::expect_element(
          ucase.value()->steadyStateUFactors,
          [&](const auto & f) { return f.tag == tags.edge; },
          FrameLoadErr::FrameOrEdgeTagMissing);
        if(!edgeF.has_value())
            return edgeF.error();

        auto uVal = Helper::expect_projected_uvalue(frameF.value());
        if(!uVal.has_value())
            return uVal.error();

        auto edgeUVal = Helper::expect_projected_uvalue(edgeF.value());
        if(!edgeUVal.has_value())
            return edgeUVal.error();

        auto projLen = Helper::expect_projected_length(frameF.value());
        if(!projLen.has_value())
            return projLen.error();

        auto wettedLen = Helper::expect_wetted_length(ucase.value(), tags);
        if(!wettedLen.has_value())
            return wettedLen.error();

        auto iguData = Helper::expect_igu_data(model);
        if(!iguData.has_value())
            return iguData.error();

        return Tarcog::ISO15099::FrameData{
            std::nullopt,
            uVal.value(),
            edgeUVal.value(),
            projLen.value(),
            wettedLen.value(),
            0.3,
            iguData.value()
        };
    }

}   // namespace wincalc