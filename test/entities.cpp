#include <iostream>
#include <gtest/gtest.h>
#include "../src/entities/spectrogram.h"
#include "../src/entities/fingerprint.h"

siren::PeakSpectrogram init_spectrogram(const std::string& audio_path, int sampling_rate, int window_size, int channel_count)
{
    std::unique_ptr<siren::KissFFT> fft = std::make_unique<siren::KissFFT>(siren::WindowFunction::Hanning, window_size);
    std::unique_ptr<siren::audio::PCM> audio = std::make_unique<siren::audio::PCM>(audio_path, channel_count, sampling_rate);

    EXPECT_TRUE(audio->config_decoder());
    return {std::move(audio), std::move(fft)};
}

TEST(Spectrogram, Trivial)
{
    const std::string path = "../audio/jazzfrom5to7.wav";
    const int sampling_rate = 11025;
    const int window_size = 1024;
    const int channels = 1;

    siren::PeakSpectrogram spectrogram = init_spectrogram(path, sampling_rate, window_size, channels);

    EXPECT_EQ(spectrogram.get_sampling_rate(), sampling_rate);
    EXPECT_EQ(spectrogram.rows(), floor(sampling_rate/2));
}

TEST(Fingerprint, Trivial)
{
    const std::string path = "../audio/jazzfrom5to7.wav";
    const int sampling_rate = 11025;
    const int window_size = 1024;
    const int channels = 1;
    const int net_size = 80;
    const int min_peak_count = 160;

    siren::PeakSpectrogram spectrogram = init_spectrogram(path, sampling_rate, window_size, channels);
    siren::Fingerprint<> fingerprint;
    fingerprint.make_fingerprint(std::move(spectrogram), net_size, min_peak_count);

    std::unordered_map<size_t, size_t> map(fingerprint.begin(), fingerprint.end());
    EXPECT_EQ(map.size(), fingerprint.get_hashes().size());
}