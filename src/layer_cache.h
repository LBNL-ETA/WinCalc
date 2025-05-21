#pragma once

#include <string>
#include <cstddef>
#include <functional>

#include "create_wce_objects.h"

namespace wincalc
{
    struct LayerCacheKey {
        std::string method_name;
        Spectal_Data_Wavelength_Range_Method wavelength_method;
        int number_visible_bands;
        int number_solar_bands;
        size_t bsdf_direction_count;  // new addition

        bool operator==(const LayerCacheKey & other) const {
            return method_name == other.method_name &&
                   wavelength_method == other.wavelength_method &&
                   number_visible_bands == other.number_visible_bands &&
                   number_solar_bands == other.number_solar_bands &&
                   bsdf_direction_count == other.bsdf_direction_count;
        }
    };
}

namespace std
{
    template<>
    struct hash<wincalc::LayerCacheKey>
    {
        std::size_t operator()(const wincalc::LayerCacheKey & key) const
        {
            std::size_t h1 = std::hash<std::string>{}(key.method_name);
            std::size_t h2 = std::hash<int>{}(static_cast<int>(key.wavelength_method));
            std::size_t h3 = std::hash<int>{}(key.number_visible_bands);
            std::size_t h4 = std::hash<int>{}(key.number_solar_bands);
            std::size_t h5 = std::hash<std::size_t>{}(key.bsdf_direction_count);

            // Combine hashes using a simple method
            return (((((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1)) >> 1) ^ (h4 << 1)) ^ (h5 << 1);
        }
    };
}
