#pragma once

#include <cassert>
#include <string>
#include "rest_request.h"
#include "../abstract_http_client.h"

namespace siren::client
{
    class RestClient : public AbstractHttpClient
    {

    public:
        RestClient();
        Response req_process_track(const RequestPtr& request) override;

    private:
        Response get(const std::string& url);
        Response post(const std::string& url, const std::string& content, const std::string& json);
    };
}