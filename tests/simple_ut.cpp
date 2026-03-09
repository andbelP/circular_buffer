#include <circular_buffer.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

template <typename T>
class circular_buffer_int_test : public testing::Test {};

using CircularBufferIntTypes = testing::Types<circular_buffer<int, false>>;
TYPED_TEST_SUITE(circular_buffer_int_test, CircularBufferIntTypes);

template <typename T>
class circular_buffer_string_test : public testing::Test {};

using CircularBufferStringTypes =
    testing::Types<circular_buffer<std::string, false>>;
TYPED_TEST_SUITE(circular_buffer_string_test, CircularBufferStringTypes);

TYPED_TEST(circular_buffer_int_test, alternatingPush) {
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

TYPED_TEST(circular_buffer_string_test, pushingComplicatedObjects) {
    TypeParam cb(3);
    cb.push_back("aaa");
    cb.push_back("bbb");
    cb.push_back("ccc");
    ASSERT_THAT(cb, testing::ElementsAre("aaa", "bbb", "ccc"));
}

TYPED_TEST(circular_buffer_int_test, simplePopTest) {
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

TYPED_TEST(circular_buffer_int_test, popFromEmpty) {
    TypeParam cb(1);
    cb.push_back(1);
    cb.pop_back();

    try {
        cb.pop_back();
        FAIL();
    } catch (...) {
    }

    try {
        cb.pop_front();
        FAIL();
    } catch (...) {
    }

    SUCCEED();
}

TYPED_TEST(circular_buffer_int_test, eraseOneElement) {
    TypeParam cb = {1, 2, 3, 4, 5};

    cb.erase(cb.cbegin() + 2);

    ASSERT_THAT(cb, testing::ElementsAre(1, 2, 4, 5));
}

TYPED_TEST(circular_buffer_int_test, eraseSequence) {
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

TYPED_TEST(circular_buffer_int_test, nValues) {
    TypeParam cb(5);

    cb.assign(static_cast<size_t>(5), 10);

    ASSERT_THAT(cb, testing::ElementsAre(10, 10, 10, 10, 10));
}

TYPED_TEST(circular_buffer_int_test, assignIterator) {
    TypeParam cb(5);
    std::vector<int> v = {1, 2, 3, 4, 5};

    cb.assign(v.begin() + 1, v.end() - 1);

    ASSERT_THAT(cb, testing::ElementsAre(2, 3, 4));
}

TYPED_TEST(circular_buffer_int_test, initializerList) {
    TypeParam cb = {5, 4, 3, 2, 1};

    cb.assign({1, 2, 3, 4, 5});

    ASSERT_THAT(cb, testing::ElementsAre(1, 2, 3, 4, 5));
}

TYPED_TEST(circular_buffer_int_test, simpleTest) {
    TypeParam cb(5);  // {3, 1, 0, 2, 4}
    for (int i = 0; i < 5; ++i) {
        if (i % 2 == 0) {
            cb.push_back(i);
        } else {
            cb.push_front(i);
        }
    }

    ASSERT_EQ(cb.front(), 3);
}

TYPED_TEST(circular_buffer_int_test, ClearBuffer) {
    TypeParam cb = {1, 2, 3};
    cb.clear();
    ASSERT_EQ(cb.size(), 0);
    ASSERT_TRUE(cb.empty());
}

TYPED_TEST(circular_buffer_int_test, ResizeLessThanSize) {
    TypeParam cb = {1, 2, 3, 4};
    cb.resize(2);
    ASSERT_EQ(cb.size(), 2);
    ASSERT_THAT(cb, testing::ElementsAre(1, 2));
}

TYPED_TEST(circular_buffer_int_test, AssignIterator) {
    std::vector<typename TypeParam::value_type> v = {52, 69, 67, 52};
    TypeParam cb(2);
    cb.assign(v.begin() + 1, v.end() - 1);
    ASSERT_THAT(cb, testing::ElementsAre(69, 67));
}

TYPED_TEST(circular_buffer_int_test, EraseAllElements) {
    TypeParam cb = {1, 2, 3};
    cb.erase(cb.begin(), cb.end());
    ASSERT_TRUE(cb.empty());
}

TYPED_TEST(circular_buffer_int_test, PushFront) {
    TypeParam cb(3);
    cb.push_front(42);
    ASSERT_EQ(cb.size(), 1);
    ASSERT_EQ(cb.front(), 42);
    ASSERT_EQ(cb.back(), 42);
}

TYPED_TEST(circular_buffer_int_test, PushFrontOverwrite) {
    TypeParam cb{1, 2, 3};
    cb.push_front(0);
    cb.push_front(-1);
    cb.push_front(-2);
    ASSERT_THAT(cb, testing::ElementsAre(-2, -1, 0));
}

TYPED_TEST(circular_buffer_int_test, OverwriteOldElementsPushBack) {
    TypeParam cb(3);

    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);

    cb.push_back(4);
    cb.push_back(5);

    ASSERT_THAT(cb, testing::ElementsAre(3, 4, 5));
}

TYPED_TEST(circular_buffer_int_test, OverwriteOldElementsPushFront) {
    TypeParam cb{1, 2, 3};

    cb.push_front(4);
    cb.push_front(5);

    ASSERT_THAT(cb, testing::ElementsAre(5, 4, 1));
}

TYPED_TEST(circular_buffer_int_test, FrontBackAfterOverwrite) {
    TypeParam cb(3);

    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);

    ASSERT_EQ(cb.front(), 1);
    ASSERT_EQ(cb.back(), 3);

    cb.push_back(4);  // {2,3,4}
    ASSERT_EQ(cb.front(), 2);
    ASSERT_EQ(cb.back(), 4);

    //{1,2,3}
    cb.push_front(1);
    ASSERT_EQ(cb.front(), 1);
    ASSERT_EQ(cb.back(), 3);
}

TEST(ReverseIteratorTest, SimpleIteration) {
    circular_buffer<int, true> cb = {1, 2, 3, 4, 5};

    std::vector<int> result;
    for (auto it = cb.rbegin(); it != cb.rend(); ++it) {
        result.push_back(*it);
    }

    ASSERT_THAT(result, testing::ElementsAre(5, 4, 3, 2, 1));
}

TEST(ReverseIteratorTest, PrefixAndPostfixIncrement) {
    circular_buffer<int, true> cb = {52, 67, 69};

    auto it = cb.rbegin();

    ASSERT_EQ(*it, 69);

    ++it;
    ASSERT_EQ(*it, 67);

    it++;
    ASSERT_EQ(*it, 52);
}

TEST(ReverseIteratorTest, DecrementOperators) {
    circular_buffer<int, true> cb = {1, 2, 3};

    auto it = cb.rend();
    --it;

    ASSERT_EQ(*it, 1);

    it--;
    ASSERT_EQ(*it, 2);
}

TEST(ReverseIteratorTest, ArithmeticOperators) {
    circular_buffer<int, true> cb = {1, 2, 3, 4, 5};

    auto it = cb.rbegin();

    ASSERT_EQ(*(it + 2), 3);
    ASSERT_EQ(*(it + 4), 1);

    auto it2 = it + 3;
    ASSERT_EQ(*it2, 2);

    auto it3 = it2 - 2;
    ASSERT_EQ(*it3, 4);
}

TEST(ReverseIteratorTest, IndexOperator) {
    circular_buffer<int, true> cb = {1, 2, 3, 4, 5};

    auto it = cb.rbegin();

    ASSERT_EQ(it[0], 5);
    ASSERT_EQ(it[1], 4);
    ASSERT_EQ(it[4], 1);
}

TEST(ReverseIteratorTest, ConstReverseIterator) {
    const circular_buffer<int, true> cb = {1, 2, 3, 4};

    std::vector<int> result;
    for (auto it = cb.crbegin(); it != cb.crend(); ++it) {
        result.push_back(*it);
    }

    ASSERT_THAT(result, testing::ElementsAre(4, 3, 2, 1));
}

TEST(ReverseIteratorTest, CompareWithStdReverse) {
    circular_buffer<int, true> cb = {1, 2, 3, 4, 5};

    std::vector<int> v(cb.begin(), cb.end());
    std::reverse(v.begin(), v.end());

    std::vector<int> result(cb.rbegin(), cb.rend());

    ASSERT_EQ(result, v);
}

TEST(CopyContainerTest, CopyConstructor) {
    circular_buffer<int, true> cb = {1, 2, 3, 4, 5};

    auto other = cb;
    ASSERT_THAT(other, testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(CopyContainerTest, MoveConstructor) {
    circular_buffer<int, true> cb = {1, 2, 3, 4, 5};

    auto other = std::move(cb);
    ASSERT_THAT(other, testing::ElementsAre(1, 2, 3, 4, 5));
    ASSERT_EQ(cb.size(), 0);
}

TEST(CircularBufferTest, InsertionToUnextendableManyCases) {
    std::vector<int> v = {30, 31, 32, 33, 34};
    std::vector<std::vector<int>> ans = {
        {1, 2, 3, 4}, {32, 2, 3, 4}, {31, 32, 3, 4}};

    for (int i = 0; i < 3; i++) {
        circular_buffer<int> buff = {1, 2, 3, 4};
        buff.insert(buff.begin() + i, v.begin(), v.begin() + 3);
        ASSERT_THAT(buff, testing::ElementsAreArray(ans[i]));
    }
}

TYPED_TEST(circular_buffer_int_test, InsertFitsWithoutOverwrite) {
    TypeParam cb(5);
    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);

    cb.insert(cb.begin() + 1, {4, 5});

    ASSERT_THAT(cb, testing::ElementsAre(1, 4, 5, 2, 3));
}

TYPED_TEST(circular_buffer_int_test, InsertOverwrite) {
    TypeParam cb(5);
    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);
    cb.push_back(4);
    cb.push_back(5);

    cb.insert(cb.begin() + 1, {6, 7, 8});

    ASSERT_EQ(cb.size(), 5);
    ASSERT_THAT(cb, testing::ElementsAre(8, 2, 3, 4, 5));
}

TYPED_TEST(circular_buffer_int_test, InsertAtBeginningOverwrite) {
    TypeParam cb = {1, 2, 3, 4, 5};
    cb.insert(cb.begin(), {6, 7, 8});
    ASSERT_THAT(cb, testing::ElementsAre(1, 2, 3, 4, 5));
}

TYPED_TEST(circular_buffer_int_test, InsertAtEndOverwrite) {
    TypeParam cb = {1, 2, 3, 4, 5};
    cb.insert(cb.end(), {6, 7});

    ASSERT_EQ(cb.size(), 5);
    ASSERT_THAT(cb, testing::ElementsAre(3, 4, 5, 6, 7));
}

TYPED_TEST(circular_buffer_int_test, InsertSingleElementOverwrite) {
    TypeParam cb = {1, 2, 3, 4, 5};
    cb.insert(cb.begin() + 2, 9);

    ASSERT_EQ(cb.size(), 5);
    ASSERT_THAT(cb, testing::ElementsAre(2, 9, 3, 4, 5));
}

TYPED_TEST(circular_buffer_int_test, InsertMultipleValsOverwrite) {
    TypeParam cb = {1, 2, 3};
    cb.insert(cb.begin(), 5, 7);

    ASSERT_EQ(cb.size(), 3);
    ASSERT_THAT(cb, testing::ElementsAre(1, 2, 3));
}

TYPED_TEST(circular_buffer_int_test, InsertIntoEmptyBuffer) {
    TypeParam cb(3);
    cb.insert(cb.begin(), {1, 2, 3, 4});

    ASSERT_EQ(cb.size(), 3);
    ASSERT_THAT(cb, testing::ElementsAre(2, 3, 4));
}