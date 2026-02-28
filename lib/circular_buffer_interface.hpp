#include <memory>

template<typename T, bool Extendable = false, typename Allocator = std::allocator<T>>
class circular_buffer {

    template<typename ValueType>
    class ContainerIterator;

public:

    using value_type = T;
    using reference = T&;
    using const_reference=const T&;
    using size_type = typename std::allocator_traits<Allocator>::size_type;
    using iterator=ContainerIterator<T>;
    using const_iterator=ContainerIterator<const T>;
    using reverse_iterator=std::reverse_iterator<iterator>;
    using const_reverse_iterator=std::reverse_iterator<const_iterator>;
    using difference_type=std::allocator_traits<Allocator>::difference_type;
    using allocator_type=Allocator;

private:

    Allocator alloc_;

    T* data_=nullptr;
    size_t capacity_{};
    size_t size_{};

    size_t write_ind_{};
    size_t read_ind_{};

    template<typename ValueType>
    class ContainerIterator{
    public:
        using iterator_category=std::random_access_iterator_tag;
        using reference=reference;
        using pointer = iterator*;
        using value_type=value_type;
        using difference_type=difference_type;
        
    private:
        circular_buffer* buffer_;
        ValueType* ptr_;

    public:

        ContainerIterator()=default;

        operator const_iterator() const {
            return const_iterator{};
        }

        bool operator==(const ContainerIterator& other) const;
        bool operator!=(const ContainerIterator& other) const;
        bool operator>=(const ContainerIterator& other) const;
        bool operator<=(const ContainerIterator& other) const;
        bool operator>(const ContainerIterator& other) const;
        bool operator<(const ContainerIterator& other) const;
        iterator operator+(difference_type n) const;

        iterator& operator++();
        iterator operator++(int);
        iterator& operator--();
        iterator operator--(int);

        iterator operator-(difference_type n) const;
        difference_type operator-(const ContainerIterator& other) const;
        reference operator[](difference_type ind) const;
        reference operator*() const;

    };


    void Extend();

public:

    circular_buffer() = default;

    circular_buffer(Allocator&& alloc) : alloc_(std::move(alloc)){}

    circular_buffer(Allocator& alloc) : alloc_(alloc){}

    circular_buffer(circular_buffer&& other, Allocator alloc) : alloc_(alloc){}

    circular_buffer(size_type capacity);

    circular_buffer(size_type n, const T& value);

    circular_buffer(std::initializer_list<T> elements);

    circular_buffer(const circular_buffer& other);

    circular_buffer(circular_buffer&& other);

    Allocator get_allocator() const;

    size_t size() const{ return size_; };

    size_t max_size() const{return capacity_;}

    bool empty() const{return size_==0;};

    void clear();

    const_reference front() const;
    const_reference back() const;

    reference front();
    reference back();

    template<typename U>
    void push_back(U&& element);

    void pop_back();

    template<typename U>
    void push_front(U&& element);

    void pop_front();

    iterator insert(const_iterator iter, const value_type& val);

    template<typename TIterator>
    iterator insert(const_iterator iter, TIterator from, TIterator to);

    iterator insert(const_iterator iter, size_type cnt, const value_type& val);
    iterator insert(const_iterator iter, std::initializer_list<T> elements);

    iterator erase(const_iterator iter);
    iterator erase(const_iterator start, const_iterator end);

    void assign(size_type n, const T& value);
    void assign(std::initializer_list<T> values);

    template<typename TIterator>
    void assign(TIterator from, TIterator to);

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