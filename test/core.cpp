#include <iostream>
#include <gtest/gtest.h>
#include "../src/siren.h"

namespace test_default
{
    std::unique_ptr<siren::SirenCore> CreateCore()
    {
        siren::CoreSpecification spec;
        return std::make_unique<siren::SirenCore>(std::move(spec));
    }
}

namespace test_params
{
    std::unique_ptr<siren::SirenCore> CreateCore()
    {
        siren::CoreSpecification spec;

        spec.core_params.target_window_size = 512;
        spec.core_params.target_peak_threshold = 2;
        spec.core_params.target_window_function = siren::WindowFunction::Blackman;

        return std::make_unique<siren::SirenCore>(std::move(spec));
    }
}

namespace test_env
{
    std::unique_ptr<siren::SirenCore> CreateCore()
    {
        siren::CoreSpecification spec;

        std::string window_function = getenv("WINDOW_FUNCTION");
        std::string sampling_rate = getenv("SAMPLING_RATE");

        if (!sampling_rate.empty())
        {
            spec.core_params.target_sampling_rate = std::stol(sampling_rate);
        }
        if (!window_function.empty())
        {
            if (window_function == "Hamming")
            {
                spec.core_params.target_window_function = siren::WindowFunction::Hamming;
            }
            else if (window_function == "Blackman")
            {
                spec.core_params.target_window_function = siren::WindowFunction::Blackman;
            }
        }

        return std::make_unique<siren::SirenCore>(std::move(spec));
    }
}

TEST(CoreTest, DefaultParams)
{
    auto core = test_default::CreateCore();
    auto fingerprint = core->make_fingerprint("../audio/jazzfrom5to7.wav");
    EXPECT_EQ(fingerprint.code, siren::CoreStatus::OK);
}

TEST(CoreTest, Params)
{
    auto core = test_params::CreateCore();
    auto fingerprint = core->make_fingerprint("../audio/jazzfrom5to7.wav");
    EXPECT_EQ(fingerprint.code, siren::CoreStatus::OK);
}

TEST(CoreTest, Env)
{
    setenv("WINDOW_FUNCTION", "Blackman", 1);
    setenv("SAMPLING_RATE", "512", 1);

    auto core = test_params::CreateCore();
    auto fingerprint = core->make_fingerprint("../audio/jazzfrom5to7.wav");
    EXPECT_EQ(fingerprint.code, siren::CoreStatus::OK);
}