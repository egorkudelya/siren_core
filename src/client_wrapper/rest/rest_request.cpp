#include "rest_request.h"

namespace siren::client
{
    RestRequest::RestRequest(std::string&& payload, const std::string& url, const std::string& ctype, RequestType req_type)
        : AbstractRequest()
        , m_payload(std::move(payload))
        , m_req_type(req_type)
        , m_ctype(ctype)
        , m_url(url)
    {
    }

}