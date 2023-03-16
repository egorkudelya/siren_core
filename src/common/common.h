#pragma once
#include <string>

#define release_assert(exp, message) { if (!(exp)) {std::cerr << (message) << std::endl; std::abort();}}

namespace siren
{
    enum class CoreStatus
    {
        OK = 0,
        PeaksTooSparse = -1,
        TargetFileDoesNotExist = -2
    };

    std::string getenv(const std::string& name);
}