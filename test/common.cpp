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
        spec.core_params.stride_coeff = 0.2;
        spec.core_params.target_zscore = 2.45;
        s_CorePtr = std::shared_ptr<siren::SirenCore>(new siren::SirenCore(std::move(spec)));
    }

}