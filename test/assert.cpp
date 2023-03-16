#include <gtest/gtest.h>
#include "../src/common/common.h"
#include "../src/siren.h"

namespace test_assert
{
    std::unique_ptr<siren::SirenCore> CreateCore()
    {
        siren::CoreSpecification spec;
        return std::make_unique<siren::SirenCore>(std::move(spec));
    }
}

TEST(AssertionTest, AssertRelTrue)
{
    auto core = test_assert::CreateCore();
    auto fingerprint = core->make_fingerprint("../audio/jazzfrom5to7.wav");
    release_assert(fingerprint.code == siren::CoreStatus::OK, "Core code != CoreStatus::OK");
}

TEST(AssertionTest, AssertRelFalse)
{
    auto core = test_assert::CreateCore();
    auto fingerprint = core->make_fingerprint("../audio/none.wav");
    EXPECT_DEATH(release_assert(fingerprint.code == siren::CoreStatus::OK, "Core code != CoreStatus::OK"), "");
}