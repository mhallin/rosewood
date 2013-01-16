#ifndef __ROSEWOOD_CORE_COMPONENT_ARRAY_H__
#define __ROSEWOOD_CORE_COMPONENT_ARRAY_H__

#include <vector>
#include <functional>
#include <type_traits>

#include "rosewood/data-structures/stable_vector.h"

namespace rosewood { namespace core {
    
    class ComponentArray {
    public:
        typedef unsigned int flag_type;
        
        template<typename TComp, typename... TArgs>
        TComp *create(size_t index, TArgs... args);
        
        template<typename TComp>
        TComp *at(size_t index);
        
        template<typename TComp>
        void remove(size_t index);
        
        void remove_dynamic(size_t index, unsigned int type_code);
        
        template<typename TComp>
        size_t size() const;
        
    private:
        data_structures::StableVector<unsigned char> _data;
        
        static std::vector<std::function<void(void*)>> _destructors;
        static std::vector<size_t> _strides;
        static std::vector<size_t> _flag_offsets;
        static std::vector<size_t> _data_offsets;
        
        template<typename TComp>
        flag_type *flag_ptr(size_t index);
        
        template<typename TComp>
        TComp *component_ptr(size_t index);
        
        template<typename TComp>
        static size_t flag_index(size_t index);
        
        template<typename TComp>
        static size_t data_index(size_t index);
        
        template<typename TComp>
        static size_t stride();
        
        template<typename TComp>
        static size_t alignment();
        
        template<typename TComp>
        static void register_component();
    };
    
    template<typename TComp>
    class ComponentArrayIterator {
    public:
        ComponentArrayIterator(ComponentArray *component_array, size_t index, int initial_step);
        
        bool operator==(const ComponentArrayIterator<TComp> &rhs) const;
        bool operator!=(const ComponentArrayIterator<TComp> &rhs) const;
        
        ComponentArrayIterator<TComp> &operator++();
        ComponentArrayIterator<TComp> operator++(int);
        
        TComp *operator*();
        const TComp *operator*() const;
        
        void step_to_valid(int step);
        
    private:
        ComponentArray *_component_array;
        size_t _index;
    };
    
    template<typename TComp>
    class ComponentArrayView {
    public:
        ComponentArrayView(ComponentArray *component_array);
        
        ComponentArrayIterator<TComp> begin();
        ComponentArrayIterator<const TComp> begin() const;
        
        ComponentArrayIterator<TComp> end();
        ComponentArrayIterator<const TComp> end() const;
        
    private:
        ComponentArray *_component_array;
    };
        
    
    template<typename TComp, typename... TArgs>
    TComp *ComponentArray::create(size_t index, TArgs... args) {
        if (!_data.capacity()) {
            _data.set_slice_size(stride<TComp>() * 32);
            _data.set_slice_data_alignment(alignment<TComp>());
        }
        register_component<TComp>();
        _data.resize((index + 1) * stride<TComp>(), 0);
        *flag_ptr<TComp>(index) = 1;
        TComp *ptr = component_ptr<TComp>(index);
        new(ptr) TComp(std::forward<TArgs>(args)...);
        return ptr;
    }
    
    template<typename TComp>
    TComp *ComponentArray::at(size_t index) {
        if (!*flag_ptr<TComp>(index)) {
            return nullptr;
        }
        return component_ptr<TComp>(index);
    }
    
    template<typename TComp>
    void ComponentArray::remove(size_t index) {
        if (!*flag_ptr<TComp>(index)) {
            return;
        }
        *flag_ptr<TComp>(index) = 0;
        TComp *ptr = component_ptr<TComp>(index);
        ptr->~TComp();
    }
    
    
    template<typename TComp>
    ComponentArray::flag_type *ComponentArray::flag_ptr(size_t index) {
        auto f_index = flag_index<TComp>(index);
        return reinterpret_cast<flag_type*>(&_data[f_index]);
    }
    
