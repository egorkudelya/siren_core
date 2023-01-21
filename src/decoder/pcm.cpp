#include <utility>
#include "pcm.h"

namespace siren::audio
{

    PCM::PCM(std::string path, unsigned int channels, unsigned int sampling_rate)
        : m_track_path(std::move(path)), m_channels(channels), m_sampling_rate(sampling_rate)
    {
    }

    float PCM::operator[](size_t idx)
    {
        return m_pcm[idx];
    }

    bool PCM::config_decoder()
    {
        ma_result result;
        ma_decoder decoder;
        ma_decoder_config config = ma_decoder_config_init(ma_format_f32, m_channels, m_sampling_rate);

        result = ma_decoder_init_file(m_track_path.c_str(), &config, &decoder);
        if (result != MA_SUCCESS)
        {
            return false;
        }

        ma_uint64 available_frames = 0;
        result = ma_decoder_get_available_frames(&decoder, &available_frames);
        if (result != MA_SUCCESS || available_frames == 0)
        {
            return false;
        }

        float* temp = new float[available_frames];
        ma_uint64 frames_read = 0;

        result = ma_data_source_read_pcm_frames(&decoder, temp, available_frames, &frames_read);
        if (result != MA_SUCCESS || frames_read <= 0)
        {
            return false;
        }

        m_pcm = std::vector(temp, temp + frames_read);

        float length;
        ma_data_source_get_length_in_seconds(&decoder, &length);

        m_length_ms = length * 1000;
        m_sampling_rate = decoder.outputSampleRate;
        m_channels = decoder.outputChannels;

        ma_decoder_uninit(&decoder);
        delete[] temp;

        return true;
    }

    size_t PCM::get_frame_count() const
    {
        return m_pcm.size();
    }

    float PCM::get_length_in_ms() const
    {
        return m_length_ms;
    }

    unsigned int PCM::get_sampling_rate() const
    {
        return m_sampling_rate;
    }

}// namespace siren::audio