#include "common.h"
#include <gtest/gtest.h>

TEST(CoreTest, DefaultParams)
{
    auto core = test_common::CommonCore::GetCore();
    auto fingerprint = core->make_fingerprint("../audio/jazzfrom5to7.wav");
    EXPECT_EQ(fingerprint.code, siren::CoreStatus::OK);
}