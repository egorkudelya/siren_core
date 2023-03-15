#include "abstract_http_client.h"

AbstractHttpClient::AbstractHttpClient(HttpClientType type)
    : m_type(type)
{
}

AbstractHttpClient::AbstractHttpClient(HttpClientType type, const HttpClientConfigPtr& config)
    : m_type(type),
      m_config(config)
{
}

HttpClientType AbstractHttpClient::get_type() const
{
    return m_type;
}
