#include "circular_buffer_interface.hpp"

template <typename T, bool Extendable, typename Allocator>
void circular_buffer<T, Extendable, Allocator>::extend() {
    if (capacity_ == 0) {
        data_ = std::allocator_traits<Allocator>::allocate(alloc_, 5);
        capacity_ = 5;
    } else {
        T* new_data =
            std::allocator_traits<Allocator>::allocate(alloc_, capacity_ * 2);
        auto it = begin();
        for (size_type i = 0; i < size_; i++) {
            std::allocator_traits<Allocator>::construct(alloc_, new_data + i,
                                                        std::move(*it));
            it++;
        }
        it = begin();
        for (size_type i = 0; i < size_; i++) {
            std::allocator_traits<Allocator>::destroy(alloc_, &(*it));
            it++;
        }
        std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
        data_ = new_data;
        start_ = 0;
        end_ = size_;
        capacity_ *= 2;
    }
}

template <typename T, bool Extendable, typename Allocator>
circular_buffer<T, Extendable, Allocator>::circular_buffer(size_type capacity) {
    data_ = std::allocator_traits<Allocator>::allocate(alloc_, capacity);
    capacity_ = capacity;
}

template <typename T, bool Extendable, typename Allocator>
circular_buffer<T, Extendable, Allocator>::circular_buffer(
    std::initializer_list<T> elements) {
    data_ = std::allocator_traits<Allocator>::allocate(alloc_, elements.size());
    for (auto it = elements.begin(); it != elements.end(); it++) {
        std::allocator_traits<Allocator>::construct(alloc_, data_ + end_, *it);
        end_++;
    }
    size_ = elements.size();
    capacity_ = size_;
    end_ = size_;
}

template <typename T, bool Extendable, typename Allocator>
circular_buffer<T, Extendable, Allocator>::circular_buffer(
    const circular_buffer& other)
    : alloc_(std::allocator_traits<
             Allocator>::select_on_container_copy_construction(other.alloc_)) {
    data_ = std::allocator_traits<Allocator>::allocate(alloc_, other.capacity_);
    for (auto it = other.begin(); it != other.end(); it++) {
        std::allocator_traits<Allocator>::construct(alloc_, data_ + end_, *it);
        end_++;
    }
    size_ = other.size_;
    capacity_ = other.capacity_;
}

template <typename T, bool Extendable, typename Allocator>
circular_buffer<T, Extendable, Allocator>::circular_buffer(
    circular_buffer&& other)
    : alloc_(std::move(other.alloc_)) {
    data_ = other.data_;
    capacity_ = other.capacity_;
    size_ = other.size_;
    start_ = other.start_;
    end_ = other.end_;

    other.data_ = nullptr;
    other.size_ = other.capacity_ = other.start_ = other.end_ = 0;
}

template <typename T, bool Extendable, typename Allocator>
circular_buffer<T, Extendable, Allocator>::circular_buffer(
    circular_buffer&& other, const Allocator& alloc)
    : alloc_(alloc) {
    if (other.alloc_ == alloc) {
        data_ = other.data_;
        capacity_ = other.capacity_;
        size_ = other.size_;
        start_ = other.start_;
        end_ = other.end_;

        other.data_ = nullptr;
        other.size_ = other.capacity_ = other.start_ = other.end_ = 0;

    } else {
        data_ =
            std::allocator_traits<Allocator>::allocate(alloc_, other.capacity_);
        for (auto it = other.begin(); it != other.end(); it++) {
            std::allocator_traits<Allocator>::construct(alloc_, data_ + end_,
                                                        std::move(*it));
            end_++;
        }
        size_ = other.size_;
        capacity_ = other.capacity_;
    }
}

