#pragma once

#include <iostream>
#include <cmath>

namespace siren
{

    class FreqBin
    {
    public:
        FreqBin(size_t window_index, size_t window_size, unsigned int sampling_rate, float r, float i);

        [[nodiscard]] float get_frequency() const;
        [[nodiscard]] float get_magnitude() const;

    private:
        float m_freq;
        float m_mag;
    };
}// namespace siren