#pragma once

#include "../siren.h"
#include "http_client.h"

namespace siren::client
{
    class ClientWrapper
    {
    public:
        ClientWrapper();
        ClientWrapper(ClientWrapper&& other) noexcept;
        ClientWrapper& operator=(ClientWrapper&& other) noexcept;

        std::string process_track(const std::string& track_path, const std::string& url);

    private:
        std::unique_ptr<SirenCore> m_engine;
        std::unique_ptr<IRestClient> m_rest_client;
    };
}// namespace siren::client