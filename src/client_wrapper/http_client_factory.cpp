#include "http_client_factory.h"
#include <memory>

namespace siren::client
{
    HttpClientPtr HttpClientFactory::CreateHttpClient(HttpClientType client_type, const HttpClientConfigPtr& config)
    {
        if (client_type == HttpClientType::RPC)
        {
            using RpcConfigType = siren::client::GrpcClientConfig;
            std::shared_ptr<RpcConfigType> grpc_config = std::static_pointer_cast<RpcConfigType>(config);
            return std::make_unique<siren::client::GrpcClient>(grpc_config);
        }
        return std::make_unique<siren::client::RestClient>();
    }
}