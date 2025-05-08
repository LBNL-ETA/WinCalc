#pragma once

#include <WCETarcog.hpp>

namespace wincalc
{
    Tarcog::ISO15099::FrameData load_frame_data(std::string_view file_name);
}