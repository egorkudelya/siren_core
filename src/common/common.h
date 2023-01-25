#pragma once

namespace siren
{
    enum class EngineStatus
    {
        OK = 0,
        TooSilent = -1,
        TargetFileDoesNotExist = -2
    };
}