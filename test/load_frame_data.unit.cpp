#include <gtest/gtest.h>
#include <string>

#include <WCETarcog.hpp>

#include "thmz.h"

namespace
{
    void PrintTo(wincalc::FrameLoadErr err, std::ostream * os)
    {
        using wincalc::FrameLoadErr;
        switch(err)
        {
            case FrameLoadErr::MissingThermResults:
                *os << "MissingThermResults";
                break;
            case FrameLoadErr::MissingThermModel:
                *os << "MissingThermModel";
                break;
            case FrameLoadErr::NoUFactorCase:
                *os << "NoUFactorCase";
                break;
            case FrameLoadErr::FrameOrEdgeTagMissing:
                *os << "FrameOrEdgeTagMissing";
                break;
            case FrameLoadErr::UValuesMissing:
                *os << "UValuesMissing";
                break;
            case FrameLoadErr::ProjectedLenMissing:
                *os << "ProjectedLenMissing";
                break;
            case FrameLoadErr::WettedLenMissing:
                *os << "WettedLenMissing";
                break;
            case FrameLoadErr::IGUDataMissing:
                *os << "IGUDataMissing";
                break;
            default:
                *os << "Unknown(" << static_cast<int>(err) << ")";
                break;
        }
    }
}   // namespace


TEST(WincalcLoadFrameData, LoadsSampleSillFile)
{
    // Construct path using macro passed from CMake
    const std::string path = std::string(TEST_DATA_DIR) + "/products/sample-sill.thmz";

    // Tags will already contain default tag values
    const wincalc::Tags tags;
    const auto frame_data = wincalc::load_frame_data(path.data(), tags).value();

    // Basic checks — adapt as needed based on file contents
    EXPECT_DOUBLE_EQ(frame_data.UValue, 2.017913);
    EXPECT_DOUBLE_EQ(frame_data.EdgeUValue, 2.339591);
    EXPECT_DOUBLE_EQ(frame_data.ProjectedFrameDimension, 42.875183);
    EXPECT_DOUBLE_EQ(frame_data.WettedLength, 56.332821);
    EXPECT_DOUBLE_EQ(frame_data.Absorptance, 0.3);

    // IGU data checks
    ASSERT_TRUE(frame_data.iguData.has_value());
    EXPECT_DOUBLE_EQ(frame_data.iguData->UValue, 1.934666);
    EXPECT_DOUBLE_EQ(frame_data.iguData->Thickness, 25.9334);
}

TEST(WincalcLoadFrameData, FailsGracefullyOnMissingResults)
{
    const std::string path = std::string(TEST_DATA_DIR) + "/products/sample-sill-no-results.thmz";
    const wincalc::Tags tags;

    const auto result = wincalc::load_frame_data(path, tags);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), wincalc::FrameLoadErr::MissingThermResults);
}
