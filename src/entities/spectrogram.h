#pragma once

#include <vector>
#include <type_traits>

#include <Eigen/Core>
#include <Eigen/Sparse>

#include <opencv2/core/eigen.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../decoder/pcm.h"
#include "../fft/fft.h"
#include "freq_bin.h"

namespace siren
{

    class Spectrogram
    {

    public:
        Spectrogram(std::unique_ptr<siren::audio::PCM> pcm, std::unique_ptr<siren::FFT> fft);

        [[nodiscard]] size_t get_window_size() const;

        [[nodiscard]] unsigned int get_sampling_rate() const;

        [[nodiscard]] size_t rows() const;

        [[nodiscard]] size_t cols() const;

    protected:
        using Triplet = Eigen::Triplet<float>;

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
        PeakSpectrogram(std::unique_ptr<siren::audio::PCM> pcm, std::unique_ptr<siren::FFT> fft, float peak_threshold = 2);
        [[nodiscard]] std::vector<std::pair<size_t, size_t>> get_occupied_indices();

    protected:
        [[nodiscard]] const Eigen::SparseMatrix<float, Eigen::RowMajor>& get_peak_spec_view() const;

    private:
        void init_peak_spectrogram();
        void make_peak_spectrogram(float peak_threshold);
        std::vector<unsigned int> log_distribution(size_t end_index, int bands);

    private:
        Eigen::SparseMatrix<float, Eigen::RowMajor> m_peak_spectrogram;
    };

    class SaveablePeakSpectrogram : public PeakSpectrogram
    {
    public:
        SaveablePeakSpectrogram(std::unique_ptr<siren::audio::PCM> pcm, std::unique_ptr<siren::FFT> fft, float peak_threshold = 1.65);

        void as_png(const std::string& path);
    };

}// namespace siren