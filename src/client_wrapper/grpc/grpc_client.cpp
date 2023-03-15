#include <grpcpp/grpcpp.h>
#include "grpc_client.h"

namespace siren::client
{
    GrpcClientConfig::GrpcClientConfig(const std::string& host)
        : m_host(host)
    {
    }

    GrpcClient::GrpcClient(const std::shared_ptr<GrpcClientConfig>& config)
    : AbstractHttpClient(HttpClientType::RPC, config)
    {
        m_host = config->m_host;
        auto channel_creds = grpc::SslCredentials(grpc::SslCredentialsOptions());
        auto channel = grpc::CreateChannel(m_host, channel_creds);
        m_stub = std::move(std::make_unique<SirenFingerprint::Stub>(channel));
    }

    Response GrpcClient::req_process_track(const RequestPtr& request)
    {
        auto req = std::static_pointer_cast<GrpcRequest>(request);
        return send_fingerprint(std::forward<Fingerprint<>>(req->m_payload));
    }

    Response GrpcClient::send_fingerprint(Fingerprint<>&& payload)
    {
        FindTrackByFingerprintRequest req;
        FindTrackByFingerprintResponse res;

        ClientContext context;
        context.set_compression_algorithm(GRPC_COMPRESS_DEFLATE);

        auto& map = *req.mutable_fingerprint();
        using ProtoMapType = decltype(std::remove_reference_t<decltype(map)>{});

        ProtoMapType proto_map(payload.begin(), payload.end());
        map = std::move(proto_map);

        Status status = m_stub->FindTrackByFingerprint(&context, req, &res);
        if (status.ok())
        {
            return Response{200, res.song_metadata()};
        }
        return Response{status.error_code(), status.error_message()};
    }

}
