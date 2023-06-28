#pragma once

#include <memory>
#include <vector>
#include <type_traits>

#include <Eigen/Core>
#include <Eigen/Sparse>

#include "../decoder/pcm.h"
#include "../fft/fft.h"
#include "freq_bin.h"

namespace siren
{
    using Triplet = Eigen::Triplet<float>;

    class Spectrogram
    {

    public:
        Spectrogram(std::unique_ptr<siren::audio::PCM> pcm, std::unique_ptr<siren::FFT> fft);

        [[nodiscard]] size_t get_window_size() const;

        [[nodiscard]] unsigned int get_sampling_rate() const;

        [[nodiscard]] float get_time_resolution() const;

        [[nodiscard]] float get_freq_resolution() const;

        [[nodiscard]] size_t rows() const;

        [[nodiscard]] size_t cols() const;

        [[nodiscard]] const Eigen::SparseMatrix<float, Eigen::RowMajor>& get_spectrogram_view() const;

    private:
        void init_spectrogram();

        void make_linear_spectrogram();

        void set_sampling_rate();

        void set_nyquist_freq();

        void set_window_counter();

        void set_time_resolution();

        void set_freq_resolution();

    private:
        std::unique_ptr<siren::audio::PCM> m_pcm;
        std::unique_ptr<siren::FFT> m_fft_core;
        Eigen::SparseMatrix<float, Eigen::RowMajor> m_spectrogram;
        unsigned int m_sampling_rate;
        size_t m_window_size;
        size_t m_window_counter;
        float m_time_resolution;
        float m_freq_resolution;
        float m_nyquist_component;
    };

    class PeakSpectrogram : public Spectrogram
    {
    public:
        PeakSpectrogram(std::unique_ptr<siren::audio::PCM> pcm, std::unique_ptr<siren::FFT> fft, float zscore = 2.5);
        [[nodiscard]] std::vector<std::pair<size_t, size_t>> get_occupied_indices();
        [[nodiscard]] const Eigen::SparseMatrix<float, Eigen::RowMajor>& get_peak_spec_view() const;

    private:
        void init_peak_spectrogram();
        void make_peak_spectrogram();
        std::vector<unsigned int> log_distribution(size_t end_index, int bands);

        template<typename T>
        double get_median(std::vector<T> dist)
        {
            if (dist.empty())
            {
                return 0;
            }
            std::sort(dist.begin(), dist.end());
            return dist[dist.size()/2];
        }

        template<typename T>
        double get_mad(std::vector<T> dist)
        {
            if (dist.empty())
            {
                return 0;
            }

            double median = get_median(dist);
            size_t size = dist.size();

            std::vector<double> deviations(size);
            std::transform(dist.begin(), dist.end(), deviations.begin(),
            [&](double x)
            {
                return std::abs(x - median);
            });

            std::sort(deviations.begin(), deviations.end());
            return deviations[size/2];
        }

        template<typename T>
        double get_zscore_of_peak(double median, double mad, T point)
        {
            double z_score = 0.6745 * ((point - median) / mad);
            if (isnan(z_score))
            {
                z_score = 0;
            }
            return z_score;
        }

    private:
        float m_zscore;
        Eigen::SparseMatrix<float, Eigen::RowMajor> m_peak_spectrogram;
    };

}// namespace siren