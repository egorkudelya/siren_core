#pragma once

#include <string>
#include <fingerprint.grpc.pb.h>
#include "../abstract_http_client.h"
#include "grpc_request.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using fingerprint::SirenFingerprint;
using fingerprint::FindTrackByFingerprintRequest;
using fingerprint::FindTrackByFingerprintResponse;

namespace siren::client
{
    struct GrpcClientConfig : public AbstractHttpClientConfig
    {
        explicit GrpcClientConfig(const std::string& host);
        std::string m_host;
    };

    class GrpcClient : public AbstractHttpClient
    {
    public:
        explicit GrpcClient(const std::shared_ptr<GrpcClientConfig>& config);
        Response req_process_track(const RequestPtr& request) override;

    private:
        Response send_fingerprint(Fingerprint<>&& payload);

    private:
        std::string m_host;
        std::unique_ptr<SirenFingerprint::Stub> m_stub;
    };
}// namespace siren::client