#pragma once
#include "abstract_http_client.h"
#include "rest/rest_client.h"
#include "grpc/grpc_client.h"

namespace siren::client
{
    class HttpClientFactory
    {
    public:
        static HttpClientPtr CreateHttpClient(HttpClientType client_type, const HttpClientConfigPtr& config);
    };
}