#include "client_wrapper.h"

namespace siren::client
    {
        std::unique_ptr<SirenCore> CreateCore()
        {
            CoreSpecification spec;
            return std::make_unique<SirenCore>(std::move(spec));
        }

        ClientWrapper::ClientWrapper()
        {
            m_engine = CreateCore();
            m_rest_client = std::make_unique<CppRestClient>();
        }

        ClientWrapper::ClientWrapper(ClientWrapper&& other) noexcept
        {
            m_engine = std::move(other.m_engine);
            m_rest_client = std::move(other.m_rest_client);
        }

        ClientWrapper& ClientWrapper::operator=(ClientWrapper&& other) noexcept
        {
            m_engine = std::move(other.m_engine);
            m_rest_client = std::move(other.m_rest_client);
            return *this;
        }

        std::string ClientWrapper::process_track(const std::string& track_path, const std::string& url)
        {
            siren::CoreReturnType core_response = m_engine->make_fingerprint(track_path);
            std::string response;

            if (!core_response)
            {
                response = "{\"http_code\": null,"
                            "\"body\": null, "
                            "\"engine_code\": " + std::to_string((int)core_response.code) + "}";
                return response;
            }

            siren::json::Json fingerprint_obj = siren::json::to_json(core_response.fingerprint);
            std::string json_str = siren::json::dumps(fingerprint_obj);

            Response serv_response = m_rest_client->post(url, "application/json", json_str);
            std::string body = serv_response.code != 200 ? "null" : serv_response.m_body;

            response = "{\"http_code\": " + std::to_string(serv_response.code)
                    + ",\"engine_code\": " + std::to_string((int)core_response.code)
                    + ",\"body\": " + body + "}";

            return response;
        }
    }// namespace siren
