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
                str_array += std::to_string(m_values[i]);
            }
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

    using HashableAnchor = Hashable<Anchor, int64_t>;

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
        CoreStatus make_fingerprint(Spec&& spectrogram, size_t block_size, size_t min_peak_count, float stride_coeff=0.5)
        {
            Eigen::SparseMatrix<float, Eigen::RowMajor> space = spectrogram.get_peak_spec_view();
            if (space.nonZeros() < min_peak_count)
            {
                return CoreStatus::PeaksTooSparse;
            }

            if (block_size > space.rows() || block_size > space.cols())
            {
                return CoreStatus::CoreParamsFatalError;
            }

            if (block_size < spectrogram.get_time_resolution() || block_size < spectrogram.get_freq_resolution())
            {
                return CoreStatus::CoreParamsLogicError;
            }

            auto hash_block = [this](Eigen::SparseMatrix<float, Eigen::RowMajor>&& block, int ioffset, int joffset)
            {
                if (block.nonZeros() < 3)
                {
                    return;
                }
                std::vector<std::array<int64_t, 2>> points;
                for (size_t i = 0; i < block.outerSize(); i++)
                {
                    for (auto it = Eigen::SparseMatrix<float, Eigen::RowMajor>::InnerIterator(block, i); it; ++it)
                    {
                        points.push_back({it.row() + ioffset, it.col() + joffset});
                    }
                }

                KDTree<int64_t, 2> tree(points);
                for (size_t i = 0; i < points.size() - 3; i++)
                {
                    std::array<int64_t, 2> anchor_point = points[i];
                    auto cluster = tree.nearest_neighbors({anchor_point[0], anchor_point[1] + block.cols()/5});
                    cluster.erase(anchor_point);

                    if (cluster.size() < 3)
                    {
                        continue;
                    }

                    std::array<int64_t, 2> first = *cluster.begin();
                    std::array<int64_t, 2> second = *std::next(cluster.begin(), 1);
                    std::array<int64_t, 2> third = *std::next(cluster.begin(), 2);

                    // avoiding dt == 0 to minimize collisions
                    if (first[1] == anchor_point[1] && second[1] == anchor_point[1] && third[1] == anchor_point[1])
                    {
                        continue;
                    }

                    HashableAnchor anchor
                        {
                            first[0], second[0], third[0],
                            anchor_point[0],
                            abs(anchor_point[1] - first[1]),
                            abs(anchor_point[1] - second[1]),
                            abs(anchor_point[1] - third[1]),
                        };
                    m_fingerprint.emplace(anchor.hash(), anchor_point[1]);
                }
            };

            for (size_t i = 0; i < space.rows() - block_size; i += floor(block_size*stride_coeff))
            {
                for (size_t j = 0; j < space.cols() - block_size; j += floor(block_size*stride_coeff))
                {
                    Eigen::SparseMatrix<float, Eigen::RowMajor> block = space.block(i, j, block_size, block_size);
                    hash_block(std::move(block), i, j);
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