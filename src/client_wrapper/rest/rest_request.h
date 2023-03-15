#pragma once
#include "../abstract_request.h"

namespace siren::client
{
    struct RestRequest : public AbstractRequest
    {
        RestRequest(std::string&& payload, const std::string& url, const std::string& ctype, RequestType req_type);

        std::string m_url;
        std::string m_ctype;
        RequestType m_req_type;
        std::string m_payload;
    };
}