#include <circular_buffer.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <vector>

template <typename T>
class CircularBufferIntTest : public testing::Test {};

using CircularBufferIntTypes = testing::Types<
    circular_buffer<int, false>
>;
TYPED_TEST_SUITE(CircularBufferIntTest, CircularBufferIntTypes);

template <typename T>
class CircularBufferStringTest : public testing::Test {};

using CircularBufferStringTypes = testing::Types<
    circular_buffer<std::string, false>
>;
TYPED_TEST_SUITE(CircularBufferStringTest, CircularBufferStringTypes);

TYPED_TEST(CircularBufferIntTest, alternatingPush) {
    TypeParam cb(6);
    for (int i = 0; i < 6; ++i) {
        if (i % 2 == 0) {
            cb.push_back(i);
        } else {
            cb.push_front(i);
        }
    }

    ASSERT_THAT(cb, testing::ElementsAre(5, 3, 1, 0, 2, 4));
}

TYPED_TEST(CircularBufferStringTest, pushingComplicatedObjects) {
    TypeParam cb(3);
    cb.push_back("aaa");
    cb.push_back("bbb");
    cb.push_back("ccc");
    ASSERT_THAT(cb, testing::ElementsAre("aaa", "bbb", "ccc"));
}

TYPED_TEST(CircularBufferIntTest, simplePopTest) {
    TypeParam cb(5);

    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);

    cb.push_front(4);
    cb.push_front(5);

    cb.pop_back();
    cb.pop_front();

    ASSERT_THAT(cb, testing::ElementsAre(4, 1, 2));
}

TYPED_TEST(CircularBufferIntTest, popFromEmpty) {
    TypeParam cb(1);
    cb.push_back(1);
    cb.pop_back();

    try {
        cb.pop_back();
        FAIL();
    } catch (...) {}

    try {
        cb.pop_front();
        FAIL();
    } catch (...) {}

    SUCCEED();
}

TYPED_TEST(CircularBufferIntTest, eraseOneElement) {
    TypeParam cb = {1, 2, 3, 4, 5};

    cb.erase(cb.cbegin() + 2);

    ASSERT_THAT(cb, testing::ElementsAre(1, 2, 4, 5));
}

TYPED_TEST(CircularBufferIntTest, eraseSequence) {
    TypeParam cb(5);
    for (int i = 0; i < 5; ++i) {
        if (i % 2 == 0) {
            cb.push_back(i);
        } else {
            cb.push_front(i);
        }
    }
    cb.erase(cb.cbegin() + 1, cb.cend() - 1);

    ASSERT_THAT(cb, testing::ElementsAre(3, 4));
}

TYPED_TEST(CircularBufferIntTest, nValues) {
    TypeParam cb(5);

    cb.assign(static_cast<size_t>(5), 10);

    ASSERT_THAT(cb, testing::ElementsAre(10, 10, 10, 10, 10));
}

TYPED_TEST(CircularBufferIntTest, assignIterator) {
    TypeParam cb(5);
    std::vector<int> v = {1, 2, 3, 4, 5};

    cb.assign(v.begin() + 1, v.end() - 1);

    ASSERT_THAT(cb, testing::ElementsAre(2, 3, 4));
}

TYPED_TEST(CircularBufferIntTest, initializerList) {
    TypeParam cb = {5, 4, 3, 2, 1};

    cb.assign({1, 2, 3, 4, 5});

    ASSERT_THAT(cb, testing::ElementsAre(1, 2, 3, 4, 5));
}

TYPED_TEST(CircularBufferIntTest, simpleTest) {
    TypeParam cb(5); // {3, 1, 0, 2, 4}
    for (int i = 0; i < 5; ++i) {
        if (i % 2 == 0) {
            cb.push_back(i);
        } else {
            cb.push_front(i);
        }
    }

    ASSERT_EQ(cb.front(), 3);
}

TYPED_TEST(CircularBufferIntTest, ClearBuffer) {
    TypeParam cb = {1, 2, 3};
    cb.clear();
    ASSERT_EQ(cb.size(), 0);
    ASSERT_TRUE(cb.empty());
}

TYPED_TEST(CircularBufferIntTest, ResizeLessThanSize) {
    TypeParam cb = {1, 2, 3, 4};
    cb.resize(2);
    ASSERT_EQ(cb.size(), 2);
    ASSERT_THAT(cb, testing::ElementsAre(1, 2));
}

TYPED_TEST(CircularBufferIntTest, AssignIterator) {
    std::vector<typename TypeParam::value_type> v = {52,69,67,52};
    TypeParam cb(2);
    cb.assign(v.begin() + 1, v.end() - 1);
    ASSERT_THAT(cb, testing::ElementsAre(69,67));
}

