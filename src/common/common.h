#pragma once
#include <string>

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