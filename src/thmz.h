#pragma once

#include <optional>

#include <WCETarcog.hpp>
#include <lbnl/expected.hxx>

namespace wincalc
{
    struct Tags
    {
        std::string frame{"Frame"};
        std::string edge{"Edge"};
        std::string shgc{"SHGC Exterior"};
    };

    enum class FrameLoadErr
    {
        MissingThermResults,
        MissingThermModel,
        NoUFactorCase,
        FrameOrEdgeTagMissing,
        UValuesMissing,
        ProjectedLenMissing,
        WettedLenMissing,
        IGUDataMissing
    };

    using FrameLoadResult = lbnl::ExpectedExt<Tarcog::ISO15099::FrameData, FrameLoadErr>;

    FrameLoadResult load_frame_data(std::string_view file_name, const Tags & tags);
}   // namespace wincalc