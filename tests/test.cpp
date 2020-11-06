// Copyright 2020 Your Name <your_email>

#include <gtest/gtest.h>
#include <string>
#include <hash_checker.hpp>

TEST(Hash_checker, Hash) {
    std::string hash1 = "hjhjo5235jfhasf00";
    EXPECT_FALSE(check_hash(3, hash1));
    std::string hash2 = "fhajkh934758900000";
    EXPECT_TRUE(check_hash(4, hash2));
    std::string hash3 = "fahoi413452afjlkjf000000";
    EXPECT_TRUE(check_hash(6, hash3));
}
