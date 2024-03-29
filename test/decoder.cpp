#include <iostream>
#include <gtest/gtest.h>
#include <cmath>
#include "../src/decoder/pcm.h"

TEST(DecoderTest, TrivialTrue)
{
    std::string test_path = "../audio/jazzfrom5to7.wav";
    siren::audio::PCM audio(test_path, 1, 11025);

    EXPECT_TRUE(audio.config_decoder());
}

TEST(DecoderTest, TrivialFalse)
{
    std::string test_path = "../audio/none.wav";
    siren::audio::PCM audio(test_path, 1, 11025);

    EXPECT_FALSE(audio.config_decoder());
}

TEST(DecoderTest, CorrectnessCheck)
{
    std::string test_path = "../audio/jazzfrom5to7.wav";
    siren::audio::PCM audio(test_path, 1, 11025);

    EXPECT_TRUE(audio.config_decoder());
    EXPECT_EQ(audio.get_sampling_rate(), 11025);
    EXPECT_EQ(audio.get_frame_count(), std::ceil(audio.get_sampling_rate() * (audio.get_length_in_ms() / 1000)));
    EXPECT_EQ((size_t)audio.get_length_in_ms(), 2025);
}