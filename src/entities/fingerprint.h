#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "spectrogram.h"
#include "kdtree.h"
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
            std::string str_array;
            str_array.reserve(m_expected_len);
            for (int i = 0; i < m_values.size(); ++i)
            {
                str_array += std::to_string(m_values[i]) + ',';
            }
            str_array.pop_back();
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

    template<typename KeyType = uint64_t, typename Timestamp = size_t>
    class Fingerprint
    {
    public:
        using MapType = std::unordered_map<KeyType, Timestamp>;

        using iterator = typename MapType::iterator;
        using const_iterator = typename MapType::const_iterator;

        Fingerprint() = default;

        iterator begin()
        {
            return m_fingerprint.begin();
        }

        iterator end()
        {
            return m_fingerprint.end();
        }

        const_iterator cbegin() const
        {
            return m_fingerprint.cbegin();
        }

        const_iterator cend() const
        {
            return m_fingerprint.cend();
        }

        size_t get_size() const
        {
            return m_fingerprint.size();
        }

        template<typename InputIterator>
        Fingerprint(InputIterator begin, InputIterator end)
        {
            static_assert(std::is_constructible_v<MapType, InputIterator, InputIterator>);
            MapType incoming(begin, end);
            m_fingerprint = std::move(incoming);
        }

        friend bool operator==(const Fingerprint& lhs, const Fingerprint& rhs)
        {
            return lhs.m_fingerprint == rhs.m_fingerprint;
        }

        void print() const
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

        template<typename Spec = siren::PeakSpectrogram>
        CoreStatus make_fingerprint(Spec&& spectrogram, size_t tile_size, size_t min_peak_count)
        {
            Eigen::SparseMatrix<float, Eigen::RowMajor> space = spectrogram.get_peak_spec_view();
            if (space.nonZeros() < min_peak_count)
            {
                return CoreStatus::PeaksTooSparse;
            }

            if (tile_size > space.rows() || tile_size > space.cols())
            {
                return CoreStatus::CoreParamsFatalError;
            }

            if (tile_size < spectrogram.get_time_resolution() || tile_size < spectrogram.get_freq_resolution())
            {
                return CoreStatus::CoreParamsLogicError;
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


            auto hash_tile = [predicate, this](Eigen::SparseMatrix<float, Eigen::RowMajor>&& block, int ioffset, int joffset)
            {
                if (block.nonZeros() < 4)
                {
                    return;
                }

                std::vector<std::array<Eigen::Index, 2>> points;
                for (int i = 0; i < block.outerSize(); i++)
                {
                    for (auto it = Eigen::SparseMatrix<float, Eigen::RowMajor>::InnerIterator(block, i); it; ++it)
                    {
                        points.push_back({it.row() + ioffset, it.col() + joffset});
                    }
                }

                KDTree<Eigen::Index, 2> tree(points);

                for (auto[freq, ts]: points)
                {
                    // {freq, ts} -> self
                    auto neighbors = tree.nearest_neighbors({freq, ts});
                    neighbors.erase({freq, ts});

                    if (neighbors.size() < 3)
                    {
                        continue;
                    }

                    // avoiding dt == 0 to minimize collisions
                    std::vector<std::pair<uint64_t, uint64_t>> valid;
                    for (auto&& neighbor : neighbors)
                    {
                        if (neighbor.at(1) != ts)
                        {
                            valid.emplace_back(std::make_pair(neighbor[0], neighbor[1]));
                        }
                    }

                    if (valid.size() < 3)
                    {
                        continue;
                    }

                    std::sort(valid.begin(), valid.end(), predicate);
                    HashableAnchor anchor
                        {
                            valid[0].first, valid[1].first,
                            valid[2].first, (unsigned)freq,
                            ts - valid[0].second,
                            ts - valid[1].second,
                            ts - valid[2].second
                        };
                    m_fingerprint.emplace(anchor.hash(), ts);
                }

            };

            // 50% overlapping tiles
            for (size_t i = 0; i < space.rows() - tile_size; i += tile_size/2)
            {
                for (size_t j = 0; j < space.cols() - tile_size; j += tile_size/2)
                {
                    Eigen::SparseMatrix<float, Eigen::RowMajor> block = space.block(i, j, tile_size, tile_size);
                    if (block.nonZeros() >= 4)
                    {
                        hash_tile(std::move(block), i, j);
                    }
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
        MapType m_fingerprint;
    };

}// namespace siren