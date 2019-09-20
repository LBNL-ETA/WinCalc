#ifndef WINCALC_GAP_H
#define WINCALC_GAP_H

#include <WCEGases.hpp>

namespace wincalc
{
    enum class Gas_Type
    {
        AIR,
        ARGON,
        KRYPTON,
        XENON
    };

    struct Gap_Data
    {
        Gas_Type gas;
        double thickness;
    };

    struct Engine_Gap_Info
    {
        Gases::CGasData gas;
        double thickness;
    };
}   // namespace wincalc
#endif
