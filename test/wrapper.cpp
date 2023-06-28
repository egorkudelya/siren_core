#include <gtest/gtest.h>
#include "../src/client_wrapper/client_wrapper.h"

TEST(WrapperTest, Env)
{
    setenv("WINDOW_FUNCTION", "Blackman", 1);
    setenv("SAMPLING_RATE", "512", 1);

    auto wrapper = siren::client::ClientWrapper();
    std::string json_str = wrapper.process_track("../audio/jazzfrom5to7.wav");
    std::size_t found = json_str.find("core failed to fingerprint the track");
    EXPECT_EQ(found, std::string::npos);
}