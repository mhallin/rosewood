#include "rosewood/data-format/object.h"

using rosewood::data_format::Object;

Object::Object() : type(DataType::Null) { }

Object::~Object() {
    switch (type) {
        case DataType::Null:
        case DataType::Boolean:
        case DataType::IntOrFloat:
        case DataType::Float:
            break;
        case DataType::String:
            string.~basic_string();
            break;
        case DataType::Array:
            array.~vector();
            break;
        case DataType::Dictionary:
            dictionary.~unordered_map();
            break;
    }
}

Object::Object(DataType data_type) : type(data_type) {
    switch (type) {
        case DataType::Null: break;
        case DataType::Boolean:
        case DataType::IntOrFloat:
        case DataType::Float:
            break;

        case DataType::String:
            new (&string) std::string();
            break;
        case DataType::Array:
            new (&array) ArrayType();
            break;
        case DataType::Dictionary:
            new (&dictionary) DictionaryType();
            break;
    }
}

Object::Object(const Object &other) {
    type = other.type;

    switch (type) {
        case DataType::Null: break;
        case DataType::Boolean:
            boolean = other.boolean;
            break;
        case DataType::IntOrFloat:
        case DataType::Float:
            int64 = other.int64;
            float64 = other.float64;
            break;
        case DataType::String:
            new (&string) std::string(other.string);
            break;
        case DataType::Array:
            new (&array) ArrayType(other.array);
            break;
        case DataType::Dictionary:
            new (&dictionary) DictionaryType(other.dictionary);
            break;
    };
}

Object &Object::operator=(const Object &other) {
    this->~Object();

    type = other.type;
    switch (type) {
        case DataType::Null: break;
        case DataType::Boolean:
            boolean = other.boolean;
            break;
        case DataType::IntOrFloat:
        case DataType::Float:
            int64 = other.int64;
            float64 = other.float64;
            break;
        case DataType::String:
            new (&string) std::string(other.string);
            break;
        case DataType::Array:
            new (&array) ArrayType(other.array);
            break;
        case DataType::Dictionary:
            new (&dictionary) DictionaryType(other.dictionary);
            break;
    }

    return *this;
}

Object Object::make_null() { return Object(DataType::Null); }

Object Object::make_boolean(bool boolean) {
    Object result(DataType::Boolean);
    result.boolean = boolean;
    return result;
}

Object Object::make_int(long long int64) {
    Object result(DataType::IntOrFloat);
    result.int64 = int64;
    result.float64 = int64;
    return result;
}

Object Object::make_float(double float64) {
    Object result(DataType::Float);
    result.float64 = float64;
    return result;
}

Object Object::make_string(const std::string &string) {
    Object result(DataType::String);
    result.string = string;
    return result;
}

Object Object::make_array(const ArrayType &array) {
    Object result(DataType::Array);
    result.array = array;
    return result;
}

Object Object::make_dictionary(const DictionaryType &dictionary) {
    Object result(DataType::Dictionary);
    result.dictionary = dictionary;
    return result;
}
