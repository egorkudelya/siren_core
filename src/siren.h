#pragma once

#include <iostream>
#include <memory>

#include "decoder/pcm.h"
#include "fft/fft.h"
#include "entities/fingerprint.h"
#include "entities/spectrogram.h"

namespace siren
{

    struct CoreParameters {
        /**
        * default target parameters can be overwritten by client in CreateCore
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
        float           target_peak_threshold = 1.85;
        size_t          min_peak_count = 160;
        size_t          target_net_size = 80;
        WindowFunction  target_window_function = WindowFunction::Hanning;
    };

    struct CoreSpecification {
        std::string name = "Siren Fingerprinting Core";
        CoreParameters core_params;
    };

    struct CoreReturnType {

        explicit operator bool() const
        {
            return code == CoreStatus::OK;
        }

        Fingerprint<> fingerprint{};
        CoreStatus code;
    };

    class SirenCore
    {
    public:
        explicit SirenCore(CoreSpecification&& core_specification);

        static SirenCore& get_instance()
        {
            return *s_instance;
        }

        [[nodiscard]] CoreReturnType make_fingerprint(const std::string& track_path) const;

    private:
        CoreSpecification m_specification;

    private:
        static SirenCore* s_instance;
    };

    SirenCore* CreateCore();
}// namespace siren