#include "client_wrapper.h"

namespace siren::client
    {
        std::unique_ptr<SirenEngine> CreateEngine()
        {
            EngineSpecification spec;
            return std::make_unique<SirenEngine>(std::move(spec));
        }

        ClientWrapper::ClientWrapper()
        {
            m_engine = CreateEngine();
            m_rest_client = std::make_unique<CppRestClient>();
        }

        std::string ClientWrapper::process_track(const std::string& track_path, const std::string& url)
        {
            siren::EngineReturnType eng_response = m_engine->make_fingerprint(track_path);
            std::string response;

            if (eng_response.code != EngineStatus::OK)
            {
                response = "{\"http_code\": null,"
                           " \"body\": null, "
                           "\"engine_code\": " + std::to_string((int)eng_response.code) + "}";
                return response;
            }

            siren::json::Json fingerprint_obj = siren::json::to_json(eng_response.fingerprint);
            std::string json_str = siren::json::dumps(fingerprint_obj);

            Response serv_response = m_rest_client->post(url, "application/json", json_str);

            std::string body = serv_response.code != 200 ? "null" : serv_response.m_body;

            response = "{\"http_code\": " + std::to_string(serv_response.code)
                    + ",\"engine_code\": " + std::to_string((int)eng_response.code)
                    + ",\"body\": " + body + "}";

            return response;
        }
    }// namespace siren
