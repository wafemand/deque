//
// Created by andrey on 24.06.18.
//

#ifndef EXAM_DEQUE_MY_DEQUE_H
#define EXAM_DEQUE_MY_DEQUE_H


#include <cstddef>
#include <iterator>
#include <algorithm>
#include <vector>
#include <memory>
#include <cassert>
#include "deque"


template<typename T>
class my_deque {

    template<typename _Tp>
    struct RA_iterator {
    public:
        typedef std::random_access_iterator_tag iterator_category;
        typedef _Tp value_type;
        typedef ptrdiff_t difference_type;
        typedef _Tp *pointer;
        typedef _Tp &reference;

        friend class my_deque;

    private:
        RA_iterator(size_t pos, size_t start, size_t capacity, pointer data)
                : pos(pos),
                  start_(start),
                  capacity_(capacity),
                  data_(data) {}

        size_t cycle_add(size_t val, ptrdiff_t delta) const {
            ptrdiff_t signed_val = val;
            signed_val += delta;
            signed_val %= capacity_;
            if (signed_val < 0) {
                signed_val += capacity_;
            }
            return static_cast<size_t>(signed_val);
        }

    public:
        template<typename U>
        RA_iterator(RA_iterator<U> const &other,
                    typename std::enable_if<std::is_same<U const, _Tp>::value &&
                                            std::is_const<_Tp>::value>::type * = nullptr) {
            pos = other.pos;
            data_ = other.data_;
            capacity_ = other.capacity_;
            start_ = other.start_;
        };

        RA_iterator &operator++() {
            pos++;
            return *this;
        }

        RA_iterator operator++(int) {
            auto res = *this;
            ++(*this);
            return res;
        }

        RA_iterator &operator--() {
            pos--;
            return *this;
        }

        RA_iterator operator--(int) {
            auto res = *this;
            --(*this);
            return res;
        }

        reference operator*() {
            return data_[cycle_add(start_, pos)];
        }

        pointer operator->() {
            return data_ + cycle_add(start_, pos);
        }

        RA_iterator &operator+=(difference_type diff) {
            pos += diff;
            return *this;
        }

        RA_iterator &operator-=(difference_type diff) {
            pos -= diff;
            return *this;
        }

        RA_iterator &operator=(RA_iterator const &other) = default;

        reference operator[](difference_type diff) const {
            return *RA_iterator(*this + diff);
        }

        size_t get_index() const {
            return pos;
        }

        friend RA_iterator operator+(RA_iterator it, difference_type diff) {
            return it += diff;
        }

        friend RA_iterator operator-(RA_iterator it, difference_type diff) {
            return it -= diff;
        }

        friend difference_type operator-(RA_iterator const &a, RA_iterator const &b) {
            assert(a.data_ == b.data_);
            return difference_type(a.pos - b.pos);
        }

        friend bool operator<(RA_iterator const &a, RA_iterator const &b) {
            return a.pos < b.pos;
        }

        friend bool operator<=(RA_iterator const &a, RA_iterator const &b) {
            return a.pos <= b.pos;
        }

        friend bool operator>(RA_iterator const &a, RA_iterator const &b) {
            return a.pos > b.pos;
        }

        friend bool operator>=(RA_iterator const &a, RA_iterator const &b) {
            return a.pos >= b.pos;
        }

        friend bool operator==(RA_iterator const &a, RA_iterator const &b) {
            return a.pos == b.pos && a.data_ == b.data_;
        }

        friend bool operator!=(RA_iterator const &a, RA_iterator const &b) {
            return a.pos != b.pos || a.data_ != b.data_;
        }

    private:
        size_t start_;
        size_t pos;
        size_t capacity_;
        pointer data_;
    };

public:
    using iterator = RA_iterator<T>;
    using const_iterator = RA_iterator<T const>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    my_deque() noexcept;
    explicit my_deque(size_t size);
    my_deque(size_t size, T const &value);
    my_deque(my_deque const &other);
    my_deque &operator=(my_deque const &other);
    ~my_deque();

    void resize(size_t new_size, T const &value);
    void reserve(size_t new_capacity);

    void push_back(T const &value);
    void push_front(T const &value);
    void pop_back();
    void pop_front();

    T &back() noexcept;
    T const &back() const noexcept;

    T &front() noexcept;
    T const &front() const noexcept;

    T &operator[](ptrdiff_t index) noexcept;
    T const &operator[](ptrdiff_t index) const noexcept;

    bool empty() const noexcept;
    size_t size() const noexcept;
    void clear() noexcept;

    iterator insert(const_iterator pos, T const &val);

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    iterator begin();
    iterator end();
    reverse_iterator rbegin();
    reverse_iterator rend();
    const_iterator begin() const;
    const_iterator end() const;
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;

    template<typename T1>
    friend void swap(my_deque<T1> &a, my_deque<T1> &b);

private:
    struct Deleter {
        void operator()(T *ptr) {
            operator delete(ptr);
        }
    };

    using storage_pointer = std::unique_ptr<T, Deleter>;

    void del_range_(iterator _begin, iterator _end) {
        for (auto it = _begin; it != _end; ++it) {
            it->~T();
        }
    }

    void fix_capacity() {
        if (size_ >= begin_.capacity_) {
            reserve(std::max(size_t(2), 2 * begin_.capacity_));
        } else if (size_ <= begin_.capacity_ / 4) {
            reserve(begin_.capacity_ / 2);
        }
    }

    storage_pointer data_;
    size_t size_;
    iterator begin_;
};

template<typename T>
my_deque<T>::my_deque() noexcept
        : data_(nullptr),
          size_(0),
          begin_(0, 0, 0, data_.get()) {}


