#include "spectrogram.h"

namespace siren
{

    Spectrogram::Spectrogram(std::unique_ptr<siren::audio::PCM> pcm, std::unique_ptr<siren::FFT> fft)
        : m_fft_core(std::move(fft)),
          m_pcm(std::move(pcm)),
          m_window_size(m_fft_core->get_window_size())
    {

        set_sampling_rate();
        set_nyquist_freq();
        set_window_counter();
        set_time_resolution();
        set_freq_resolution();
        init_spectrogram();

        make_linear_spectrogram();
    }

    const Eigen::SparseMatrix<float, Eigen::RowMajor>& Spectrogram::get_spectrogram_view() const
    {
        return m_spectrogram;
    }

    float Spectrogram::get_time_resolution() const
    {
        return m_time_resolution;
    }

    float Spectrogram::get_freq_resolution() const
    {
        return m_freq_resolution;
    }

    void Spectrogram::make_linear_spectrogram()
    {
        std::vector<Triplet> triplet_list;
        for (size_t frame_idx = 0; frame_idx < m_pcm->get_frame_count() - m_window_size / 2; frame_idx += m_window_size)
        {
            std::vector<float> window(m_window_size);
            for (size_t w_idx = 0; w_idx < m_window_size; w_idx++)
            {
                if (frame_idx == 0)
                {
                    window[w_idx] = (*m_pcm)[frame_idx + w_idx];
                    continue;
                }
                // 50% overlapping window
                window[w_idx] = (*m_pcm)[frame_idx + w_idx - m_window_size / 2];
            }
            m_fft_core->process_window(std::move(window));
            float ts = m_time_resolution * frame_idx;

            for (size_t b_idx = 0; b_idx < m_fft_core->get_fft_size(); b_idx++)
            {
                if (static_cast<float>(b_idx) / m_window_size * m_sampling_rate >= m_nyquist_component)
                {
                    break;
                }
                FreqBin freq_bin(
                    b_idx,
                    m_window_size,
                    m_sampling_rate,
                    m_fft_core->get_real_by_idx(b_idx),
                    m_fft_core->get_imag_by_idx(b_idx));

                triplet_list.emplace_back(Triplet(freq_bin.get_frequency(), floor(ts), freq_bin.get_magnitude()));
            }
        }
        m_spectrogram.setFromTriplets(triplet_list.begin(), triplet_list.end());
    }

    void Spectrogram::set_sampling_rate()
    {
        m_sampling_rate = m_pcm->get_sampling_rate();
    }

    void Spectrogram::set_nyquist_freq()
    {
        m_nyquist_component = m_sampling_rate / 2;
    }

    void Spectrogram::set_window_counter()
    {
        m_window_counter = floor(m_pcm->get_frame_count() / m_window_size);
    }

    void Spectrogram::set_time_resolution()
    {
        m_time_resolution = m_pcm->get_length_in_ms() / m_pcm->get_frame_count();
    }

    void Spectrogram::set_freq_resolution()
    {
        m_freq_resolution = (float)m_sampling_rate / (float)m_window_size;
    }

    void Spectrogram::init_spectrogram()
    {
        size_t last_ts = m_window_counter * m_window_size * m_time_resolution;
        size_t non_zero_est = (m_nyquist_component / m_freq_resolution) * (last_ts / m_time_resolution);

        m_spectrogram = Eigen::SparseMatrix<float, Eigen::RowMajor>(ceil(m_nyquist_component), last_ts + 1);
        m_spectrogram.reserve(non_zero_est);
    }

    size_t Spectrogram::get_window_size() const
    {
        return m_window_size;
    }

    unsigned int Spectrogram::get_sampling_rate() const
    {
        return m_sampling_rate;
    }

    size_t Spectrogram::rows() const
    {
        return m_spectrogram.rows();
    }

    size_t Spectrogram::cols() const
    {
        return m_spectrogram.cols();
    }


    PeakSpectrogram::PeakSpectrogram(std::unique_ptr<siren::audio::PCM> pcm, std::unique_ptr<siren::FFT> fft, float zscore)
        : Spectrogram(std::move(pcm), std::move(fft)), m_zscore(zscore)
    {
        init_peak_spectrogram();
        make_peak_spectrogram();
    }

    void PeakSpectrogram::init_peak_spectrogram()
    {
        m_peak_spectrogram = Eigen::SparseMatrix<float, Eigen::RowMajor>(this->rows(), this->cols());
    }

    const Eigen::SparseMatrix<float, Eigen::RowMajor>& PeakSpectrogram::get_peak_spec_view() const
    {
        return m_peak_spectrogram;
    }

    std::vector<std::pair<size_t, size_t>> PeakSpectrogram::get_occupied_indices()
    {
        std::vector<std::pair<size_t, size_t>> indices;
        for (size_t i = 0; i < m_peak_spectrogram.outerSize(); i++)
        {
            for (Eigen::SparseMatrix<float, Eigen::RowMajor>::InnerIterator it(m_peak_spectrogram, i); it; ++it)
            {
                indices.emplace_back(it.row(), it.col());
            }
        }
        return indices;
    }

    void PeakSpectrogram::make_peak_spectrogram()
    {
        std::vector<Triplet> triplet_list;
        auto distribution = log_distribution(this->rows(), 8);

        auto flattenEigenBlock = [](const Eigen::SparseMatrix<float, Eigen::RowMajor>& block, auto& vec)
        {
            for (size_t i = 0; i < block.outerSize(); i++)
            {
                for (Eigen::SparseMatrix<float, Eigen::RowMajor>::InnerIterator it(block, i); it; ++it)
                {
                    vec.emplace_back(it.value());
                }
            }
        };

        auto spec = this->get_spectrogram_view();
        for (int i = 0; i < distribution.size() - 1; i++)
        {
            int row_range = distribution[i + 1] - distribution[i];

            Eigen::SparseMatrix<float, Eigen::RowMajor> block = spec.block(distribution[i], 0, row_range, spec.cols());
            block.prune([](size_t, size_t, float val) {
                return val >= 0.0;
            });

            std::vector<float> flat_block;
            flattenEigenBlock(block, flat_block);

            double median = get_median(flat_block);
            double mad = get_mad(flat_block, median);

            block.prune([this, median, mad](size_t, size_t, float val) {
                return get_zscore_of_peak(median, mad, val) >= m_zscore;
            });

            for (size_t j = 0; j < block.outerSize(); j++)
            {
                for (auto it = Eigen::SparseMatrix<float, Eigen::RowMajor>::InnerIterator(block, j); it; ++it)
                {
                    int freq = distribution[i] + it.row();
                    size_t ts = it.col();
                    triplet_list.emplace_back(Triplet(freq, ts, 255.0f));
                }
            }
        }

        m_peak_spectrogram.setFromTriplets(triplet_list.begin(), triplet_list.end());
    }

    std::vector<unsigned int> PeakSpectrogram::log_distribution(size_t end_index, int bands)
    {
        double scale = end_index / log(1.0 + bands);
        std::vector<unsigned int> intervals;
        for (int i = bands - 1; i >= 0; i--)
        {
            int upper_bound = log(2.0 + i) * scale;
            intervals.push_back(end_index - upper_bound);
        }
        intervals.push_back(end_index);
        return intervals;
    }

}// namespace siren