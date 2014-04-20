#include "rosewood/core/component_array.h"

using rosewood::core::ComponentArray;

std::vector<std::function<void(void*)>> ComponentArray::_destructors;
std::vector<size_t> ComponentArray::_sizes;
std::vector<size_t> ComponentArray::_flag_offsets;
std::vector<size_t> ComponentArray::_data_offsets;

void ComponentArray::remove_dynamic(size_t index, unsigned int type_code) {
    auto assumed_size = _sizes[type_code];
    auto assumed_flag_index = index * assumed_size + _flag_offsets[type_code];
    auto assumed_data_index = index * assumed_size + _data_offsets[type_code];
    
    if (assumed_flag_index >= _data.size()) {
        return;
    }
    
    flag_type *flag_ptr = reinterpret_cast<flag_type*>(&_data[assumed_flag_index]);
    if (!*flag_ptr) {
        return;
    }
    
    *flag_ptr = 0;
    void *data_ptr = &_data[assumed_data_index];
    _destructors[type_code](data_ptr);
}
