#include "http_client.h"
#include "restclient-cpp/restclient.h"

namespace siren::client
{

    Response CppRestClient::get(const std::string& url)
    {
        ::RestClient::Response response = ::RestClient::get(url);
        return Response{response.code, response.body};
    }

    Response CppRestClient::post(const std::string& url, const std::string& content, const std::string& json)
    {
        ::RestClient::Response response = ::RestClient::post(url, content, json);
        return Response{response.code, response.body};
    }
}// namespace siren::client
