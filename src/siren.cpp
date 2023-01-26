#include "siren.h"

namespace siren
{
    SirenEngine* SirenEngine::s_instance = nullptr;

    SirenEngine::SirenEngine(siren::EngineSpecification&& engine_specification)
        : m_specification(std::move(engine_specification))
    {
        assert(!s_instance && "Siren core already exists");
        s_instance = this;
    }

    EngineReturnType SirenEngine::make_fingerprint(const std::string& track_path) const
    {
        const unsigned int target_sampling_rate = m_specification.engine_params.target_sampling_rate;
        const unsigned int target_channel_count = m_specification.engine_params.target_channel_count;
        const size_t target_window_size = m_specification.engine_params.target_window_size;
        const size_t target_net_size = m_specification.engine_params.target_net_size;
        const float target_peak_threshold = m_specification.engine_params.target_peak_threshold;
        const size_t min_peak_count = m_specification.engine_params.min_peak_count;
        siren::WindowFunction target_window_function = m_specification.engine_params.target_window_function;

        EngineReturnType return_obj;

        std::unique_ptr<siren::KissFFT> fft = std::make_unique<siren::KissFFT>(target_window_function, target_window_size);
        std::unique_ptr<siren::audio::PCM> audio = std::make_unique<siren::audio::PCM>(track_path, target_channel_count, target_sampling_rate);

        if (!audio->config_decoder())
        {
            return_obj.code = EngineStatus::TargetFileDoesNotExist;
            return return_obj;
        }

        siren::PeakSpectrogram spectrogram(std::move(audio), std::move(fft), target_peak_threshold);

        siren::Fingerprint fingerprint;
        EngineStatus code = fingerprint.make_fingerprint(std::move(spectrogram), target_net_size, min_peak_count);

        return_obj.code = code;
        return_obj.fingerprint = std::move(fingerprint);

        return return_obj;
    }
}// namespace siren