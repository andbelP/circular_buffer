#include <memory>

#include "iterators.hpp"

template <typename T, bool Extendable = false,
          typename Allocator = std::allocator<T>>
class circular_buffer {
   public:
    friend ContainerIterator<T, Extendable, Allocator, false>;
    friend ContainerIterator<T, Extendable, Allocator, true>;

    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using size_type = typename std::allocator_traits<Allocator>::size_type;
    using iterator = ContainerIterator<T, Extendable, Allocator, false>;
    using const_iterator = ContainerIterator<T, Extendable, Allocator, true>;
    using reverse_iterator = ReverseIterator<iterator>;
    using const_reverse_iterator = ReverseIterator<const_iterator>;
    using difference_type = std::allocator_traits<Allocator>::difference_type;
    using allocator_type = Allocator;

   private:
    Allocator alloc_{};

    T* data_ = nullptr;
    size_type capacity_{};
    size_type size_{};

    size_type start_{};
    size_type end_{};

    void Extend();

    void ShiftRight(iterator from, size_type n);

   public:
    circular_buffer() = default;

    circular_buffer(Allocator&& alloc) : alloc_(std::move(alloc)) {}

    circular_buffer(const Allocator& alloc) : alloc_(alloc) {}

    circular_buffer(circular_buffer&& other, const Allocator& alloc);

    circular_buffer(size_type capacity);

    circular_buffer(size_type n, const T& value);

    circular_buffer(std::initializer_list<T> elements);

    circular_buffer(const circular_buffer& other);

    circular_buffer(circular_buffer&& other);

    Allocator get_allocator() const;

    size_type size() const { return size_; };

    size_type max_size() const { return capacity_; }

    bool empty() const { return size_ == 0; };

    void clear();

    const_reference front() const;
    const_reference back() const;

    reference front();
    reference back();

    template <typename U>
    void push_back(U&& element);

    void pop_back();

    template <typename U>
    void push_front(U&& element);

    void pop_front();

    iterator insert(const_iterator iter, const value_type& val);

    template <typename ForwardIterator,
              bool isIterator = std::is_base_of<
                  std::forward_iterator_tag,
                  typename std::iterator_traits<
                      ForwardIterator>::iterator_category>::value>
    iterator insert(const_iterator iter, ForwardIterator from,
                    ForwardIterator to);

    iterator insert(const_iterator iter, size_type cnt, const value_type& val);
    iterator insert(const_iterator iter, std::initializer_list<T> elements);

    iterator erase(const_iterator iter);
    iterator erase(const_iterator start, const_iterator end);

    void assign(size_type n, const T& value);
    void assign(std::initializer_list<T> values);

    template <typename ForwardIterator,
              bool isIterator = std::is_base_of<
                  std::forward_iterator_tag,
                  typename std::iterator_traits<
                      ForwardIterator>::iterator_category>::value>
    void assign(ForwardIterator from, ForwardIterator to);

    void resize(size_type n);
    void resize(size_type n, const T& val);

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

    reverse_iterator rbegin();
    reverse_iterator rend();

    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;

    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    bool operator==(const circular_buffer& other);
    bool operator!=(const circular_buffer& other);

    circular_buffer& operator=(const circular_buffer& other);
    circular_buffer& operator=(circular_buffer&& other);
    circular_buffer& operator=(std::initializer_list<value_type> other);
};