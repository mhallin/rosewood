#ifndef __ROSEWOOD_CORE_COMPONENT_ARRAY_H__
#define __ROSEWOOD_CORE_COMPONENT_ARRAY_H__

#include <vector>
#include <functional>
#include <type_traits>

#include "rosewood/data-structures/stable_vector.h"

namespace rosewood { namespace core {
    
    class ComponentArray {
        typedef unsigned int flag_type;
    
        template<typename TComp>
        struct CompData {
            TComp data;
            flag_type flag;
        };

    public:
        
        template<typename TComp, typename... TArgs> TComp *create(size_t index, TArgs... args) {
            if (!_data.capacity()) {
                _data.set_slice_size(stride<TComp>() * 32);
                _data.set_slice_data_alignment(std::alignment_of<CompData<TComp>>::value);
            }
            register_component<TComp>();
            _data.resize((index + 1) * stride<TComp>(), 0);

            auto data = comp_data<TComp>(index);
            data->flag = 1;
            return new(&data->data) TComp(std::forward<TArgs>(args)...);
        }
        
        template<typename TComp> TComp *at(size_t index) {
            auto data = comp_data<TComp>(index);
            return data->flag ? &data->data : nullptr;
        }
        
        template<typename TComp> void remove(size_t index) {
            auto data = comp_data<TComp>(index);
            if (data->flag) {
                data->flag = 0;
                data->data.~TComp();
            }
        }
        
        void remove_dynamic(size_t index, unsigned int type_code);
        
        template<typename TComp>
        size_t size() const {
            return _data.size() / stride<TComp>();
        }
        
    private:
        data_structures::StableVector<unsigned char> _data;
        
        static std::vector<std::function<void(void*)>> _destructors;
        static std::vector<size_t> _sizes;
        static std::vector<size_t> _flag_offsets;
        static std::vector<size_t> _data_offsets;
        
        template<typename TComp>
        CompData<TComp> *comp_data(size_t index) {
            auto ptr = &_data[index * stride<TComp>()];
            return reinterpret_cast<CompData<TComp>*>(ptr);
        }
        
        template<typename TComp>
        static size_t stride() {
            auto size = sizeof(CompData<TComp>);
            auto alignment = std::alignment_of<CompData<TComp>>::value;
            if (size % alignment != 0) {
                size += alignment - (size % alignment);
            }
            return size;
        }
        
        template<typename TComp>
        static void register_component();
    };
    
    template<typename TComp>
    class ComponentArrayIterator {
    public:
        ComponentArrayIterator(ComponentArray *component_array, size_t index, int initial_step)
        : _component_array(component_array), _index(index) {
            if (initial_step) {
                step_to_valid(initial_step);
            }
        }
        
        bool operator==(const ComponentArrayIterator<TComp> &rhs) const {
            return _index == rhs._index && _component_array == rhs._component_array;
        }
        
        bool operator!=(const ComponentArrayIterator<TComp> &rhs) const {
            return !(*this == rhs);
        }
        
        ComponentArrayIterator<TComp> &operator++() {
            ++_index;
            step_to_valid(1);
            return *this;
        }
        ComponentArrayIterator<TComp> operator++(int) {
            ComponentArrayIterator<TComp> copy(*this);
            ++*this;
            return copy;
        }
        
        TComp *operator*() {
            return _component_array->at<TComp>(_index);
        }

        const TComp *operator*() const {
            return _component_array->at<TComp>(_index);
        }
        
        void step_to_valid(int step) {
            while (_index < _component_array->size<TComp>() && !_component_array->at<TComp>(_index)) {
                _index += step;
            }
        }
        
    private:
        ComponentArray *_component_array;
        size_t _index;
    };
    
    template<typename TComp>
    class ComponentArrayView {
    public:
        ComponentArrayView(ComponentArray *component_array) : _component_array(component_array) { }
        
        ComponentArrayIterator<TComp> begin() {
            return ComponentArrayIterator<TComp>(_component_array, 0, 1);
        }
        
        ComponentArrayIterator<TComp> end() {
            return ComponentArrayIterator<TComp>(_component_array,
                                                 _component_array->size<TComp>(), 0);
        }
        
    private:
        ComponentArray *_component_array;
    };
    
    template<typename TComp>
    void ComponentArray::register_component() {
        auto index = TComp::register_type();
        if (index >= _destructors.size()) {
            _destructors.resize(index + 1);
            _flag_offsets.resize(index + 1);
            _data_offsets.resize(index + 1);
            _sizes.resize(index + 1);
        }
        if (!_destructors[index]) {
            _destructors[index] = [](void *ptr){ static_cast<TComp*>(ptr)->~TComp(); };
            _sizes[index] = stride<TComp>();
            _flag_offsets[index] = (size_t)&(((CompData<TComp>*)nullptr)->flag);
            _data_offsets[index] = (size_t)&(((CompData<TComp>*)nullptr)->data);
        }
    }

} }

#endif
