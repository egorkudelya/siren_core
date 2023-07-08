#pragma once
#include <iostream>
#include <string>
#include <cstdlib>

#define release_assert(exp, message) { if (!(exp)) {std::cerr << (message) << std::endl; std::abort();}}

namespace siren
{
    enum class CoreStatus
    {
        OK = 0,
        PeaksTooSparse = -1,
        TargetFileDoesNotExist = -2,
        CoreParamsFatalError = -3,
        CoreParamsLogicError = -4
    };

    std::string getenv(const std::string& name);
}