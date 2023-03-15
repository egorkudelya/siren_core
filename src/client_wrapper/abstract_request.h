#pragma once

#include <string>
#include <memory>

enum class RequestType
{
    POST,
    GET,
};

struct AbstractRequest
{
    AbstractRequest() = default;
    virtual ~AbstractRequest() = default;
};

using RequestPtr = std::shared_ptr<AbstractRequest>;