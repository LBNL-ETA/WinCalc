#pragma once

#include <optional>
#include <WCETarcog.hpp>

namespace wincalc
{
    struct Tags
    {
        std::string frame{"Frame"};
        std::string edge{"Edge"};
        std::string shgc{"SHGC Exterior"};
    };

    std::optional<Tarcog::ISO15099::FrameData> load_frame_data(std::string_view file_name, const Tags& tags);
}