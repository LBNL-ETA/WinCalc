#include <THMZ/Model/THMX.hxx>
#include <THMZ/Model/DB.hxx>
#include <THMZ/SteadyStateResults/Results.hxx>
#include <THMZ/SteadyStateResults/DB.hxx>

#include "thmz.h"

namespace wincalc
{
    Tarcog::ISO15099::FrameData load_frame_data(std::string_view file_name)
    {
        auto results = ThermFile::loadSteadyStateResultsFromZipFile(file_name.data());
        auto model = ThermFile::loadThermModelFromZipFile(file_name.data());

        return {};
    }
}   // namespace wincalc