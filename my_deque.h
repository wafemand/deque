//
// Created by andrey on 24.06.18.
//

#ifndef EXAM_DEQUE_MY_DEQUE_H
#define EXAM_DEQUE_MY_DEQUE_H


#include <cstddef>
#include <iterator>
#include <vector>
#include <memory>


template<typename T>
class my_deque {

    template<typename _Tp, typename _Deque_pointer>
    struct RA_iterator {
    public:
        typedef std::random_access_iterator_tag iterator_category;
        typedef _Tp value_type;
        typedef ptrdiff_t difference_type;
        typedef _Tp *pointer;
        typedef _Tp &reference;

        friend class my_deque;

    private:
        RA_iterator(size_t pos, _Deque_pointer base_deque)
                : pos(pos),
                  base_deque(base_deque) {}

    public:
        RA_iterator(RA_iterator const &other) = default;

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
            return (*base_deque)[pos];
        }

        pointer operator->() {
            return &(*base_deque)[pos];
        }

        bool operator==(RA_iterator const &other) {
            return other.pos == pos && other.base_deque == base_deque;
        }

        bool operator!=(RA_iterator const &other) {
            return other.pos != pos || other.base_deque != base_deque;
        }

        RA_iterator operator+(difference_type diff) {
            return RA_iterator(pos + diff, base_deque);
        }

        RA_iterator operator-(difference_type diff) {
            return RA_iterator(pos - diff, base_deque);
        }

        difference_type operator-(RA_iterator const &other) {
            return difference_type(pos - other.pos);
        }

        reference operator[](difference_type diff) {
            return *RA_iterator(*this + diff);
        }

    private:
        size_t pos;
        _Deque_pointer base_deque;
    };


    struct MallocDeleter {
        void operator()(T *ptr) {
            free(ptr);
        }
    };

public:
    using iterator = RA_iterator<T, my_deque<T> *>;
    using const_iterator = RA_iterator<T const, my_deque<T> const *>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    my_deque() noexcept;
    explicit my_deque(size_t size);
    my_deque(size_t size, T const &value);
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

    T &operator[](size_t index) noexcept;
    T const &operator[](size_t index) const noexcept;

    bool empty() noexcept;
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

private:

    void del_range_(iterator _begin, iterator _end) {
        for (auto it = _begin; it != _end; ++it) {
            it->~T();
        }
    }

    void fix_capacity() {
        if (size_ == capacity_) {
            reserve(2 * capacity_);
        } else if (size_ <= capacity_ / 4) {
            reserve(capacity_ / 2);
        }
    }

    std::unique_ptr<T, MallocDeleter> data_;
    size_t size_;
    size_t capacity_;
    size_t start_;
};

template<typename T>
my_deque<T>::my_deque() noexcept
        : data_(nullptr),
          size_(0),
          capacity_(0),
          start_(0) {}


template<typename T>
my_deque<T>::my_deque(size_t size) : my_deque(size, T()) {}

template<typename T>
my_deque<T>::my_deque(size_t size, const T &value) : my_deque() {
    resize(size, value);
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
    auto new_data = static_cast<T *>(malloc(new_capacity * sizeof(T)));
    size_t move_count = std::min(capacity_, new_capacity);
    std::move(begin(), begin() + move_count, new_data);
    data_.reset(new_data);
    capacity_ = new_capacity;
    start_ = 0;
}

template<typename T>
void my_deque<T>::push_back(const T &value) {
    new (&operator[](size_)) T(value);
    size_++;
    fix_capacity();
}

template<typename T>
void my_deque<T>::push_front(const T &value) {
    start_--;
    new (&operator[](0)) T(value);
    size_++;
    fix_capacity();
}

template<typename T>
void my_deque<T>::pop_back() {
    del_range_(end() - 1, end());
    size_--;
}

template<typename T>
void my_deque<T>::pop_front() {
    del_range_(begin(), begin() + 1);
    size_--;
    start_++;
}

template<typename T>
T &my_deque<T>::back() noexcept {
    return operator[](size_ - 1);
}

template<typename T>
T const &my_deque<T>::back() const noexcept {
    return operator[](size_);
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
T &my_deque<T>::operator[](size_t index) noexcept {
    return data_.get()[(start_ + index) % capacity_];
}

template<typename T>
T const &my_deque<T>::operator[](size_t index) const noexcept {
    return data_.get()[(start_ + index) % capacity_];
}

template<typename T>
bool my_deque<T>::empty() noexcept {
    return size_ == 0;
}

template<typename T>
void my_deque<T>::clear() noexcept {
    del_range_(begin(), end());
    size_ = 0;
    start_ = 0;
}

template<typename T>
typename my_deque<T>::iterator my_deque<T>::insert(my_deque::const_iterator pos, const T &val) {
    push_back(val);
    iterator res = iterator(pos.pos, this);
    for (iterator it = res; it != end(); ++it) {
        std::swap(*it, *end()[-1]);
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
    iterator start = iterator(first.pos, this);
    iterator finish = iterator(last.pos, this);
    for (iterator it = start; it + range_size != end(); it++) {
        std::swap(*it, it[range_size]);
    }
    while (range_size-- > 0) {
        pop_back();
    }
    return finish;
}

template<typename T>
typename my_deque<T>::iterator my_deque<T>::begin() {
    return my_deque::iterator(0, this);
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
    return my_deque::const_iterator(0, this);
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


#endif //EXAM_DEQUE_MY_DEQUE_H
