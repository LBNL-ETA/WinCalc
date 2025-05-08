#include <gtest/gtest.h>
#include <string>

#include <WCETarcog.hpp>

#include "thmz.h"  // wherever the function is declared

TEST(WincalcLoadFrameData, LoadsSampleSillFile)
{
    // Construct path using macro passed from CMake
    const std::string path = std::string(TEST_DATA_DIR) + "/products/sample-sill.thmz";

    // Load frame data
    const auto frame_data = wincalc::load_frame_data(path);

    // Basic checks — adapt as needed based on file contents
    EXPECT_GT(frame_data.UValue, 0.0);
    EXPECT_GT(frame_data.ProjectedFrameDimension, 0.0);
    EXPECT_GE(frame_data.Absorptance, 0.0);
    EXPECT_LE(frame_data.Absorptance, 1.0);

    // Optional: verify against expected values if known
    // EXPECT_NEAR(frame_data.UValue, 2.123, 1e-6);
}
