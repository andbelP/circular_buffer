template <typename T, bool Extendable, typename Allocator>
class circular_buffer;

#define CntnrIterator container_iterator<T, Extendable, Allocator, IsConst>

template <typename T, bool Extendable, typename Allocator, bool IsConst>
class container_iterator {
    template <typename, bool, typename, bool>
    friend class container_iterator;

    friend circular_buffer<T, Extendable, Allocator>;

   public:
    using iterator_category = std::random_access_iterator_tag;
    using reference = std::conditional<IsConst, const T&, T&>::type;
    using pointer = std::conditional<IsConst, const T*, T*>::type;
    using value_type = T;
    using difference_type =
        typename circular_buffer<T, Extendable, Allocator>::difference_type;
    using size_type =
        typename circular_buffer<T, Extendable, Allocator>::size_type;

   private:
    using buffer_type =
        std::conditional<IsConst,
                         const circular_buffer<T, Extendable, Allocator>,
                         circular_buffer<T, Extendable, Allocator>>::type;
    buffer_type* buffer_;
    size_type index_;

   public:
    container_iterator() = default;
    container_iterator(buffer_type* buffer, size_type index)
        : buffer_(buffer), index_(index) {}

    container_iterator(
        const container_iterator<T, Extendable, Allocator, false>& other)
        : buffer_(other.buffer_), index_(other.index_) {};

    bool operator==(const container_iterator& other) const;
    bool operator!=(const container_iterator& other) const;
    bool operator>=(const container_iterator& other) const;
    bool operator<=(const container_iterator& other) const;
    bool operator>(const container_iterator& other) const;
    bool operator<(const container_iterator& other) const;
    container_iterator operator+(difference_type n) const;

    container_iterator& operator++();
    container_iterator operator++(int);
    container_iterator& operator--();
    container_iterator operator--(int);

    container_iterator operator-(difference_type n) const;
    difference_type operator-(
        const container_iterator<T, Extendable, Allocator, true>& other) const;
    reference operator[](size_type ind) const;
    reference operator*() const;
};

template <typename T, bool IsConst = false>
class repeat_iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using reference = std::conditional<IsConst, const T&, T&>::type;
    using pointer = std::conditional<IsConst, const T*, T*>::type;
    using value_type = T;
    using difference_type = int64_t;
    using size_type = size_t;

   private:
    reference value_{};
    size_type pos_{};

   public:
    repeat_iterator(reference value, size_type pos)
        : value_(value), pos_(pos) {}

    reference operator*() const { return value_; }

    repeat_iterator& operator++() {
        pos_++;
        return *this;
    }

    repeat_iterator operator++(int) {
        repeat_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator==(const repeat_iterator& a, const repeat_iterator& b) {
        return a.pos_ == b.pos_;
    }

    friend bool operator!=(const repeat_iterator& a, const repeat_iterator& b) {
        return !(a == b);
    }
};

template <typename Iterator>
class reverse_iterator_adapter {
    Iterator it_{};

   public:
    using iterator_category = Iterator::iterator_category;
    using reference = Iterator::reference;
    using pointer = Iterator::pointer;
    using value_type = Iterator::value_type;
    using difference_type = Iterator::difference_type;
    using size_type = Iterator::size_type;
    reverse_iterator_adapter() = default;
    reverse_iterator_adapter(Iterator it) : it_(it) {}
    reverse_iterator_adapter& operator++() {
        --it_;
        return *this;
    }
    reverse_iterator_adapter operator++(int) {
        auto to_ret = *this;
        it_--;
        return to_ret;
    }
    reverse_iterator_adapter& operator--() {
        ++it_;
        return *this;
    }
    reverse_iterator_adapter operator--(int) {
        auto to_ret = *this;
        it_++;
        return to_ret;
    }

    reverse_iterator_adapter operator+(typename Iterator::difference_type n) const {
        reverse_iterator_adapter result = *this;
        result.it_ = result.it_ - n;
        return result;
    }

    reverse_iterator_adapter operator-(typename Iterator::difference_type n) const {
        reverse_iterator_adapter result = *this;
        result.it_ = result.it_ + n;
        return result;
    }
    difference_type operator-(const reverse_iterator_adapter& other) const {
        return other.it_ - it_;
    }
    decltype(auto) operator*() const {
        auto tmp = it_;
        --tmp;
        return *tmp;
    }

    decltype(auto) operator[](typename Iterator::difference_type ind) const {
        return *(*this + ind);
    }

    bool operator==(const reverse_iterator_adapter other) const {
        return other.it_ == it_;
    }
    bool operator!=(const reverse_iterator_adapter other) const {
        return !(other.it_ == it_);
    }
};

template <typename T, bool Extendable, typename Allocator, bool IsConst>
bool CntnrIterator::operator==(const container_iterator& other) const {
    return (buffer_ == other.buffer_ && index_ == other.index_);
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
bool CntnrIterator::operator!=(const container_iterator& other) const {
    return !(*this == other);
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
bool CntnrIterator::operator>=(const container_iterator& other) const {
    return (buffer_ == other.buffer_ && index_ >= other.index_);
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
bool CntnrIterator::operator<=(const container_iterator& other) const {
    return !(*this >= other) || *this == other;
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
bool CntnrIterator::operator<(const container_iterator& other) const {
    return !(*this >= other);
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
bool CntnrIterator::operator>(const container_iterator& other) const {
    return !(*this <= other);
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
CntnrIterator CntnrIterator::operator+(difference_type n) const {
    container_iterator result;
    result.buffer_ = buffer_;
    result.index_ = index_ + n;
    return result;
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
CntnrIterator& CntnrIterator::operator++() {
    index_++;
    return *this;
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
CntnrIterator CntnrIterator::operator++(int) {
    container_iterator result = *this;
    index_ = index_ + 1;
    return result;
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
CntnrIterator& CntnrIterator::operator--() {
    index_--;
    return *this;
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
CntnrIterator CntnrIterator::operator--(int) {
    container_iterator result = *this;
    index_ = index_ - 1;
    return result;
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
CntnrIterator CntnrIterator::operator-(difference_type n) const {
    container_iterator result;
    result.index_ = index_ - n;
    result.buffer_ = buffer_;
    return result;
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
auto CntnrIterator::operator-(
    const container_iterator<T, Extendable, Allocator, true>& other) const
    -> difference_type {
    return index_ - other.index_;
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
auto CntnrIterator::operator[](size_type ind) const -> reference {
    return *(*this + ind);
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
auto CntnrIterator::operator*() const -> reference {
    size_type real_ind = (buffer_->start_ + index_) % buffer_->capacity_;
    return ((buffer_->data_)[real_ind]);
}
