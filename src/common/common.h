#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <charconv>

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

    template <typename T>
    void convert_to_type(const std::string& src, T& target)
    {
        using TargetType = std::remove_reference_t<decltype(target)>;
        TargetType value;
        auto [p, ec] = std::from_chars(src.data(), src.data() + src.size(), value);
        if (ec == std::errc())
        {
            target = value;
        }
    }
}