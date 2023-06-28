#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/entities/kdtree.h"

TEST(KDTree, Trivial)
{
    using ::testing::ElementsAre;

    KDTree<int, 2> tree{{{2, 3}, {5, 4}, {9, 6}, {4, 7}, {8, 1}, {7, 2}}};
    EXPECT_EQ(tree.get_size(), 6);

    auto neighbors = tree.nearest_neighbors({6, 6});
    EXPECT_EQ(neighbors.size(), 3);
    ASSERT_THAT(neighbors, ElementsAre(std::array{4, 7}, std::array{5, 4}, std::array{7, 2}));
}