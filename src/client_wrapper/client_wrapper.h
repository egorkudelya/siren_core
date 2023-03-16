#pragma once
#include "../siren.h"

namespace siren::client
{
    class ClientWrapper
    {
    public:
        ClientWrapper();
        ~ClientWrapper();
        std::string process_track(const std::string& track_path);

    private:
        SirenCore* m_core;
    };
}