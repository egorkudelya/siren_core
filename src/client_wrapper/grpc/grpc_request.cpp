#include "grpc_request.h"

namespace siren::client
{
    GrpcRequest::GrpcRequest(Fingerprint<>&& payload)
        : AbstractRequest()
        , m_payload(std::move(payload))
    {
    }

}