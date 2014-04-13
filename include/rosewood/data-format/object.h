#ifndef __ROSEWOOD_DATA_FORMAT_OBJECT_H__
#define __ROSEWOOD_DATA_FORMAT_OBJECT_H__

#include <assert.h>

#include <string>
#include <vector>
#include <unordered_map>

namespace rosewood { namespace data_format {

    enum class DataType {
        Null,
        Boolean,
        IntOrFloat,
        Float,
        String,
        Array,
        Dictionary,
    };

    struct Object;

    typedef std::vector<Object> ArrayType;
    typedef std::unordered_map<std::string, Object> DictionaryType;

    template<typename T> T as(const Object &object);
    template<typename T> void convert(const Object &object, T *result);

    struct Object {
        // Constructing functions
        static Object make_null();
        static Object make_boolean(bool boolean);
        static Object make_int(long long int64);
        static Object make_float(double float64);
        static Object make_string(const std::string &string);
        static Object make_array(const ArrayType &array);
        static Object make_dictionary(const DictionaryType &dictionary);

        // Data
        union {
            bool boolean;
            struct {
                long long int64;
                double float64;
            };
            std::string string;
            ArrayType array;
            DictionaryType dictionary;
        };

        DataType type;
        
        // Constructors, assignment operators and destructors
        Object();
        Object(const Object &other);
        Object(DataType data_type);
        Object &operator=(const Object &other);
        ~Object();

        // Array access
        Object &operator[](size_t index) {
            assert(type == DataType::Array);
            return array[index];
        }

        const Object &operator[](size_t index) const {
            assert(type == DataType::Array);
            return array[index];
        }

        // Dictionary access
        Object &operator[](const std::string &key) {
            assert(type == DataType::Dictionary);
            return dictionary[key];
        }

        const Object &operator[](const std::string &key) const {
            assert(type == DataType::Dictionary);
            return dictionary.at(key);
        }
    };

    template<typename T> T as(const Object &object) {
        T result;
        convert(object, &result);
        return result;
    }

    inline bool is_null(const Object &object) {
        return object.type == DataType::Null;
    }

} }

#endif