template <typename T, bool Extendable, typename Allocator>
circular_buffer<T, Extendable, Allocator>&
circular_buffer<T, Extendable, Allocator>::operator=(
    const circular_buffer& other) {
    if (this == &other) {
        return *this;
    }

    if constexpr (std::allocator_traits<Allocator>::
                      propagate_on_container_copy_assignment::value) {
        if (alloc_ != other.alloc_) {
            T* new_data;
            size_type i = 0;
            try {
                new_data = std::allocator_traits<Allocator>::allocate(
                    other.alloc_, other.capacity_);
                for (; i < other.size_; i++) {
                    std::allocator_traits<Allocator>::construct(
                        other.alloc_, new_data + i,
                        other.data_[(other.start_ + i) % other.capacity_]);
                }
            } catch (...) {
                for (size_type j = 0; j < i; j++) {
                    std::allocator_traits<Allocator>::destroy(other.alloc_,
                                                              new_data + j);
                }
                std::allocator_traits<Allocator>::deallocate(
                    other.alloc_, new_data, other.capacity_);
                throw;
            }

            for (auto it = begin(); it != end(); it++) {
                std::allocator_traits<Allocator>::destroy(alloc_, &(*it));
            }
            std::allocator_traits<Allocator>::deallocate(alloc_, data_,
                                                         capacity_);
            data_ = new_data;
            alloc_ = other.alloc_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            start_ = 0;
            end_ = size_;
        } else {
            if (size_ >= other.size_) {
                for (size_type i = 0; i < other.size_; i++) {
                    data_[(start_ + i) % capacity_] =
                        other[(other.start_ + i) % other.capacity_];
                }
                for (size_type i = other.size_; i < size_; i++) {
                    std::allocator_traits<Allocator>::destroy(
                        alloc_, data_ + (start_ + i) % capacity_);
                }
                size_ = other.size_;
                end_ = (start_ + size_) % capacity_;
            } else {
                while (capacity_ < other.capacity_) {
                    extend();
                }
                for (auto it = begin(); it != end(); it++) {
                    std::allocator_traits<Allocator>::destroy(alloc_, &(*it));
                }
                for (size_type i = 0; i < other.size_; i++) {
                    std::allocator_traits<Allocator>::construct(
                        alloc_, &data_[(start_ + i) % capacity_],
                        other[(other.start_ + i) % other.capacity_]);
                }
                size_ = other.size_;
                end_ = (start_ + size_) % capacity_;
            }
        }
    } else {
        while (capacity_ < other.capacity_) {
            extend();
        }
        for (auto it = begin(); it != end(); it++) {
            std::allocator_traits<Allocator>::destroy(alloc_, &(*it));
        }
        for (size_type i = 0; i < other.size_; i++) {
            std::allocator_traits<Allocator>::construct(
                alloc_, &data_[(start_ + i) % capacity_],
                other[(other.start_ + i) % other.capacity_]);
        }
        size_ = other.size_;
        end_ = (start_ + size_) % capacity_;
    }
}

template <typename T, bool Extendable, typename Allocator>
circular_buffer<T, Extendable, Allocator>&
circular_buffer<T, Extendable, Allocator>::operator=(circular_buffer&& other) {
    if (this == &other) {
        return *this;
    }

    if constexpr (std::allocator_traits<Allocator>::
                      propagate_on_container_move_assignment::value) {
        for (auto it = begin(); it != end(); it++) {
            std::allocator_traits<Allocator>::destroy(alloc_, &(*it));
        }
        std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);

        alloc_ = std::move(other.alloc_);
        data_ = other.data_;
        capacity_ = other.capacity_;
        size_ = other.size_;
        start_ = other.start_;
        end_ = other.end_;

        other.end_ = other.start_ = other.size_ = other.capacity_ = 0;
        other.data_ = nullptr;
    } else if (alloc_ == other.alloc_) {
        for (auto it = begin(); it != end(); it++) {
            std::allocator_traits<Allocator>::destroy(alloc_, &(*it));
        }
        std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);

        data_ = other.data_;
        capacity_ = other.capacity_;
        size_ = other.size_;
        start_ = other.start_;
        end_ = other.end_;

        other.end_ = other.start_ = other.size_ = other.capacity_ = 0;
        other.data_ = nullptr;
    } else {
        for (auto it = begin(); it != end(); it++) {
            std::allocator_traits<Allocator>::destroy(alloc_, &(*it));
        }
        start_ = end_ = 0;
        if (capacity_ < other.capacity_) {
            if (data_ != nullptr) {
                std::allocator_traits<Allocator>::deallocate(alloc_, data_,
                                                             capacity_);
            }
            data_ = std::allocator_traits<Allocator>::allocate(alloc_,
                                                               other.capacity_);
            capacity_ = other.capacity_;
        }

        for (auto it = other.begin(); it != other.end(); it++) {
            std::allocator_traits<Allocator>::construct(alloc_, data_ + end_,
                                                        std::move(*it));
            end_++;
        }
        size_ = other.size_;
    }

    return *this;
}

template <typename T, bool Extendable, typename Allocator>
circular_buffer<T, Extendable, Allocator>::circular_buffer(size_type n,
                                                           const T& value) {
    capacity_ = n;
    data_ = std::allocator_traits<Allocator>::allocate(alloc_, n);
    for (size_type i = 0; i < n; i++) {
        std::allocator_traits<Allocator>::construct(alloc_, data_ + i, value);
    }
    end_ = n;
    size_ = n;
}

