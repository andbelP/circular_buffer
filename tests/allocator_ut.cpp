#include <circular_buffer.h>

#include <gtest/gtest.h>

#include <cstdlib>
#include <concepts>
#include <iostream>

class node_tag {};

class some_obj {
public:
    static inline int constructor_called = 0;
    static inline int destructor_called = 0;

    some_obj() {
        ++constructor_called;
    }

    ~some_obj() {
        ++destructor_called;
    }
};


template<class Alloc>
concept AllocatorRequirements = requires(Alloc alloc, std::size_t n)
{
    { *alloc.allocate(n) } -> std::same_as<typename Alloc::value_type&>;
    { alloc.deallocate(alloc.allocate(n), n) };
} && std::copy_constructible<Alloc>
  && std::equality_comparable<Alloc>;


template<typename T>
class test_allocator {
public:
    using value_type = T;
    using pointer = T*;
    using size_type = size_t;
    using is_always_equal = std::true_type;

    static inline int allocation_count = 0;
    static inline int elements_allocated = 0;

    test_allocator() = default;

    template<typename U>
    test_allocator(const test_allocator<U>& other) {
    }

    pointer allocate(size_type sz) {
        if constexpr (std::is_same_v<T, some_obj>) {
            ++test_allocator<some_obj>::allocation_count;
            test_allocator<some_obj>::elements_allocated += sz;
        } else {
            ++test_allocator<node_tag>::allocation_count;
            test_allocator<node_tag>::elements_allocated += sz;
        }
        return static_cast<pointer>(std::aligned_alloc(alignof(T), sizeof(T) * sz));
    }

    void deallocate(pointer p, std::size_t) {
        std::free(p);
    }

    bool operator==(const test_allocator& other) const {
        return true;
    }

};


static_assert(AllocatorRequirements<test_allocator<some_obj>>);

class WorkWithAllocatorTest : public testing::Test {
public:
    void SetUp() override {
        some_obj::constructor_called = 0;
        some_obj::destructor_called = 0;

        test_allocator<some_obj>::allocation_count = 0;
        test_allocator<some_obj>::elements_allocated = 0;

        test_allocator<node_tag>::allocation_count = 0;
        test_allocator<node_tag>::elements_allocated = 0;
    }

};

/*
    В тесте задаётся Capacity = 5, Extended = false.

    Ожидается, что будет:
        1. 1 аллокация буфера
*/
TEST_F(WorkWithAllocatorTest, reserve) {
    test_allocator<some_obj> allocator;
    circular_buffer<some_obj, false, test_allocator<some_obj>> buffer(5, allocator);

    ASSERT_EQ(test_allocator<some_obj>::allocation_count, 1);
    ASSERT_EQ(test_allocator<some_obj>::elements_allocated, 5);

    ASSERT_EQ(some_obj::constructor_called, 0);
    ASSERT_EQ(some_obj::destructor_called, 0);
}

/*
    В тесте задаётся Capacity = 5, Extended = false, а далее добавляется 5 элементов.

    Ожидается, что будет:
        1. 1 аллокация буфера
        2. 5 конструкторов и деструкторов у SomeObj
*/
TEST_F(WorkWithAllocatorTest, simplePushBack) {
    test_allocator<some_obj> allocator;
    circular_buffer<some_obj, false, test_allocator<some_obj>> buffer(5, allocator);
    for (int i = 0; i < 5; ++i) {
        buffer.push_back(some_obj{});
    }

    ASSERT_EQ(test_allocator<some_obj>::allocation_count, 1);
    ASSERT_EQ(test_allocator<some_obj>::elements_allocated, 5);

    ASSERT_EQ(some_obj::constructor_called, 5);
    ASSERT_EQ(some_obj::destructor_called, 5);
}

/*
    В тесте задаётся Capacity = 5, Extended = true, а далее добавляется 6 элементов.

    Ожидается, что будет:
        1. 2 аллокации буфера
        2. 6 конструкторов и деструкторов у some_obj
*/
#ifdef RUN_EXT_TESTS
TEST_F(WorkWithAllocatorTest, extendedPushBack) {
    test_allocator<some_obj> allocator;
    circular_buffer<some_obj, true, test_allocator<some_obj>> buffer(5, allocator);
    for (int i = 0; i < 6; ++i) {
        buffer.push_back(some_obj{});
    }

    ASSERT_EQ(test_allocator<some_obj>::allocation_count, 2);
    ASSERT_EQ(test_allocator<some_obj>::elements_allocated, 15);

    std::cout<< "DFD"<<some_obj::constructor_called << "DFD" << some_obj::destructor_called<< "DFD";
    ASSERT_EQ(some_obj::constructor_called, 6);
    ASSERT_EQ(some_obj::destructor_called, 11);

    
}
#endif
