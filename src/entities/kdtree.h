#pragma once
#include <array>
#include <set>

template <typename CoordType, size_t dims>
class Point
{
public:
    Point(std::array<CoordType, dims>&& arr)
        : m_point{std::move(arr)}
    {
    }

    CoordType get(size_t index) const
    {
        return m_point[index % m_point.size()];
    }

    auto as_array() const
    {
        return m_point;
    }

    size_t get_dims() const
    {
        return dims;
    }

    double distance(const Point& other) const
    {
        double dist = 0;
        for (size_t i = 0; i < dims; ++i)
        {
            double d = get(i) - other.get(i);
            dist += d * d;
        }
        return dist;
    }

private:
    std::array<CoordType, dims> m_point;
};


template <typename CoordType, size_t dims>
class KDTree
{
    using PointType = Point<CoordType, dims>;
    using NeighborSet = std::set<std::array<CoordType, dims>>;

    struct Node
    {
        Node(const PointType& point)
            : m_point(point), m_left(nullptr), m_right(nullptr) {}

        CoordType get(size_t index) const
        {
            return m_point.get(index);
        }

        double distance(const PointType& other) const
        {
            return m_point.distance(other);
        }

        void add(Node* node)
        {
            add(node, 0);
        }

        bool add(Node* node, int k)
        {
            if (node->get(k) < m_point.get(k))
            {
                if (m_left == nullptr)
                {
                    m_left = node;
                }
                else
                {
                    m_left->add(node, k + 1);
                }
                return true;
            }
            if (m_right == nullptr)
            {
                m_right = node;
            }
            else
            {
                m_right->add(node, k + 1);
            }
            return true;
        }

        PointType m_point;
        Node* m_left{nullptr};
        Node* m_right{nullptr};
    };

public:
    explicit KDTree(const std::vector<std::array<CoordType, dims>>& points)
    {
        init_tree(points);
    }

    explicit KDTree(std::vector<std::array<CoordType, dims>>&& points)
    {
        init_tree(std::move(points));
    }

    ~KDTree()
    {
        clear(m_root);
    }

    NeighborSet nearest_neighbors(std::array<CoordType, dims>&& point_coords) const
    {
        NeighborSet neighbors;
        nearest_neighbors(m_root, PointType{std::move(point_coords)}, 0, neighbors);
        return neighbors;
    }

    size_t get_size() const
    {
        return m_size;
    }

private:
    void clear(Node* node)
    {
        if (node != nullptr)
        {
            clear(node->m_left);
            clear(node->m_right);
            delete node;
        }
    }

    template<typename Range>
    void init_tree(Range&& range)
    {
        if (range.empty())
        {
            return;
        }

        for (size_t i = 0; i < range.size(); i++)
        {
            auto point_coords = range[i];
            if (dims != point_coords.size())
            {
                throw std::logic_error("KDTree dim and point dim are not the same");
            }
            if (i == 0)
            {
                m_root = new Node(std::move(point_coords));
                m_size++;
                continue;
            }
            Node* node = new Node(std::move(point_coords));
            m_root->add(node);
            m_size++;
        }
    }

    Node* closest_node(Node* n0, Node* n1, const PointType& target) const
    {
        if (n0 == nullptr)
        {
            return n1;
        }
        if (n1 == nullptr)
        {
            return n0;
        }

        auto d0 = n0->m_point.distance(target);
        auto d1 = n1->m_point.distance(target);

        if (d0 < d1)
        {
            return n0;
        }
        return n1;
    }

    Node* nearest_neighbors(Node* root, const PointType& target, int depth, NeighborSet& neighbors) const
    {
        if (root == nullptr)
        {
            return nullptr;
        }

        Node* next_branch = nullptr;
        Node* other_branch = nullptr;

        if (target.get(depth) < root->get(depth))
        {
            next_branch = root->m_left;
            other_branch = root->m_right;
        }
        else
        {
            next_branch = root->m_right;
            other_branch = root->m_left;
        }

        Node* temp = nearest_neighbors(next_branch, target, depth + 1, neighbors);
        Node* closest = closest_node(temp, root, target);

        auto radius = target.distance(closest->m_point);

        auto dist = target.get(depth) - root->get(depth);
        if (radius >= dist * dist)
        {
            temp = nearest_neighbors(other_branch, target, depth + 1, neighbors);
            closest = closest_node(temp, closest, target);
        }

        neighbors.insert(closest->m_point.as_array());
        return closest;
    }

private:
    size_t m_size{0};
    Node* m_root{nullptr};
};