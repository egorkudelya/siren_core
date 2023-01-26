#pragma once

#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"

namespace siren::client
{
    struct Response {
        Response() = default;
        Response(int code, const std::string& body)
            : code(code), m_body(body)
        {
        }

        int code;
        std::string m_body;
    };

    class IRestClient
    {
    public:
        IRestClient() = default;
        virtual ~IRestClient() = default;

        virtual Response get(const std::string& url) = 0;
        virtual Response post(const std::string& url, const std::string& content, const std::string& json) = 0;
    };

    class CppRestClient : public IRestClient
    {
    public:
        CppRestClient() = default;

        Response get(const std::string& url) override;
        Response post(const std::string& url, const std::string& content, const std::string& json) override;
    };
}// namespace siren::client