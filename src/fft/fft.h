#pragma once

#include <iostream>
#include <cmath>
#include <climits>
#include <vector>
#include <kiss_fft.h>
#include "../common/common.h"

namespace siren
{
    enum class WindowFunction
    {
        Hanning,
        Hamming,
        Blackman
    };

    class FFT
    {

    public:
        FFT(WindowFunction w_func, size_t window_size);
        virtual ~FFT() = default;

        virtual void process_window(std::vector<float>&& window) = 0;

        [[nodiscard]] size_t get_fft_size() const;
        [[nodiscard]] size_t get_window_size() const;
        [[nodiscard]] float get_real_by_idx(size_t i) const;
        [[nodiscard]] float get_imag_by_idx(size_t i) const;

    private:
        void config_hanning_window(size_t window_size);
        void config_hamming_window(size_t window_size);
        void config_blackman_window(size_t window_size);

    protected:
        size_t m_window_size;

        std::vector<float> m_window_func;
        std::vector<float> m_fft_out_r;
        std::vector<float> m_fft_out_i;
    };

    class KissFFT : public FFT
    {

    public:
        KissFFT(WindowFunction w_func, size_t window_size);
        ~KissFFT();

        void process_window(std::vector<float>&& window) override;

    private:
        kiss_fft_cpx* fft_in;
        kiss_fft_cpx* fft_out;
        kiss_fft_cfg config;
    };
}// namespace siren