template<typename T>
my_deque<T>::my_deque(size_t size) : my_deque(size, T()) {}

template<typename T>
my_deque<T>::my_deque(size_t size, const T &value) : my_deque() {
    resize(size, value);
}

template<typename T>
my_deque<T>::my_deque(my_deque const &other) : my_deque() {
    reserve(other.begin_.capacity_);
    std::uninitialized_copy(other.begin(), other.end(), data_.get());
    size_ = other.size_;
}

template<typename T>
my_deque<T> &my_deque<T>::operator=(my_deque const &other) {
    my_deque tmp(other);
    swap(tmp, *this);
    return *this;
}

template<typename T>
my_deque<T>::~my_deque() {
    clear();
}

template<typename T>
void my_deque<T>::resize(size_t new_size, const T &value) {
    if (new_size < size_) {
        del_range_(begin() + new_size, end());
        size_ = new_size;
    } else {
        auto old_end = end();
        reserve(new_size);
        size_ = new_size;
        std::uninitialized_fill(old_end, end(), value);
    }
}

template<typename T>
void my_deque<T>::reserve(size_t new_capacity) {
    if (new_capacity == 0){
        return;
    }
    storage_pointer new_data(static_cast<T*>(operator new(new_capacity * sizeof(T))));
    size_t move_count = std::min(size_, new_capacity);
    if (data_ != nullptr) {
        std::uninitialized_copy(begin(), begin() + move_count, new_data.get());
        del_range_(begin(), end());
    }
    data_.swap(new_data);
    begin_.data_ = data_.get();
    begin_.capacity_ = new_capacity;
    begin_.start_ = 0;
    begin_.pos = 0;
}

template<typename T>
void my_deque<T>::push_back(const T &value) {
    fix_capacity();
    new(&operator[](size_)) T(value);
    size_++;
}

template<typename T>
void my_deque<T>::push_front(const T &value) {
    fix_capacity();
    new(&operator[](-1)) T(value);
    --begin_;
    size_++;
}

template<typename T>
void my_deque<T>::pop_back() {
    del_range_(end() - 1, end());
    size_--;
    //fix_capacity();
}

template<typename T>
void my_deque<T>::pop_front() {
    del_range_(begin(), begin() + 1);
    size_--;
    ++begin_;
    //fix_capacity();
}

template<typename T>
T &my_deque<T>::back() noexcept {
    return operator[](size_ - 1);
}

template<typename T>
T const &my_deque<T>::back() const noexcept {
    return operator[](size_ - 1);
}

template<typename T>
T &my_deque<T>::front() noexcept {
    return *begin();
}

template<typename T>
T const &my_deque<T>::front() const noexcept {
    return *begin();
}

template<typename T>
T &my_deque<T>::operator[](ptrdiff_t index) noexcept {
    return begin()[index];
}

template<typename T>
T const &my_deque<T>::operator[](ptrdiff_t index) const noexcept {
    return begin()[index];
}

template<typename T>
bool my_deque<T>::empty() const noexcept {
    return size_ == 0;
}

template<typename T>
size_t my_deque<T>::size() const  noexcept {
    return size_;
}

template<typename T>
void my_deque<T>::clear() noexcept {
    if (size_ > 0) {
        resize(0, *begin());
    }
}

template<typename T>
typename my_deque<T>::iterator my_deque<T>::insert(my_deque::const_iterator pos, const T &val) {
    push_back(val);
    iterator res = begin_ + pos.get_index();
    for (iterator it = res; it != end(); ++it) {
        std::swap(*it, end()[-1]);
    }
    return res;
}

template<typename T>
typename my_deque<T>::iterator my_deque<T>::erase(my_deque::const_iterator pos) {
    return erase(pos, pos + 1);
}

template<typename T>
typename my_deque<T>::iterator my_deque<T>::erase(my_deque::const_iterator first, my_deque::const_iterator last) {
    auto res = last;
    ptrdiff_t range_size = last - first;
    iterator start = begin_ + first.get_index();
    iterator finish = begin_ + last.get_index();
    for (iterator it = start; it + range_size != end(); it++) {
        std::swap(*it, it[range_size]);
    }
    while (range_size-- > 0) {
        pop_back();
    }
    return start;
}

template<typename T>
typename my_deque<T>::iterator my_deque<T>::begin() {
    return begin_;
}

template<typename T>
typename my_deque<T>::iterator my_deque<T>::end() {
    return begin() + size_;
}

template<typename T>
typename my_deque<T>::reverse_iterator my_deque<T>::rbegin() {
    return my_deque::reverse_iterator(end());
}

template<typename T>
typename my_deque<T>::reverse_iterator my_deque<T>::rend() {
    return my_deque::reverse_iterator(begin());
}

template<typename T>
typename my_deque<T>::const_iterator my_deque<T>::begin() const {
    return my_deque::const_iterator(begin_);
}

template<typename T>
typename my_deque<T>::const_iterator my_deque<T>::end() const {
    return begin() + size_;
}

template<typename T>
typename my_deque<T>::const_reverse_iterator my_deque<T>::rbegin() const {
    return my_deque::const_reverse_iterator(end());
}

template<typename T>
typename my_deque<T>::const_reverse_iterator my_deque<T>::rend() const {
    return my_deque::const_reverse_iterator(begin());
}

template<typename T>
void swap(my_deque<T> &a, my_deque<T> &b) {
    std::swap(a.data_, b.data_);
    std::swap(a.size_, b.size_);
    std::swap(a.begin_, b.begin_);
}


#endif //EXAM_DEQUE_MY_DEQUE_H
