#pragma once

#include "../siren.h"
#include "http_client.h"

namespace siren::client
{
    class ClientWrapper
    {
    public:
        ClientWrapper();
        std::string process_track(const std::string& track_path, const std::string& url);

    private:
        std::unique_ptr<SirenEngine> m_engine;
        std::unique_ptr<IRestClient> m_rest_client;
    };
}// namespace siren::client