template <typename T, bool Extendable, typename Allocator>
void circular_buffer<T, Extendable, Allocator>::clear() {
    for (auto it = begin(); it != end(); it++) {
        std::allocator_traits<Allocator>::destroy(alloc_, &(*it));
    }
    start_ = 0;
    end_ = 0;
    size_ = 0;
}

template <typename T, bool Extendable, typename Allocator>
template <typename U>
void circular_buffer<T, Extendable, Allocator>::push_back(U&& element) {
    if constexpr (Extendable) {
        if (size_ == capacity_) {
            extend();
        }
    } else {
        if (size_ == capacity_) {
            std::allocator_traits<Allocator>::destroy(alloc_, data_ + start_);
            start_ = (start_ + 1) % capacity_;
            size_--;
        }
    }

    std::allocator_traits<Allocator>::construct(
        alloc_, data_ + end_ % capacity_, std::forward<U>(element));
    size_++;
    end_ = (end_ + 1) % capacity_;
}

template <typename T, bool Extendable, typename Allocator>
template <typename U>
void circular_buffer<T, Extendable, Allocator>::push_front(U&& element) {
    if constexpr (Extendable) {
        if (size_ == capacity_) {
            extend();
        }
    } else {
        if (size_ == capacity_) {
            std::allocator_traits<Allocator>::destroy(
                alloc_, data_ + (start_ + size_ - 1) % capacity_);
            size_--;
        }
    }

    if (start_ == 0) {
        start_ = capacity_ - 1;
    } else {
        start_ = start_ - 1;
    }
    std::allocator_traits<Allocator>::construct(alloc_, data_ + start_,
                                                std::forward<U>(element));
    size_++;
    end_ = (start_ + size_) % capacity_;
}

template <typename T, bool Extendable, typename Allocator>
void circular_buffer<T, Extendable, Allocator>::pop_front() {
    if (size_ == 0) {
        throw std::out_of_range("container is empty. cant pop.");
    }

    std::allocator_traits<Allocator>::destroy(alloc_, data_ + start_);
    start_ = (start_ + 1) % capacity_;
    size_--;
}

template <typename T, bool Extendable, typename Allocator>
void circular_buffer<T, Extendable, Allocator>::pop_back() {
    if (size_ == 0) {
        throw std::out_of_range("container is empty. cant pop.");
    }

    if (end_ == 0) {
        end_ = capacity_ - 1;
    } else {
        end_ = end_ - 1;
    }

    std::allocator_traits<Allocator>::destroy(alloc_, data_ + end_);
    size_--;
}

