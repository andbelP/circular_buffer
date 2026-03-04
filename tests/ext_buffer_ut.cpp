#include <circular_buffer.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(ExtendableBufferTest, simpleTest) {
    circular_buffer<int, true> cb(5);
    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);
    cb.push_back(4);
    cb.push_back(5);
    cb.push_back(6);
    ASSERT_THAT(cb, testing::ElementsAre(1, 2, 3, 4, 5, 6));
}

TEST(ExtendableBufferTest, insertInTheMiddle) {
    circular_buffer<int, true> cb(5);
    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);
    cb.push_back(4);
    cb.push_back(5);

    cb.insert(cb.begin() + 2, {6, 7, 8});
    ASSERT_THAT(cb, testing::ElementsAre(1, 2, 6, 7, 8, 3, 4, 5));
}

TEST(CircularBufferIntTest, expand) {
    circular_buffer<int, true> cb = {1, 2, 3, 4, 5};
    cb.resize(7, 6);

    ASSERT_THAT(cb, testing::ElementsAre(1, 2, 3, 4, 5, 6, 6));
}

TEST(CircularBufferIntTest, shrink) {
    circular_buffer<int, true> cb = {1, 2, 3, 4, 5};
    cb.resize(3);

    ASSERT_THAT(cb, testing::ElementsAre(1, 2, 3));
}

TEST(CircularBufferIntTest, insertValue) {
    circular_buffer<int, true> cb(5); // {3, 1, 0, 2, 4}
    for (int i = 0; i < 5; ++i) {
        if (i % 2 == 0) {
            cb.push_back(i);
        } else {
            cb.push_front(i);
        }
    }
    cb.insert(cb.begin() + 2, 10);

    ASSERT_THAT(cb, testing::ElementsAre(3, 1, 10, 0, 2, 4));
}

TEST(CircularBufferIntTest, insertNValues) {
    circular_buffer<int, true> cb(7); // {3, 1, 0, 2, 4}
    for (int i = 0; i < 5; ++i) {
        if (i % 2 == 0) {
            cb.push_back(i);
        } else {
            cb.push_front(i);
        }
    }
    cb.insert(cb.begin() + 2, 2, 8);

    ASSERT_THAT(cb, testing::ElementsAre(3, 1, 8, 8, 0, 2, 4));
}

TEST(CircularBufferIntTest, insertIterators) {
    circular_buffer<int, true> cb(8); // {3, 1, 0, 2, 4}
    std::vector<int> v = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; ++i) {
        if (i % 2 == 0) {
            cb.push_back(i);
        } else {
            cb.push_front(i);
        }
    }
    cb.insert(cb.begin() + 2, v.begin() + 1, v.end() - 1);

    ASSERT_THAT(cb, testing::ElementsAre(3, 1, 2, 3, 4, 0, 2, 4));
}

TEST(CircularBufferIntTest, insertInitializerList) {
    circular_buffer<int, true> cb(8); // {3, 1, 0, 2, 4}
    for (int i = 0; i < 5; ++i) {
        if (i % 2 == 0) {
            cb.push_back(i);
        } else {
            cb.push_front(i);
        }
    }
    cb.insert(cb.begin() + 2, {2, 3, 4});

    ASSERT_THAT(cb, testing::ElementsAre(3, 1, 2, 3, 4, 0, 2, 4));
}

TEST(ExtendableBufferTest, PopBackAndFront) {
    circular_buffer<int, true> cb = {1, 2, 3, 4};

    cb.pop_back();
    ASSERT_THAT(cb, testing::ElementsAre(1, 2, 3));

    cb.pop_front();
    ASSERT_THAT(cb, testing::ElementsAre(2, 3));

    cb.pop_back();
    cb.pop_front();
    ASSERT_TRUE(cb.empty());
}

TEST(ExtendableBufferTest, FrontBackAccess) {
    circular_buffer<int, true> cb;
    cb.push_back(1);
    cb.push_back(2);
    cb.push_front(0);

    ASSERT_EQ(cb.front(), 0);
    ASSERT_EQ(cb.back(), 2);

    cb.pop_front();
    // {1,2}
    ASSERT_EQ(cb.front(), 1);

    cb.pop_back(); 
    // {1}
    ASSERT_EQ(cb.back(), 1);
}


TEST(ExtendableBufferTest, EraseElements) {
    circular_buffer<int, true> cb = {1, 2, 3, 4, 5};

    cb.erase(cb.begin() + 2);
    ASSERT_THAT(cb, testing::ElementsAre(1,2,4,5));

    cb.erase(cb.begin(), cb.begin() + 2);
    ASSERT_THAT(cb, testing::ElementsAre(4,5));
}

TEST(ExtendableBufferTest, AssignValues) {
    circular_buffer<int, true> cb(3);

    cb.assign(4, 7);
    ASSERT_THAT(cb, testing::ElementsAre(7,7,7,7));

    std::vector<int> v = {1,2,3,4};
    cb.assign(v.begin() + 1, v.end() - 1);
    ASSERT_THAT(cb, testing::ElementsAre(2,3));

    cb.assign({5,6,7});
    ASSERT_THAT(cb, testing::ElementsAre(5,6,7));
}

TEST(ExtendableBufferTest, ClearBufferThenPushBack) {
    circular_buffer<int, true> cb = {1,2,3};
    cb.clear();
    ASSERT_TRUE(cb.empty());
    ASSERT_EQ(cb.size(), 0);

    cb.push_back(10);
    ASSERT_THAT(cb, testing::ElementsAre(10));
}

TEST(ExtendableBufferTest, WrapAround) {
    circular_buffer<int, true> cb(3);

    cb.push_back(1);
    cb.push_back(2);
    cb.push_back(3);

    cb.push_back(4);
    cb.push_front(0);

    ASSERT_THAT(cb, testing::ElementsAre(0,1,2,3,4));
}