#include "freq_bin.h"

namespace siren
{
    FreqBin::FreqBin(size_t window_index, size_t window_size, unsigned int sampling_rate, float r, float i)
    {
        m_freq = (static_cast<float>(window_index) / window_size) * sampling_rate;

        if (std::fpclassify(r) == FP_ZERO && std::fpclassify(i) == FP_ZERO)
        {
            m_mag = 0.0;
            return;
        }
        // return 20 * log10(std::sqrt(std::pow(r, 2) + std::pow(i, 2))); dB
        m_mag = std::sqrt(std::pow(r, 2) + std::pow(i, 2));
    }

    [[nodiscard]] float FreqBin::get_frequency() const
    {
        return m_freq;
    }

    [[nodiscard]] float FreqBin::get_magnitude() const
    {
        return m_mag;
    }
}// namespace siren