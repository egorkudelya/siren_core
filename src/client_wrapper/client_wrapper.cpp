#include <charconv>
#include "client_wrapper.h"

namespace siren::client
{
    std::unique_ptr<SirenCore> CreateCore()
    {
        CoreSpecification spec;

        std::string sampling_rate = getenv("SAMPLING_RATE");
        std::string channel_count = getenv("CHANNEL_COUNT");
        std::string window_size = getenv("WINDOW_SIZE");
        std::string peak_threshold = getenv("PEAK_THRESHOLD");
        std::string min_peak_count = getenv("MIN_PEAK_COUNT");
        std::string net_size = getenv("NET_SIZE");
        std::string window_function = getenv("WINDOW_FUNCTION");

        auto convert_to_type = [](const std::string& src, auto& target)
        {
            using TargetType = std::remove_reference_t<decltype(target)>;
            TargetType value;
            auto [p, ec] = std::from_chars(src.data(), src.data() + src.size(), value);
            if (ec != std::errc())
            {
                target = value;
            }
        };
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
        if (!peak_threshold.empty())
        {
        #ifndef __clang__
            convert_to_type(peak_threshold, spec.core_params.target_peak_threshold);
        #else
            float peak_threshold_f = std::stof(peak_threshold);
            assert(!isnan(peak_threshold_f));
            spec.core_params.target_peak_threshold = peak_threshold_f;
        #endif
        }
        if (!min_peak_count.empty())
        {
            convert_to_type(min_peak_count, spec.core_params.min_peak_count);
        }
        if (!net_size.empty())
        {
            convert_to_type(net_size, spec.core_params.target_net_size);
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
        return std::make_unique<SirenCore>(std::move(spec));
    }

    ClientWrapper::ClientWrapper()
    {
        m_core = CreateCore();
        m_http_client = HttpClientFactory::CreateHttpClient(HttpClientType::REST, {});
    }

    ClientWrapper::ClientWrapper(const HttpClientPtr& http_client)
    {
        m_core = CreateCore();
        m_http_client = http_client;
    }

    ClientWrapper::ClientWrapper(ClientWrapper&& other) noexcept
    {
        m_core = std::move(other.m_core);
        m_http_client = std::move(other.m_http_client);
    }

    ClientWrapper& ClientWrapper::operator=(ClientWrapper&& other) noexcept
    {
        m_core = std::move(other.m_core);
        m_http_client = std::move(other.m_http_client);
        return *this;
    }

    std::string ClientWrapper::process_track(const std::string& track_path, const std::string& url)
    {
        auto generate_response = [](const std::string& core_code, const std::string& http_code, const std::string& body)
        {
            return "{"
                   "\"http_code\":" + http_code + ","
                   "\"body\":" + body + ","
                   "\"core_code\": " + core_code +
                   "}";
        };

        siren::CoreReturnType core_response = m_core->make_fingerprint(track_path);
        if (!core_response)
        {
            return generate_response(std::to_string((int)core_response.code),
                                     "null",
                                     "core failed to fingerprint the track"
                                     );
        }
        Response serv_response;
        RequestPtr req;
        if (m_http_client->get_type() == HttpClientType::REST)
        {
            siren::json::Json fingerprint_obj = siren::json::to_json(core_response.fingerprint);
            std::string json_str = siren::json::dumps(fingerprint_obj);
            req = std::make_shared<RestRequest>(std::forward<std::string>(json_str), url, "application/json", RequestType::POST);
        }
        else if (m_http_client->get_type() == HttpClientType::RPC)
        {
            req = std::make_shared<GrpcRequest>(std::forward<decltype(core_response.fingerprint)>(core_response.fingerprint));
        }
        else
        {
            return generate_response(std::to_string((int)core_response.code),
                                     "null",
                                     "request configuration failed, reason: unsupported http client type"
                                     );
        }
        serv_response = m_http_client->req_process_track(req);
        std::string body = serv_response.code != 200 ? "null" : serv_response.m_body;

        return generate_response(std::to_string((int)core_response.code), std::to_string(serv_response.code), body);
    }

}// namespace siren::client
