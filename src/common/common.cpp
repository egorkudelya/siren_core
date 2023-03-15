#include "common.h"

namespace siren
{
    std::string getenv(const std::string& name)
    {
        const char* val = std::getenv(name.c_str());
        return val == nullptr ? std::string{} : std::string{val};
    }
}// namespace siren