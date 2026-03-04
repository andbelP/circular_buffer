template <typename T, bool Extendable, typename Allocator>
class circular_buffer;

#define CntnrIterator ContainerIterator<T, Extendable, Allocator, IsConst>

template <typename T, bool Extendable, typename Allocator, bool IsConst>
class ContainerIterator {
    template <typename, bool, typename, bool>
    friend class ContainerIterator;

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
    ContainerIterator() = default;
    ContainerIterator(buffer_type* buffer, size_type index)
        : buffer_(buffer), index_(index) {}

    ContainerIterator(
        const ContainerIterator<T, Extendable, Allocator, false>& other)
        : buffer_(other.buffer_), index_(other.index_) {};

    bool operator==(const ContainerIterator& other) const;
    bool operator!=(const ContainerIterator& other) const;
    bool operator>=(const ContainerIterator& other) const;
    bool operator<=(const ContainerIterator& other) const;
    bool operator>(const ContainerIterator& other) const;
    bool operator<(const ContainerIterator& other) const;
    ContainerIterator operator+(difference_type n) const;

    ContainerIterator& operator++();
    ContainerIterator operator++(int);
    ContainerIterator& operator--();
    ContainerIterator operator--(int);

    ContainerIterator operator-(difference_type n) const;
    difference_type operator-(
        const ContainerIterator<T, Extendable, Allocator, true>& other) const;
    reference operator[](size_type ind) const;
    reference operator*() const;
};

template <typename Iterator>
class ReverseIterator {
    Iterator it_{};

   public:
    using iterator_category = Iterator::iterator_category;
    using reference = Iterator::reference;
    using pointer = Iterator::pointer;
    using value_type = Iterator::value_type;
    using difference_type = Iterator::difference_type;
    using size_type = Iterator::size_type;
    ReverseIterator() = default;
    ReverseIterator(Iterator it) : it_(it) {}
    ReverseIterator& operator++() {
        --it_;
        return *this;
    }
    ReverseIterator operator++(int) {
        auto to_ret = *this;
        it_--;
        return to_ret;
    }
    ReverseIterator& operator--() {
        ++it_;
        return *this;
    }
    ReverseIterator operator--(int) {
        auto to_ret = *this;
        it_++;
        return to_ret;
    }

    ReverseIterator operator+(typename Iterator::difference_type n) const {
        ReverseIterator result = *this;
        result.it_ = result.it_ - n;
        return result;
    }

    ReverseIterator operator-(typename Iterator::difference_type n) const {
        ReverseIterator result = *this;
        result.it_ = result.it_ + n;
        return result;
    }
    difference_type operator-(const ReverseIterator& other) const {
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

    bool operator==(const ReverseIterator other) const {
        return other.it_ == it_;
    }
    bool operator!=(const ReverseIterator other) const {
        return !(other.it_ == it_);
    }
};

template <typename T, bool Extendable, typename Allocator, bool IsConst>
bool CntnrIterator::operator==(const ContainerIterator& other) const {
    return (buffer_ == other.buffer_ && index_ == other.index_);
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
bool CntnrIterator::operator!=(const ContainerIterator& other) const {
    return !(*this == other);
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
bool CntnrIterator::operator>=(const ContainerIterator& other) const {
    return (buffer_ == other.buffer_ && index_ >= other.index_);
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
bool CntnrIterator::operator<=(const ContainerIterator& other) const {
    return !(*this >= other) || *this == other;
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
bool CntnrIterator::operator<(const ContainerIterator& other) const {
    return !(*this >= other);
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
bool CntnrIterator::operator>(const ContainerIterator& other) const {
    return !(*this <= other);
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
CntnrIterator CntnrIterator::operator+(difference_type n) const {
    ContainerIterator result;
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
    ContainerIterator result = *this;
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
    ContainerIterator result = *this;
    index_ = index_ - 1;
    return result;
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
CntnrIterator CntnrIterator::operator-(difference_type n) const {
    ContainerIterator result;
    result.index_ = index_ - n;
    result.buffer_ = buffer_;
    return result;
}

template <typename T, bool Extendable, typename Allocator, bool IsConst>
auto CntnrIterator::operator-(
    const ContainerIterator<T, Extendable, Allocator, true>& other) const
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