TYPED_TEST(CircularBufferIntTest, EraseAllElements) {
    TypeParam cb = {1, 2, 3};
    cb.erase(cb.begin(), cb.end());
    ASSERT_TRUE(cb.empty());
}

TYPED_TEST(CircularBufferIntTest, PushFront) {
    TypeParam cb(3);
    cb.push_front(42);
    ASSERT_EQ(cb.size(), 1);
    ASSERT_EQ(cb.front(), 42);
    ASSERT_EQ(cb.back(), 42);
}

TYPED_TEST(CircularBufferIntTest, OverwriteOldElementsPushBack) {
    TypeParam cb(3);

    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);

    cb.push_back(4);
    cb.push_back(5);

    ASSERT_THAT(cb, testing::ElementsAre(3,4,5));
}

TYPED_TEST(CircularBufferIntTest, OverwriteOldElementsPushFront) {
    TypeParam cb {1,2,3};

    cb.push_front(4);
    cb.push_front(5);

    ASSERT_THAT(cb, testing::ElementsAre(5,4,1));
}

TYPED_TEST(CircularBufferIntTest, FrontBackAfterOverwrite) {
    TypeParam cb(3);

    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);

    ASSERT_EQ(cb.front(), 1);
    ASSERT_EQ(cb.back(), 3);

    cb.push_back(4); // {2,3,4}
    ASSERT_EQ(cb.front(), 2);
    ASSERT_EQ(cb.back(), 4);

    //{1,2,3}
    cb.push_front(1);
    ASSERT_EQ(cb.front(), 1);
    ASSERT_EQ(cb.back(), 3);
}

TEST(ReverseIteratorTest, SimpleIteration) {
    circular_buffer<int, true> cb = {1,2,3,4,5};

    std::vector<int> result;
    for (auto it = cb.rbegin(); it != cb.rend(); ++it) {
        result.push_back(*it);
    }

    ASSERT_THAT(result, testing::ElementsAre(5,4,3,2,1));
}

TEST(ReverseIteratorTest, PrefixAndPostfixIncrement) {
    circular_buffer<int, true> cb = {52,67,69};

    auto it = cb.rbegin();

    ASSERT_EQ(*it, 69);

    ++it;
    ASSERT_EQ(*it, 67);

    it++;
    ASSERT_EQ(*it, 52);
}

TEST(ReverseIteratorTest, DecrementOperators) {
    circular_buffer<int, true> cb = {1,2,3};

    auto it = cb.rend();
    --it;

    ASSERT_EQ(*it, 1);

    it--;
    ASSERT_EQ(*it, 2);
}

TEST(ReverseIteratorTest, ArithmeticOperators) {
    circular_buffer<int, true> cb = {1,2,3,4,5};

    auto it = cb.rbegin();

    ASSERT_EQ(*(it + 2), 3);
    ASSERT_EQ(*(it + 4), 1);

    auto it2 = it + 3;
    ASSERT_EQ(*it2, 2);

    auto it3 = it2 - 2;
    ASSERT_EQ(*it3, 4);
}

TEST(ReverseIteratorTest, IndexOperator) {
    circular_buffer<int, true> cb = {1,2,3,4,5};

    auto it = cb.rbegin();

    ASSERT_EQ(it[0], 5);
    ASSERT_EQ(it[1], 4);
    ASSERT_EQ(it[4], 1);
}


TEST(ReverseIteratorTest, ConstReverseIterator) {
    const circular_buffer<int, true> cb = {1,2,3,4};

    std::vector<int> result;
    for (auto it = cb.crbegin(); it != cb.crend(); ++it) {
        result.push_back(*it);
    }

    ASSERT_THAT(result, testing::ElementsAre(4,3,2,1));
}

TEST(ReverseIteratorTest, CompareWithStdReverse) {
    circular_buffer<int, true> cb = {1,2,3,4,5};

    std::vector<int> v(cb.begin(), cb.end());
    std::reverse(v.begin(), v.end());

    std::vector<int> result(cb.rbegin(), cb.rend());

    ASSERT_EQ(result, v);
}


TEST(CopyContainerTest, CopyConstructor) {
    circular_buffer<int, true> cb = {1,2,3,4,5};

    auto other = cb;
    ASSERT_THAT(other, testing::ElementsAre(1,2,3,4,5));
    
}

TEST(CopyContainerTest, MoveConstructor) {
    circular_buffer<int, true> cb = {1,2,3,4,5};

    auto other = std::move(cb);
    ASSERT_THAT(other, testing::ElementsAre(1,2,3,4,5));
    ASSERT_EQ(cb.size(), 0);
    
}