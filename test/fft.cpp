#include <iostream>
#include <gtest/gtest.h>

#include "../src/decoder/pcm.h"
#include "../src/fft/fft.h"

TEST(FFTest, ProcessWindow)
{
    std::string test_path = "../audio/lullaby.wav";
    siren::audio::PCM audio(test_path, 1, 11025);
    const int window_size = 1024;

    EXPECT_TRUE(audio.config_decoder());

    std::shared_ptr<siren::KissFFT> fft = std::make_shared<siren::KissFFT>(siren::WindowFunction::Hanning, window_size);
    for (int frame_idx = 0; frame_idx < audio.get_frame_count() - window_size / 2; frame_idx += window_size)
    {
        std::vector<float> window(window_size);
        for (int wframe_idx = 0; wframe_idx < window_size; wframe_idx++)
        {
            if (frame_idx == 0)
            {
                window[wframe_idx] = audio[frame_idx + wframe_idx];
                continue;
            }
            // 50% overlapping window
            window[wframe_idx] = audio[frame_idx + wframe_idx - window_size / 2];
        }
        fft->process_window(std::move(window));
        EXPECT_EQ(fft->get_fft_size(), window_size);
    }
}
