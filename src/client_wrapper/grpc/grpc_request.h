#pragma once
#include "../abstract_request.h"
#include "../../entities/fingerprint.h"

namespace siren::client
{
    struct GrpcRequest : public AbstractRequest
    {
        GrpcRequest(Fingerprint<>&& payload);

        Fingerprint<> m_payload;
    };

}