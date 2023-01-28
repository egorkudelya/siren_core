#pragma once

#include <iostream>
#include <vector>

namespace siren::audio
{

    class PCM
    {

    public:
        PCM(std::string path, unsigned int channels, unsigned int sampling_rate);
        float operator[](size_t idx);

        bool config_decoder();

        [[nodiscard]] float get_length_in_ms() const;
        [[nodiscard]] size_t get_frame_count() const;
        [[nodiscard]] unsigned int get_sampling_rate() const;

    private:
        std::vector<float> m_pcm;
        std::string m_track_path;
        unsigned int m_sampling_rate;
        unsigned int m_channels;
        float m_length_ms;
    };

}// namespace siren::audio