template <typename T, bool Extendable, typename Allocator>
Allocator circular_buffer<T, Extendable, Allocator>::get_allocator() const {
    return alloc_;
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::front() const
    -> const_reference {
    if (size_ == 0) {
        throw std::out_of_range("0 elements in container");
    }
    return *begin();
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::back() const
    -> const_reference {
    if (size_ == 0) {
        throw std::out_of_range("0 elements in container");
    }
    return *(end() - 1);
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::front() -> reference {
    if (size_ == 0) {
        throw std::out_of_range("0 elements in container");
    }
    return *begin();
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::back() -> reference {
    if (size_ == 0) {
        throw std::out_of_range("0 elements in container");
    }
    return *(end() - 1);
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::begin() -> iterator {
    return iterator(this, 0);
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::end() -> iterator {
    return iterator(this, size_);
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::begin() const
    -> const_iterator {
    return const_iterator(this, 0);
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::end() const -> const_iterator {
    return const_iterator(this, size_);
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::cbegin() const
    -> const_iterator {
    return const_iterator(this, 0);
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::cend() const -> const_iterator {
    return const_iterator(this, size_);
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::rbegin() -> reverse_iterator {
    return reverse_iterator(end());
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::rend() -> reverse_iterator {
    return reverse_iterator(begin());
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::rbegin() const
    -> const_reverse_iterator {
    return const_reverse_iterator(end());
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::rend() const
    -> const_reverse_iterator {
    return const_reverse_iterator(begin());
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::crbegin() const
    -> const_reverse_iterator {
    return const_reverse_iterator(cend());
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::crend() const
    -> const_reverse_iterator {
    return const_reverse_iterator(cbegin());
}

template <typename T, bool Extendable, typename Allocator>
template <typename ForwardIterator, bool>
auto circular_buffer<T, Extendable, Allocator>::insert(const_iterator it,
                                                       ForwardIterator from,
                                                       ForwardIterator to)
    -> iterator {
    size_type n = std::distance(from, to);
    if (n == 0) {
        return iterator{this, it.index_};
    }
    if constexpr (Extendable) {
        while (size_ + n > capacity_) {
            extend();
        }

        for (size_type i = size_; i < size_ + n; i++) {
            std::allocator_traits<Allocator>::construct(
                alloc_, data_ + (start_ + i) % capacity_);
        }

        for (size_type i = size_; i > it.index_; i--) {
            size_type from_ind = (start_ + i - 1) % capacity_;
            size_type to_ind = (start_ + i + n - 1) % capacity_;
            data_[to_ind] = std::move(data_[from_ind]);
        }
        iterator ret_it{this, it.index_};
        size_type ind = it.index_;
        size_ += n;
        for (; from != to; from++, it++) {
            data_[(start_ + it.index_) % capacity_] = *from;
        }
        end_ = (start_ + size_) % capacity_;
        return ret_it;
    } else {
        if (size_ + n <= capacity_) {
            for (size_type i = size_; i < size_ + n; i++) {
                std::allocator_traits<Allocator>::construct(
                    alloc_, data_ + (start_ + i) % capacity_);
            }

            for (size_type i = size_; i > it.index_; i--) {
                size_type from_ind = (start_ + i - 1) % capacity_;
                size_type to_ind = (start_ + i + n - 1) % capacity_;
                data_[to_ind] = std::move(data_[from_ind]);
            }
            iterator ret_it{this, it.index_};
            size_type ind = it.index_;
            size_ += n;
            for (; from != to; from++, it++) {
                data_[(start_ + it.index_) % capacity_] = *from;
            }
            end_ = (start_ + size_) % capacity_;

            return ret_it;
        } else {
            circular_buffer tmp(size_ + n);
            tmp.assign(begin(), end());
            tmp.insert(tmp.begin() + it.index_, from, to);
            for (auto it = begin(); it != end(); it++) {
                std::allocator_traits<Allocator>::destroy(alloc_, &(*it));
            }
            size_type ind = capacity_;
            for (auto it = tmp.rbegin(); it != tmp.rend() && ind > 0;
                 it++, ind--) {
                std::allocator_traits<Allocator>::construct(
                    alloc_, data_ + ind - 1, std::move(*it));
            }
            size_ = capacity_;
            start_ = 0;
            end_ = size_;
            return begin() + it.index_;
        }
    }
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::insert(const_iterator it,
                                                       const value_type& val)
    -> iterator {
    value_type other = val;
    return insert(it, &other, &other + 1);
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::insert(const_iterator it,
                                                       size_type cnt,
                                                       const value_type& val)
    -> iterator {
    repeat_iterator<T, true> first(val, 0);
    repeat_iterator<T, true> last(val, cnt);

    return insert(it, first, last);
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::insert(
    const_iterator it, std::initializer_list<T> elements) -> iterator {
    return insert(it, elements.begin(), elements.end());
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::erase(const_iterator it)
    -> iterator {
    if (size_ == 0 || it.index_ >= size_) {
        throw std::out_of_range(
            "erase(iterator) cant be executed. invalid iterator");
    }

    for (size_type ind = it.index_; ind < size_ - 1; ind++) {
        data_[(start_ + ind) % capacity_] =
            std::move(data_[(start_ + ind + 1) % capacity_]);
    }
    std::allocator_traits<Allocator>::destroy(
        alloc_, data_ + (start_ + size_ - 1) % capacity_);
    size_--;
    end_ = (start_ + size_) % capacity_;

    return iterator{this, it.index_};
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::erase(const_iterator start,
                                                      const_iterator end)
    -> iterator {
    if (start > end || end.index_ > size_) {
        throw std::out_of_range("erase() called with invalid iterator range");
    }

    for (size_type ind = start.index_; ind < size_ - (end - start); ind++) {
        data_[(start_ + ind) % capacity_] =
            std::move(data_[(start_ + ind + (end - start)) % capacity_]);
    }
    for (size_type to_destroy = 1; to_destroy <= (end - start); to_destroy++) {
        std::allocator_traits<Allocator>::destroy(
            alloc_, data_ + (start_ + size_ - to_destroy) % capacity_);
    }
    size_ -= (end - start);
    end_ = (start_ + size_) % capacity_;

    return iterator{this, start.index_};
}

template <typename T, bool Extendable, typename Allocator>
void circular_buffer<T, Extendable, Allocator>::assign(size_type n,
                                                       const T& value) {
    if constexpr (Extendable) {
        while (capacity_ < n) {
            extend();
        }
    } else {
        n = (capacity_ < n) ? capacity_ : n;
    }
    for (auto it = begin(); it != end(); it++) {
        std::allocator_traits<Allocator>::destroy(
            alloc_, data_ + (start_ + it.index_) % capacity_);
    }

    for (size_type i = 0; i < n; i++) {
        std::allocator_traits<Allocator>::construct(
            alloc_, data_ + (start_ + i) % capacity_, value);
    }
    size_ = n;
    end_ = (start_ + size_) % capacity_;
}

template <typename T, bool Extendable, typename Allocator>
void circular_buffer<T, Extendable, Allocator>::assign(
    std::initializer_list<T> elements) {
    size_type n = elements.size();
    if constexpr (Extendable) {
        while (capacity_ < n) {
            extend();
        }
    } else {
        n = (capacity_ < n) ? capacity_ : n;
    }
    for (auto it = begin(); it != end(); it++) {
        std::allocator_traits<Allocator>::destroy(
            alloc_, data_ + (start_ + it.index_) % capacity_);
    }

    size_type ind = 0;
    for (auto it = elements.begin(); it != elements.end() && ind < n; it++) {
        std::allocator_traits<Allocator>::construct(
            alloc_, data_ + (start_ + ind++) % capacity_, *it);
    }
    size_ = n;
    end_ = (start_ + size_) % capacity_;
}

template <typename T, bool Extendable, typename Allocator>
template <typename ForwardIterator, bool>
void circular_buffer<T, Extendable, Allocator>::assign(ForwardIterator from,
                                                       ForwardIterator to) {
    size_type n = 0;
    for (auto it = from; it != to; it++, n++) {
    }
    if constexpr (Extendable) {
        while (capacity_ < n) {
            extend();
        }
    } else {
        n = (capacity_ < n) ? capacity_ : n;
    }
    for (auto it = begin(); it != end(); it++) {
        std::allocator_traits<Allocator>::destroy(
            alloc_, data_ + (start_ + it.index_) % capacity_);
    }

    size_type ind = 0;
    for (auto it = from; it != to && ind < n; it++) {
        std::allocator_traits<Allocator>::construct(
            alloc_, data_ + (start_ + ind++) % capacity_, *it);
    }
    size_ = n;
    end_ = (start_ + size_) % capacity_;
}

template <typename T, bool Extendable, typename Allocator>
void circular_buffer<T, Extendable, Allocator>::resize(size_type n) {
    if constexpr (Extendable) {
        while (n > capacity_) {
            extend();
        }
    } else {
        n = (n > capacity_ ? capacity_ : n);
    }

    if (size_ < n) {
        for (size_type ind = size_; ind < n; ind++) {
            std::allocator_traits<Allocator>::construct(
                alloc_, data_ + (start_ + ind) % capacity_, T{});
        }
    } else {
        for (size_type ind = n; ind < size_; ind++) {
            std::allocator_traits<Allocator>::destroy(
                alloc_, data_ + (start_ + ind) % capacity_);
        }
    }
    size_ = n;
    end_ = (start_ + size_) % capacity_;
}

template <typename T, bool Extendable, typename Allocator>
void circular_buffer<T, Extendable, Allocator>::resize(size_type n,
                                                       const T& val) {
    if constexpr (Extendable) {
        while (n > capacity_) {
            extend();
        }
    } else {
        n = (n > capacity_ ? capacity_ : n);
    }

    if (size_ < n) {
        for (size_type ind = size_; ind < n; ind++) {
            std::allocator_traits<Allocator>::construct(
                alloc_, data_ + (start_ + ind) % capacity_, val);
        }
    } else {
        for (size_type ind = n; ind < size_; ind++) {
            std::allocator_traits<Allocator>::destroy(
                alloc_, data_ + (start_ + ind) % capacity_);
        }
    }
    size_ = n;
    end_ = (start_ + size_) % capacity_;
}

template <typename T, bool Extendable, typename Allocator>
circular_buffer<T, Extendable, Allocator>::~circular_buffer() {
    for (auto it = begin(); it != end(); it++) {
        std::allocator_traits<Allocator>::destroy(alloc_, &(*it));
    }
    std::allocator_traits<Allocator>::deallocate(alloc_, data_, capacity_);
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::operator[](size_type index)
    -> reference {
    return data_[(start_ + index) % capacity_];
}

template <typename T, bool Extendable, typename Allocator>
auto circular_buffer<T, Extendable, Allocator>::operator[](
    size_type index) const -> const_reference {
    return data_[(start_ + index) % capacity_];
}