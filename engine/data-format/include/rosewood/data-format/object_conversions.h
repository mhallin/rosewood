#ifndef __ROSEWOOD_DATA_FORMAT_OBJECT_CONVERSIONS_H__
#define __ROSEWOOD_DATA_FORMAT_OBJECT_CONVERSIONS_H__

#include "rosewood/data-format/object.h"

namespace rosewood { namespace data_format {

    inline void convert(const Object &object, bool *result) {
        RW_ASSERT(object.type == DataType::Boolean, "Can't convert non-boolean to boolean");
        *result = object.boolean;
    }

    inline void convert(const Object &object, int *result) {
        RW_ASSERT(object.type == DataType::IntOrFloat, "Can't convert non-integral to int");
        *result = (int)object.int64;
    }

    inline void convert(const Object &object, unsigned int *result) {
        RW_ASSERT(object.type == DataType::IntOrFloat, "Can't convert non-integral to unsigned int");
        *result = (unsigned int)object.int64;
    }

    inline void convert(const Object &object, long *result) {
        RW_ASSERT(object.type == DataType::IntOrFloat, "Can't convert non-integral to long");
        *result = (long)object.int64;
    }

    inline void convert(const Object &object, unsigned long *result) {
        RW_ASSERT(object.type == DataType::IntOrFloat, "Can't convert non-integral to unsigned long");
        *result = (unsigned long)object.int64;
    }

    inline void convert(const Object &object, long long *result) {
        RW_ASSERT(object.type == DataType::IntOrFloat, "Can't convert non-integral to long long");
        *result = object.int64;
    }

    inline void convert(const Object &object, float *result) {
        RW_ASSERT(object.type == DataType::IntOrFloat || object.type == DataType::Float,
                  "Can't convert non-floating point to float");
        *result = (float)object.float64;
    }

    inline void convert(const Object &object, double *result) {
        RW_ASSERT(object.type == DataType::IntOrFloat || object.type == DataType::Float,
                  "Can't convert non-floating ponit to double");
        *result = object.float64;
    }

    inline void convert(const Object &object, std::string *result) {
        RW_ASSERT(object.type == DataType::String, "Can't convert non-string to string");
        *result = object.string;
    }

    template<typename T>
    void convert(const Object &object, std::vector<T> *result) {
        RW_ASSERT(object.type == DataType::Array, "Can't convert non-array to std::vector");

        result->reserve(object.array.size());
        for (const auto &elem : object.array) {
            result->emplace_back(as<T>(elem));
        }
    }

    template<typename T>
    void convert(const Object &object, std::unordered_map<std::string, T> *result) {
        RW_ASSERT(object.type == DataType::Dictionary, "Can't convert non-object to std::unordered_map");

        result->reserve(object.dictionary.size());
        for (const auto &pair : object.dictionary) {
            (*result)[pair.first] = as<T>(pair.second);
        }
    }

} }

#endif
