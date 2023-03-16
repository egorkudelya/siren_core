#include <gtest/gtest.h>
#include "../src/common/common.h"
#include "../src/siren.h"

namespace test_assert
{
    siren::SirenCore* CreateCore()
    {
        siren::CoreSpecification spec;
        return new siren::SirenCore(std::move(spec));
    }
}

TEST(AssertionTest, AssertRelTrue)
{
    auto core = test_assert::CreateCore();
    auto fingerprint = core->make_fingerprint("../audio/jazzfrom5to7.wav");
    delete core;
    release_assert(fingerprint.code == siren::CoreStatus::OK, "Core code != CoreStatus::OK");
}

TEST(AssertionTest, AssertRelFalse)
{
    auto core = test_assert::CreateCore();
    auto fingerprint = core->make_fingerprint("../audio/none.wav");
    delete core;
    EXPECT_DEATH(release_assert(fingerprint.code == siren::CoreStatus::OK, "Core code != CoreStatus::OK"), "");
}