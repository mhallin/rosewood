#ifndef __ROSEWOOD_DATA_STRUCTURES_STABLE_VECTOR_H__
#define __ROSEWOOD_DATA_STRUCTURES_STABLE_VECTOR_H__

#include <stdlib.h>

#include <vector>

namespace rosewood { namespace data_structures {
    
    template<typename T> class StableVector;
    
    template<typename T>
    class StableVectorIterator {
    public:
        StableVectorIterator(size_t index, StableVector<T> *_owner);

        bool operator==(const StableVectorIterator<T> &rhs) const;
        bool operator!=(const StableVectorIterator<T> &rhs) const;
        
        StableVectorIterator<T> &operator++();
        StableVectorIterator<T> operator++(int);
        
        T &operator*();
        const T&operator *() const;

    private:
        size_t _index;
        StableVector<T> *_owner;
    };
    
    template<typename T>
    class StableVector {
    public:
        explicit StableVector(size_t slice_size = 32, size_t slice_data_alignment = 8);
        ~StableVector();
        
        bool empty() const;
        size_t size() const;
        
        size_t capacity() const;
        
        void set_slice_size(size_t size); // can only be called wher capacity() == 0
        void set_slice_data_alignment(size_t alignment); // will only affect new slices
        
        void push_back(T &&v);
        void push_back(const T &v);
        
        T &operator[](size_t index);
        const T &operator[](size_t index) const;

        void resize(size_t new_size);
        void resize(size_t new_size, const T &fill_value);
        
        StableVectorIterator<T> begin();
        StableVectorIterator<const T> begin() const;
        
        StableVectorIterator<T> end();
        StableVectorIterator<const T> end() const;
        
        StableVector(const StableVector &other) = delete;
        StableVector &operator=(const StableVector &other) = delete;
        
    private:
        std::vector<T*> _element_arrays;
        size_t _slice_size;
        size_t _total_size;
        size_t _slice_data_alignment;
        
        void ensure_valid_index(size_t index);
        
        size_t slice_index(size_t index) const;
        size_t element_index(size_t index) const;
        
        void free_slice(T *slice) const;
    };
    
    template<typename T>
    StableVector<T>::StableVector(size_t slice_size, size_t slice_data_alignment)
    : _slice_size(slice_size), _total_size(0), _slice_data_alignment(slice_data_alignment) { }
    
    template<typename T>
    StableVector<T>::~StableVector() {
        for (T *slice : _element_arrays) {
            free_slice(slice);
        }
    }
    
    template<typename T>
    bool StableVector<T>::empty() const { return size() == 0; }
    
    template<typename T>
    size_t StableVector<T>::size() const { return _total_size; }
    
    template<typename T>
    size_t StableVector<T>::capacity() const { return _slice_size * _element_arrays.size(); }
    
    template<typename T>
    void StableVector<T>::set_slice_size(size_t slice_size) {
        if (capacity()) {
            throw std::range_error("Can not change slice size when capacity() != 0");
        }
        _slice_size = slice_size;
    }
    
    template<typename T>
    void StableVector<T>::set_slice_data_alignment(size_t slice_data_alignment) {
        _slice_data_alignment = slice_data_alignment;
    }
    
    template<typename T>
    void StableVector<T>::push_back(T &&v) {
        ensure_valid_index(_total_size);
        (*this)[_total_size-1] = std::forward<T>(v);
    }
    
    template<typename T>
    void StableVector<T>::push_back(const T &v) {
        ensure_valid_index(_total_size);
        (*this)[_total_size-1] = v;
    }
    
    template<typename T>
    T &StableVector<T>::operator[](size_t index) {
        return _element_arrays[slice_index(index)][element_index(index)];
    }
    
    template<typename T>
    const T &StableVector<T>::operator[](size_t index) const {
        return _element_arrays[slice_index(index)][element_index(index)];
    }
    
    template<typename T>
    void StableVector<T>::ensure_valid_index(size_t index) {
        if (index >= _total_size) {
            resize(index + 1);
        }
    }
    
    template<typename T>
    void StableVector<T>::resize(size_t new_size) {
        auto required_slices = slice_index(new_size - 1) + 1;
        if (new_size > size()) {
            while (_element_arrays.size() < required_slices) {
                void *data;
                if (posix_memalign(&data, _slice_data_alignment, _slice_size * sizeof(T))) {
                    throw std::bad_alloc();
                }
                T *ptr = new(data) T[_slice_size];
                _element_arrays.push_back(ptr);
            }
            _total_size = new_size;
        }
        else {
            while (_element_arrays.size() > required_slices) {
                T *slice = _element_arrays.back();
                free_slice(slice);
                _element_arrays.pop_back();
            }
            _total_size = new_size;
        }
    }
    
    template<typename T>
    void StableVector<T>::resize(size_t new_size, const T &fill_value) {
        if (new_size < size()) {
            resize(new_size);
        }
        else {
            auto required_slices = slice_index(new_size - 1) + 1;
            while (_element_arrays.size() < required_slices) {
                _element_arrays.push_back(new T[_slice_size]);
                for (size_t e = 0; e < _slice_size; ++e) {
                    _element_arrays.back()[e] = fill_value;
                }
            }
            _total_size = new_size;
        }
    }
    
    template<typename T>
    StableVectorIterator<T> StableVector<T>::begin() {
        return StableVectorIterator<T>(0, this);
    }
    
    template<typename T>
    StableVectorIterator<const T> StableVector<T>::begin() const {
        return StableVectorIterator<const T>(0, this);
    }
    
    template<typename T>
    StableVectorIterator<T> StableVector<T>::end() {
        return StableVectorIterator<T>(_total_size, this);
    }
    
    template<typename T>
    StableVectorIterator<const T> StableVector<T>::end() const {
        return StableVectorIterator<T>(_total_size, this);
    }
    
    template<typename T>
    size_t StableVector<T>::slice_index(size_t index) const {
        return index / _slice_size;
    }
    
    template<typename T>
    size_t StableVector<T>::element_index(size_t index) const {
        return index % _slice_size;
    }
    
    template<typename T>
    void StableVector<T>::free_slice(T *slice) const {
        for (size_t i = 0; i < _slice_size; ++i) {
            slice[i].~T();
        }
        free(slice);
    }

    template<typename T>
    StableVectorIterator<T>::StableVectorIterator(size_t index, StableVector<T> *owner)
    : _index(index)
    , _owner(owner) { }
    
    template<typename T>
    bool StableVectorIterator<T>::operator==(const StableVectorIterator<T> &rhs) const {
        return _index == rhs._index && _owner == rhs._owner;
    }
    
    template<typename T>
    bool StableVectorIterator<T>::operator!=(const StableVectorIterator<T> &rhs) const {
        return !(*this == rhs);
    }
    
    template<typename T>
    StableVectorIterator<T> &StableVectorIterator<T>::operator++() {
        ++_index;
        return *this;
    }
    
    template<typename T>
    StableVectorIterator<T> StableVectorIterator<T>::operator++(int) {
        return StableVectorIterator<T>(_index++, _owner);
    }
    
    template<typename T>
    T &StableVectorIterator<T>::operator*() {
        return (*_owner)[_index];
    }
    
    template<typename T>
    const T &StableVectorIterator<T>::operator *() const {
        return (*_owner)[_index];
    }

} }

#endif
