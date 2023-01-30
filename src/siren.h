#pragma once

#include <iostream>
#include <memory>

#include "decoder/pcm.h"
#include "fft/fft.h"
#include "entities/fingerprint.h"
#include "entities/spectrogram.h"

namespace siren
{

    struct EngineParameters {
        /**
        * default target parameters can by overridden by client in CreateEngine
        */

        enum class HashingPattern
        {
            Sequential,
            Triangular,
            Rectangular
        };

        unsigned int    target_sampling_rate = 11025;
        unsigned int    target_channel_count = 1;
        size_t          target_window_size = 1024;
        float           target_peak_threshold = 1.65;
        size_t          min_peak_count = 160;
        size_t          target_net_size = 80;
        WindowFunction  target_window_function = WindowFunction::Hanning;
    };

    struct EngineSpecification {
        std::string name = "Siren Fingerprinting Engine";
        EngineParameters engine_params;
    };

    struct EngineReturnType {

        explicit operator bool() const
        {
            return code == EngineStatus::OK;
        }

        Fingerprint<siren::PeakSpectrogram> fingerprint{};
        EngineStatus code;
    };

    class SirenEngine
    {
    public:
        explicit SirenEngine(siren::EngineSpecification&& engine_specification);

        static SirenEngine& get_instance()
        {
            return *s_instance;
        }

        [[nodiscard]] EngineReturnType make_fingerprint(const std::string& track_path) const;

    private:
        EngineSpecification m_specification;

    private:
        static SirenEngine* s_instance;
    };

    std::unique_ptr<SirenEngine> CreateEngine();
}// namespace siren