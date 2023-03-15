#include "rest_client.h"
#include <restclient-cpp/restclient.h>

namespace siren::client
{
    RestClient::RestClient()
        : AbstractHttpClient(HttpClientType::REST)
    {
    }

    Response RestClient::get(const std::string& url)
    {
        ::RestClient::Response response = ::RestClient::get(url);
        return Response{response.code, response.body};
    }

    Response RestClient::post(const std::string& url, const std::string& content, const std::string& json)
    {
        ::RestClient::Response response = ::RestClient::post(url, content, json);
        return Response{response.code, response.body};
    }

    Response RestClient::req_process_track(const RequestPtr& request)
    {
        auto req = std::static_pointer_cast<RestRequest>(request);
        switch (req->m_req_type)
        {
            case RequestType::POST:
                return post(req->m_url, req->m_ctype, req->m_payload);

            case RequestType::GET:
                return get(req->m_url);
        }
    }

}// namespace siren::client