#include "rosewood/data-format/reader.h"

#include <assert.h>
#include <string.h>
#include <arpa/inet.h>

#include "rosewood/data-format/object.h"

#if defined(__APPLE__)

# include <libkern/OSByteOrder.h>
# define ntohl64(x) OSSwapHostToBigInt64(x)

#elif defined(EMSCRIPTEN)

# include <endian.h>
# define ntohl64(x) betoh64(x)

#else

# error Unknown platform - please create 64 bit byte swapping primitives

#endif

using rosewood::data_format::ArrayType;
using rosewood::data_format::DataType;
using rosewood::data_format::DictionaryType;
using rosewood::data_format::Object;


static Object read_from(const std::string &source, size_t from, size_t *out_end);


static int ntoh(int x) { return ntohl(x); }
static unsigned int ntoh(unsigned int x) { return ntohl(x); }
static long long ntoh(long long x) { return ntohl64(x); }
static unsigned long long ntoh(unsigned long long x) { return ntohl64(x); }


template<typename T>
T read_number(const std::string &source, size_t from, size_t *out_end) {
    T value;

    memcpy(&value, &source[from], sizeof(value));

    if (out_end) *out_end = from + sizeof(value);
    return ntoh(value);
}


template<typename T>
std::string read_string(const std::string &source, size_t from, size_t *out_end) {
    auto length = (size_t)read_number<T>(source, from, &from);
    std::string string(&source[from], length);

    if (out_end) *out_end = from + length;
    return string;
}


template <typename T>
ArrayType read_array(const std::string &source, size_t from, size_t *out_end) {
    auto length = (size_t)read_number<T>(source, from, &from);
    ArrayType array;
    array.reserve(length);

    for (T i = 0; i < length; ++i) {
        array.emplace_back(read_from(source, from, &from));
    }

    if (out_end) *out_end = from;
    return array;
}


template<typename T>
DictionaryType read_dictionary(const std::string &source, size_t from, size_t *out_end) {
    auto length = (size_t)read_number<T>(source, from, &from);
    DictionaryType dictionary;
    dictionary.reserve(length);

    for (T i = 0; i < length; ++i) {
        auto key = read_from(source, from, &from);
        assert(key.type == DataType::String);
        assert(dictionary.count(key.string) == 0);

        auto value = read_from(source, from, &from);
        dictionary[key.string] = value;
    }

    if (out_end) *out_end = from;
    return dictionary;
}


static Object read_from(const std::string &source, size_t from, size_t *out_end) {
    assert(from < source.size());

    switch (source[from]) {
        case 'n':
            if (out_end) *out_end = from + 1;
            return Object::make_null();
            
        case 'f':
            if (out_end) *out_end = from + 1;
            return Object::make_boolean(false);
            
        case 't':
            if (out_end) *out_end = from + 1;
            return Object::make_boolean(true);
            
        case 'i':
            return Object::make_int(read_number<int>(source, from + 1, out_end));

        case 'I':
            return Object::make_int(read_number<long long>(source, from + 1, out_end));

        case 'p': {
            auto temporary = read_number<int>(source, from + 1, out_end);
            float number;

            memcpy(&number, &temporary, sizeof(temporary));

            return Object::make_float(number);
        }

        case 'P': {
            auto temporary = read_number<long long>(source, from + 1, out_end);
            double number;

            memcpy(&number, &temporary, sizeof(number));
            
            return Object::make_float(number);
        }

        case 's':
            return Object::make_string(read_string<unsigned int>(source, from + 1, out_end));

        case 'S':
            return Object::make_string(read_string<unsigned long long>(source, from + 1, out_end));

        case 'a':
            return Object::make_array(read_array<unsigned int>(source, from + 1, out_end));

        case 'A':
            return Object::make_array(read_array<unsigned long long>(source, from + 1, out_end));

        case 'd':
            return Object::make_dictionary(read_dictionary<unsigned int>(source, from + 1, out_end));

        case 'D':
            return Object::make_dictionary(read_dictionary<unsigned long long>(source, from + 1, out_end));
    }

    assert(false && "Unknown byte in RBDEF");
}


Object rosewood::data_format::read_data(const std::string &source) {
    return read_from(source, 0, nullptr);
}
