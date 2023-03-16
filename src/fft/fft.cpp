#include "fft.h"

namespace siren
{
    FFT::FFT(WindowFunction w_func, size_t window_size)
    {
        m_window_size = window_size;
        switch (w_func)
        {
        case WindowFunction::Hanning:
            config_hanning_window(window_size);
            break;
        case WindowFunction::Hamming:
            config_hamming_window(window_size);
            break;
        case WindowFunction::Blackman:
            config_blackman_window(window_size);
            break;
        }
    }

    void FFT::config_hanning_window(size_t window_size)
    {
        float ws = window_size - 1;
        std::vector<float> window_func(window_size);
        for (size_t i = 0; i < window_size; i++)
        {
            window_func[i] = 0.5f * (1 - cos(2 * M_PI * (i / ws)));
        }
        m_window_func = std::move(window_func);
    }

    void FFT::config_hamming_window(size_t window_size)
    {
        float ws = window_size - 1;
        std::vector<float> window_func(window_size);
        for (size_t i = 0; i < window_size; i++)
        {
            window_func[i] = 0.54f - (0.46f * cos(2 * M_PI * (i / ws)));
        }
        m_window_func = std::move(window_func);
    }

    void FFT::config_blackman_window(size_t window_size)
    {
        float ws = window_size - 1;
        std::vector<float> window_func(window_size);
        for (size_t i = 0; i < window_size; i++)
        {
            window_func[i] = 0.42f - 0.5 * cos((2 * M_PI * i)/(ws - 1)) + 0.08f * cos((4 * M_PI * i)/(ws - 1));
        }
        m_window_func = std::move(window_func);
    }

    size_t FFT::get_fft_size() const
    {
        release_assert(m_fft_out_r.size() == m_fft_out_i.size(), "m_fft_out_r.size() != m_fft_out_i.size()");
        return m_fft_out_r.size();
    }

    float FFT::get_real_by_idx(size_t i) const
    {
        return m_fft_out_r[i];
    }

    float FFT::get_imag_by_idx(size_t i) const
    {
        return m_fft_out_i[i];
    }

    size_t FFT::get_window_size() const
    {
        return m_window_size;
    }

    KissFFT::KissFFT(WindowFunction w_func, size_t window_size)
        : FFT(w_func, window_size)
    {
        release_assert(window_size < INT_MAX, "window_size exceeds INT_MAX limit")
        config = kiss_fft_alloc((int)m_window_size, 0, nullptr, nullptr);
        fft_in = new kiss_fft_cpx[m_window_size];
        fft_out = new kiss_fft_cpx[m_window_size];
    }

    KissFFT::~KissFFT()
    {
        delete[] fft_out;
        delete[] fft_in;
        kiss_fft_free(config);
    }

    void KissFFT::process_window(std::vector<float>&& window)
    {
        release_assert(window.size() == m_window_size, "window.size() != m_window_size");

        std::vector<float> fft_out_r, fft_out_i;
        for (size_t i = 0; i < m_window_size; i++)
        {
            fft_in[i].r = window[i] * m_window_func[i];
            fft_in[i].i = 0.0f;
        }

        kiss_fft(config, fft_in, fft_out);

        for (size_t i = 0; i < m_window_size; i++)
        {
            fft_out_r.push_back(static_cast<float>(fft_out[i].r));
            fft_out_i.push_back(static_cast<float>(fft_out[i].i));
        }

        m_fft_out_r = std::move(fft_out_r);
        m_fft_out_i = std::move(fft_out_i);
    }
}// namespace siren