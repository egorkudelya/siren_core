#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "spectrogram.h"
#include "../common/common.h"
#include "../common/hash/xxh64.h"
#include "../serializer/serializer.h"

namespace siren
{

    template<typename T>
    class Anchor
    {
    public:
        Anchor(std::initializer_list<T> initializer_list)
            : m_values(std::move(initializer_list))
        {
        }

        [[nodiscard]] inline std::string to_str() const
        {
            std::string str_array{"["};
            str_array.reserve(m_expected_len);
            for (int i = 0; i < m_values.size(); ++i)
            {
                str_array += std::to_string(m_values[i]) + ',';
            }
            str_array.pop_back();
            str_array += "]";
            return str_array;
        }

    private:
        const int m_expected_len{35};
        std::vector<T> m_values;
    };

    template<template<typename> class BASE, typename T>
    class Hashable : public BASE<T>
    {
        static_assert(std::is_member_function_pointer_v<decltype(&BASE<T>::to_str)>);
    public:
        Hashable(std::initializer_list<T> initializer_list)
            : BASE<T>(initializer_list)
        {
        }

        [[nodiscard]] uint64_t hash() const noexcept
        {
            std::string str = BASE<T>::to_str();
            uint64_t seed = 0;
            return xxh64::hash(str.c_str(), str.size(), seed);
        }
    };

    using HashableAnchor = Hashable<Anchor, size_t>;

    template<typename Spec = siren::PeakSpectrogram, typename KeyType = uint64_t, typename Timestamp = size_t>
    class Fingerprint
    {
    public:
        Fingerprint() = default;
        using Iterator = typename std::unordered_map<KeyType, Timestamp>::iterator;
        using ConstIterator = typename std::unordered_map<KeyType, Timestamp>::const_iterator;

        Iterator begin()
        {
            return m_fingerprint.begin();
        }

        Iterator end()
        {
            return m_fingerprint.end();
        }

        ConstIterator cbegin()
        {
            return m_fingerprint.cbegin();
        }

        ConstIterator cend()
        {
            return m_fingerprint.cend();
        }

        template<typename InputIterator>
        Fingerprint(InputIterator begin, InputIterator end)
        {
            static_assert(std::is_same_v<Iterator, InputIterator> || std::is_same_v<ConstIterator, InputIterator>);
            std::unordered_map<KeyType, Timestamp> incoming(begin, end);
            m_fingerprint = std::move(incoming);
        }

        friend bool operator==(const Fingerprint& lhs, const Fingerprint& rhs)
        {
            return lhs.m_fingerprint == rhs.m_fingerprint;
        }

        void print()
        {
            for (const auto& bucket : m_fingerprint)
            {
                std::cout << "fingerprint: " << bucket.first << ", ts: " << bucket.second << std::endl;
            }
            std::cout << "fingerprint size: " << m_fingerprint.size() << std::endl;
        }

        std::vector<KeyType> get_hashes() const
        {
            std::vector<KeyType> hashes;
            hashes.reserve(m_fingerprint.size());
            for (const auto& bucket : m_fingerprint)
            {
                hashes.push_back(bucket.first);
            }
            return hashes;
        }

        CoreStatus make_fingerprint(Spec&& spectrogram, size_t net_size, size_t min_peak_count)
        {
            std::vector<std::pair<size_t, size_t>> ind = spectrogram.get_occupied_indices();

            if (ind.size() < min_peak_count)
            {
                return CoreStatus::PeaksTooSparse;
            }

            auto predicate = [&](const auto& first, const auto& second) {
                /**
			    * .first - frequency, .second - timestamp
			    */
                if (first.second == second.second)
                {
                    return first.first < second.first;
                }
                return first.second < second.second;
            };

            std::sort(ind.begin(), ind.end(), predicate);
            for (size_t i = 6; i < ind.size() - net_size; i++)
            {
                for (size_t j = i + 7; j < i + net_size; j++)
                {
                    if  (  ind[j].second - ind[i-4].second == 0
                        || ind[j].second - ind[i-2].second == 0
                        || ind[j].second - ind[i].second == 0)
                    {
                        continue;
                    }

                    Timestamp ts = ind[j].second;
                    HashableAnchor anchor
                        {
                        ind[i-4].first, ind[i-2].first,
                        ind[i].first,   ind[j].first,
                        ind[j].second - ind[i-4].second,
                        ind[j].second - ind[i-2].second,
                        ind[j].second - ind[i].second
                        };

                    m_fingerprint.emplace(anchor.hash(), ts);
                }
            }
            return CoreStatus::OK;
        }

        constexpr static auto properties()
        {
            return std::make_tuple(
                json::property(&Fingerprint::m_fingerprint, "fingerprint"));
        }

    private:
        std::unordered_map<KeyType, Timestamp> m_fingerprint;
    };

}// namespace siren