#pragma once

namespace siren
{
    enum class CoreStatus
    {
        OK = 0,
        PeaksTooSparse = -1,
        TargetFileDoesNotExist = -2
    };
}