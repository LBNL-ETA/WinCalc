#include "environmental_conditions.h"

namespace wincalc
{
    Environments nfrc_u_environments()
    {
        Environment inside{294.15,
                           101325.0,
                           0.0,
                           Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH,
                           294.15,
                           1.0,
                           0.0,
                           0.0};

        Environment outside{255.15,
                            101325.0,
                            26.0,
                            Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH,
                            255.15,
                            1.0,
                            5.5,
                            0.0};

        return Environments{outside, inside};
    }

    Environments nfrc_shgc_environments()
    {
        Environment inside{297.15,
                           101325.0,
                           0.0,
                           Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH,
                           297.15,
                           1.0,
                           0.0,
                           0.0};

        Environment outside{305.15,
                            101325.0,
                            15.0,
                            Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH,
                            305.15,
                            1.0,
                            2.75,
                            783};

        return Environments{outside, inside};
    }
}   // namespace wincalc
