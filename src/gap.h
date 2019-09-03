#ifndef WINCALC_GAP_H
#define WINCALC_GAP_H

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
}   // namespace wincalc
#endif
