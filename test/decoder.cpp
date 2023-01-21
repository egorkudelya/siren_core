#include <iostream>
#include <gtest/gtest.h>

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
    std::string test_path = "../audio/lullaby.wav";
    siren::audio::PCM audio(test_path, 1, 11025);

    EXPECT_TRUE(audio.config_decoder());
    EXPECT_EQ(audio.get_sampling_rate(), 11025);
    EXPECT_EQ(audio.get_frame_count(), audio.get_sampling_rate() * (audio.get_length_in_ms() / 1000));
    std::cout << audio.get_length_in_ms() << std::endl;
}