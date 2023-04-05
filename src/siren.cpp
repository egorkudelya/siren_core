#include "siren.h"

namespace siren
{
    SirenCore* SirenCore::s_instance = nullptr;

    SirenCore::SirenCore(siren::CoreSpecification&& core_specification)
        : m_specification(std::move(core_specification))
    {
        release_assert(!s_instance, "Siren core already exists");
        s_instance = this;
    }

    CoreReturnType SirenCore::make_fingerprint(const std::string& track_path) const
    {
        const unsigned int target_sampling_rate = m_specification.core_params.target_sampling_rate;
        const unsigned int target_channel_count = m_specification.core_params.target_channel_count;
        const size_t target_window_size = m_specification.core_params.target_window_size;
        const size_t target_net_size = m_specification.core_params.target_net_size;
        const float target_peak_threshold = m_specification.core_params.target_peak_threshold;
        const size_t min_peak_count = m_specification.core_params.min_peak_count;
        siren::WindowFunction target_window_function = m_specification.core_params.target_window_function;

        CoreReturnType return_obj;

        std::unique_ptr<siren::FFT> fft = std::make_unique<siren::KissFFT>(target_window_function, target_window_size);
        std::unique_ptr<siren::audio::PCM> audio = std::make_unique<siren::audio::PCM>(track_path, target_channel_count, target_sampling_rate);

        if (!audio->config_decoder())
        {
            return_obj.code = CoreStatus::TargetFileDoesNotExist;
            return return_obj;
        }

        siren::PeakSpectrogram spectrogram(std::move(audio), std::move(fft), target_peak_threshold);
        siren::Fingerprint fingerprint;

        CoreStatus code = fingerprint.make_fingerprint(std::move(spectrogram), target_net_size, min_peak_count);
        return_obj.code = code;
        return_obj.fingerprint = std::move(fingerprint);

        return return_obj;
    }
}// namespace siren