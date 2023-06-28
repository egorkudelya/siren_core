#pragma once

#include "../src/common/common.h"
#include "../src/siren.h"

namespace test_common
{
    class CommonCore
    {
    public:
        static std::shared_ptr<siren::SirenCore>& GetCore();

    private:
        static void CreateCore();

        inline static std::shared_ptr<siren::SirenCore> s_CorePtr;
    };
}