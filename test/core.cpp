#include "../src/client_wrapper/client_wrapper.h"
#include <gtest/gtest.h>

namespace test_default
{
    siren::SirenCore* CreateCore()
    {
        siren::CoreSpecification spec;
        return new siren::SirenCore(std::move(spec));
    }
}

namespace test_params
{
    siren::SirenCore* CreateCore()
    {
        siren::CoreSpecification spec;

        spec.core_params.target_window_size = 512;
        spec.core_params.target_peak_threshold = 2;
        spec.core_params.target_window_function = siren::WindowFunction::Blackman;

        return new siren::SirenCore(std::move(spec));
    }
}


TEST(CoreTest, DefaultParams)
{
    auto core = test_default::CreateCore();
    auto fingerprint = core->make_fingerprint("../audio/jazzfrom5to7.wav");
    delete core;
    EXPECT_EQ(fingerprint.code, siren::CoreStatus::OK);
}

TEST(CoreTest, Params)
{
    auto core = test_params::CreateCore();
    auto fingerprint = core->make_fingerprint("../audio/jazzfrom5to7.wav");
    delete core;
    EXPECT_EQ(fingerprint.code, siren::CoreStatus::OK);
}

TEST(WrapperTest, Env)
{
    setenv("WINDOW_FUNCTION", "Blackman", 1);
    setenv("SAMPLING_RATE", "512", 1);

    auto wrapper = siren::client::ClientWrapper();
    std::string json_str = wrapper.process_track("../audio/jazzfrom5to7.wav");
    std::size_t found = json_str.find("core failed to fingerprint the track");
    EXPECT_EQ(found, std::string::npos);
}