    template<typename TComp>
    TComp *ComponentArray::component_ptr(size_t index) {
        auto d_index = data_index<TComp>(index);
        return reinterpret_cast<TComp*>(&_data[d_index]);
    }
    
    template<typename TComp>
    size_t ComponentArray::size() const {
        return _data.size() / stride<TComp>();
    }
    
    template<typename TComp>
    size_t ComponentArray::flag_index(size_t index) {
        struct TCompData { flag_type flag; TComp comp; };
        return index * stride<TComp>() + size_t(&(((TCompData*)nullptr)->flag));
    }
    
    template<typename TComp>
    size_t ComponentArray::data_index(size_t index) {
        struct TCompData { flag_type flag; TComp comp; };
        return index * stride<TComp>() + size_t(&(((TCompData*)nullptr)->comp));
    }
    
    template<typename TComp>
    size_t ComponentArray::stride() {
        struct TCompData { flag_type flag; TComp comp; };
        return sizeof(TCompData);
    }
    
    template<typename TComp>
    size_t ComponentArray::alignment() {
        struct TCompData { flag_type flag; TComp comp; };
        return std::alignment_of<TCompData>::value;
    }
    
    template<typename TComp>
    void ComponentArray::register_component() {
        auto index = TComp::register_type();
        if (index >= _destructors.size()) {
            _destructors.resize(index + 1);
            _flag_offsets.resize(index + 1);
            _data_offsets.resize(index + 1);
            _strides.resize(index + 1);
        }
        if (!_destructors[index]) {
            _destructors[index] = [](void *ptr){ static_cast<TComp*>(ptr)->~TComp(); };
            _strides[index] = stride<TComp>();
            _flag_offsets[index] = flag_index<TComp>(0);
            _data_offsets[index] = data_index<TComp>(0);
        }
    }
    
    template<typename TComp>
    ComponentArrayIterator<TComp>::ComponentArrayIterator(ComponentArray *component_array,
                                                          size_t index, int initial_step)
    : _component_array(component_array), _index(index)
    {
        if (initial_step) {
            step_to_valid(initial_step);
        }
    }
    
    template<typename TComp>
    bool ComponentArrayIterator<TComp>::operator==(const ComponentArrayIterator<TComp> &rhs) const {
        return _index == rhs._index && _component_array == rhs._component_array;
    }
    
    template<typename TComp>
    bool ComponentArrayIterator<TComp>::operator!=(const ComponentArrayIterator<TComp> &rhs) const {
        return !(*this == rhs);
    }
    
    template<typename TComp>
    ComponentArrayIterator<TComp> &ComponentArrayIterator<TComp>::operator++() {
        ++_index;
        step_to_valid(1);
        return *this;
    }
    
    template<typename TComp>
    ComponentArrayIterator<TComp> ComponentArrayIterator<TComp>::operator++(int) {
        ComponentArrayIterator<TComp> copy(*this);
        ++_index;
        step_to_valid(1);
        return copy;
    }
    
    template<typename TComp>
    TComp *ComponentArrayIterator<TComp>::operator*() {
        return _component_array->at<TComp>(_index);
    }
    
    template<typename TComp>
    const TComp *ComponentArrayIterator<TComp>::operator*() const {
        return _component_array->at<TComp>(_index);
    }
    
    template<typename TComp>
    void ComponentArrayIterator<TComp>::step_to_valid(int step) {
        while (_index < _component_array->size<TComp>() && !_component_array->at<TComp>(_index)) {
            _index += step;
        }
    }
    
    template<typename TComp>
    ComponentArrayView<TComp>::ComponentArrayView(ComponentArray *component_array)
    : _component_array(component_array) { }
    
    template<typename TComp>
    ComponentArrayIterator<TComp> ComponentArrayView<TComp>::begin() {
        return ComponentArrayIterator<TComp>(_component_array, 0, 1);
    }
    
    template<typename TComp>
    ComponentArrayIterator<TComp> ComponentArrayView<TComp>::end() {
        return ComponentArrayIterator<TComp>(_component_array, _component_array->size<TComp>(), 0);
    }

} }

#endif
