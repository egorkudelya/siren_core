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
        size_t ws = window_size - 1;
        std::vector<float> window_func(window_size);
        for (size_t i = 0; i < window_size; i++)
        {
            window_func[i] = 0.5f * (1 - cos(2.0 * M_PI * (i / ws)));
        }
        m_window_func = std::move(window_func);
    }

    void FFT::config_hamming_window(size_t window_size)
    {
        std::cerr << "Hamming window function is not yet supported" << std::endl;
    }

    void FFT::config_blackman_window(size_t window_size)
    {
        std::cerr << "Blackman window function is not yet supported" << std::endl;
    }

    size_t FFT::get_fft_size() const
    {
        assert(m_fft_out_r.size() == m_fft_out_i.size());
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
        assert(window_size < INT_MAX);
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
        assert(window.size() == m_window_size);

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