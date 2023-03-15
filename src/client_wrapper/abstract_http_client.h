#pragma once
#include <string>
#include <memory>
#include "abstract_request.h"

struct Response
{
    Response() = default;
    Response(int code, const std::string& body)
        : code(code)
        , m_body(body)
    {}

    int code;
    std::string m_body;
};

enum class HttpClientType
{
    REST = 1,
    RPC = 2
};

struct AbstractHttpClientConfig
{
    virtual ~AbstractHttpClientConfig() = default;
};
using HttpClientConfigPtr = std::shared_ptr<AbstractHttpClientConfig>;

class AbstractHttpClient
{
public:
    AbstractHttpClient() = delete;
    explicit AbstractHttpClient(HttpClientType type);
    AbstractHttpClient(HttpClientType type, const HttpClientConfigPtr& config);
    virtual ~AbstractHttpClient() = default;

    virtual Response req_process_track(const RequestPtr& request) = 0;
    HttpClientType get_type() const;

private:
    HttpClientType      m_type;
    HttpClientConfigPtr m_config;
};

using HttpClientPtr = std::shared_ptr<AbstractHttpClient>;
