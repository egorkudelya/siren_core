#include "client_wrapper.h"
#include <charconv>

namespace siren::client
{
    SirenCore* CreateCore()
    {
        CoreSpecification spec;

        std::string sampling_rate = getenv("SAMPLING_RATE");
        std::string channel_count = getenv("CHANNEL_COUNT");
        std::string window_size = getenv("WINDOW_SIZE");
        std::string zscore = getenv("CORE_PEAK_ZSCORE");
        std::string band_count = getenv("CORE_FREQ_BAND_COUNT");
        std::string min_peak_count = getenv("MIN_PEAK_COUNT");
        std::string block_size = getenv("CORE_BLOCK_SIZE");
        std::string window_function = getenv("WINDOW_FUNCTION");
        std::string stride_coeff = getenv("CORE_BLOCK_STRIDE_COEFF");

        if (!sampling_rate.empty())
        {
            convert_to_type(sampling_rate, spec.core_params.target_sampling_rate);
        }
        if (!channel_count.empty())
        {
            convert_to_type(channel_count, spec.core_params.target_channel_count);
        }
        if (!window_size.empty())
        {
            convert_to_type(window_size, spec.core_params.target_window_size);
        }
        if (!band_count.empty())
        {
            convert_to_type(band_count, spec.core_params.target_band_count);
        }
        if (!zscore.empty())
        {
        #ifndef __clang__
            convert_to_type(zscore, spec.core_params.target_zscore);
        #else
            float zscore_f = std::stof(zscore);
            release_assert(!isnan(zscore_f), "zscore_f is nan");
            spec.core_params.target_zscore = zscore_f;
        #endif
        }
        if (!min_peak_count.empty())
        {
            convert_to_type(min_peak_count, spec.core_params.min_peak_count);
        }
        if (!block_size.empty())
        {
            convert_to_type(block_size, spec.core_params.target_block_size);
        }
        if (!stride_coeff.empty())
        {
        #ifndef __clang__
            convert_to_type(stride_coeff, spec.core_params.stride_coeff);
        #else
            float stride_coeff_f = std::stof(stride_coeff);
            release_assert(!isnan(stride_coeff_f), "stride_coeff_f is nan");
            spec.core_params.stride_coeff = stride_coeff_f;
        #endif
        }
        if (!window_function.empty())
        {
            if (window_function == "Hamming")
            {
                spec.core_params.target_window_function = WindowFunction::Hamming;
            }
            else if (window_function == "Blackman")
            {
                spec.core_params.target_window_function = WindowFunction::Blackman;
            }
        }
        return new SirenCore(std::move(spec));
    }

    ClientWrapper::ClientWrapper()
    {
        m_core = CreateCore();
    }

    ClientWrapper::~ClientWrapper()
    {
        delete m_core;
    }

    std::string ClientWrapper::process_track(const std::string& track_path)
    {
        auto generate_response = [](const std::string& core_code, const std::string& body)
        {
            return "{"
                   "\"body\": " + body + ","
                   "\"core_code\": " + core_code +
                   "}";
        };
        siren::CoreReturnType core_response = m_core->make_fingerprint(track_path);
        if (!core_response)
        {
            return generate_response(std::to_string((int)core_response.code), "core failed to fingerprint the track");
        }
        siren::json::Json fingerprint_obj = siren::json::to_json(core_response.fingerprint);
        return generate_response(std::to_string((int)core_response.code), siren::json::dumps(fingerprint_obj));
    }

}// namespace siren::client
