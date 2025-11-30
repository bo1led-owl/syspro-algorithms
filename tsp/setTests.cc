#include <gtest/gtest.h>
#include <unordered_set>

#include "set.hh"

TEST(Set, Iterate) {
    Set s{0, 1, 3, 37, 42, 15};
    auto it = s.begin();

    EXPECT_NE(it, s.end());
    EXPECT_EQ(*it, 0);
    ++it;

    EXPECT_NE(it, s.end());
    EXPECT_EQ(*it, 1);
    ++it;

    EXPECT_NE(it, s.end());
    EXPECT_EQ(*it, 3);
    ++it;

    EXPECT_NE(it, s.end());
    EXPECT_EQ(*it, 15);
    ++it;

    EXPECT_NE(it, s.end());
    EXPECT_EQ(*it, 37);
    ++it;

    EXPECT_NE(it, s.end());
    EXPECT_EQ(*it, 42);
    ++it;

    EXPECT_EQ(it, s.end());
}

TEST(Set, Hash) {
    Set s1{0, 2, 3};

    Set s2{0, 2, 3, 128};

    std::unordered_set<Set> sets;
    sets.insert(s1);

    EXPECT_TRUE(sets.contains(s1));
    EXPECT_FALSE(sets.contains(s2));

    s2.erase(128);
    EXPECT_TRUE(sets.contains(s2));
}
