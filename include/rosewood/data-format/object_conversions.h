#ifndef __ROSEWOOD_DATA_FORMAT_OBJECT_CONVERSIONS_H__
#define __ROSEWOOD_DATA_FORMAT_OBJECT_CONVERSIONS_H__

#include "rosewood/data-format/object.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"

namespace rosewood { namespace data_format {

    void convert(const Object &object, bool *result) {
        assert(object.type == DataType::Boolean);
        *result = object.boolean;
    }

    void convert(const Object &object, int *result) {
        assert(object.type == DataType::IntOrFloat);
        *result = (int)object.int64;
    }

    void convert(const Object &object, unsigned int *result) {
        assert(object.type == DataType::IntOrFloat);
        *result = (unsigned int)object.int64;
    }

    void convert(const Object &object, long *result) {
        assert(object.type == DataType::IntOrFloat);
        *result = (long)object.int64;
    }

    void convert(const Object &object, unsigned long *result) {
        assert(object.type == DataType::IntOrFloat);
        *result = (unsigned long)object.int64;
    }

    void convert(const Object &object, long long *result) {
        assert(object.type == DataType::IntOrFloat);
        *result = object.int64;
    }

    void convert(const Object &object, float *result) {
        assert(object.type == DataType::IntOrFloat || object.type == DataType::Float);
        *result = (float)object.float64;
    }

    void convert(const Object &object, double *result) {
        assert(object.type == DataType::IntOrFloat || object.type == DataType::Float);
        *result = object.float64;
    }

    template<typename T>
    void convert(const Object &object, std::vector<T> *result) {
        assert(object.type == DataType::Array);

        result->reserve(object.array.size());
        for (const auto &elem : object.array) {
            result->emplace_back(as<T>(elem));
        }
    }

    template<typename T>
    void convert(const Object &object, std::unordered_map<std::string, T> *result) {
        assert(object.type == DataType::Dictionary);

        result->reserve(object.dictionary.size());
        for (const auto &pair : object.dictionary) {
            (*result)[pair.first] = as<T>(pair.second);
        }
    }

} }

#pragma clang diagnostic pop

#endif
