#include <gtest/gtest.h>
#include "common.h"


TEST(AssertionTest, AssertRelTrue)
{
    auto core = test_common::CommonCore::GetCore();
    auto fingerprint = core->make_fingerprint("../audio/jazzfrom5to7.wav");
    release_assert(fingerprint.code == siren::CoreStatus::OK, "Core code != CoreStatus::OK");
}

TEST(AssertionTest, AssertRelFalse)
{
    auto core = test_common::CommonCore::GetCore();
    auto fingerprint = core->make_fingerprint("../audio/none.wav");
    EXPECT_DEATH(release_assert(fingerprint.code == siren::CoreStatus::OK, "Core code != CoreStatus::OK"), "");
}