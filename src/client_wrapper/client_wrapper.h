#pragma once

#include "../siren.h"
#include "http_client_factory.h"

namespace siren::client
{
    class ClientWrapper
    {
    public:
        ClientWrapper();
        explicit ClientWrapper(const HttpClientPtr& http_client);
        ClientWrapper(ClientWrapper&& other) noexcept;
        ClientWrapper& operator=(ClientWrapper&& other) noexcept;
        ClientWrapper& operator=(const ClientWrapper& other) = delete;
        ClientWrapper(const ClientWrapper& other) = delete;

        std::string process_track(const std::string& track_path, const std::string& url);

    private:
        std::unique_ptr<SirenCore> m_core;
        std::shared_ptr<AbstractHttpClient> m_http_client;
    };
}// namespace siren::client