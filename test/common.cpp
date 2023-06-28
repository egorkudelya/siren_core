#include "common.h"

namespace test_common
{
    std::shared_ptr<siren::SirenCore>& CommonCore::GetCore()
    {
        if (!s_CorePtr)
        {
            CreateCore();
        }
        return s_CorePtr;
    }

    void CommonCore::CreateCore()
    {
        siren::CoreSpecification spec;
        s_CorePtr = std::shared_ptr<siren::SirenCore>(new siren::SirenCore(std::move(spec)));
    }

}