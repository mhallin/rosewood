#include <gtest/gtest.h>

#include "rosewood/data-format/object.h"
#include "rosewood/data-format/object_conversions.h"
#include "rosewood/data-format/reader.h"

using namespace rosewood::data_format;

TEST(DataFormatTests, ReadNull) {
    ASSERT_EQ(DataType::Null, read_data("n").type);
}

TEST(DataFormatTests, ReadTrue) {
    ASSERT_EQ(DataType::Boolean, read_data("t").type);
    EXPECT_EQ(true, read_data("t").boolean);
}

TEST(DataFormatTests, ReadFalse) {
    ASSERT_EQ(DataType::Boolean, read_data("f").type);
    EXPECT_EQ(false, read_data("f").boolean);
}

TEST(DataFormatTests, ReadInt32) {
    auto object = read_data("i\x0a\x0b\x0c\x0d");
    ASSERT_EQ(DataType::IntOrFloat, object.type);
    EXPECT_EQ(0x0a0b0c0d, object.int64);
    EXPECT_EQ(0x0a0b0c0d, object.float64);
}

TEST(DataFormatTests, ReadInt64) {
    auto object = read_data(std::string("I\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01", 9));
    ASSERT_EQ(DataType::IntOrFloat, object.type);
    EXPECT_EQ(0x0a0b0c0d0e0f0001, object.int64);
    EXPECT_EQ(0x0a0b0c0d0e0f0001, object.float64);
}

TEST(DataFormatTests, ReadFloat32) {
    auto object = read_data(std::string("p\x40\xb0\x00\x00", 5));
    ASSERT_EQ(DataType::Float, object.type);
    EXPECT_EQ(5.5, object.float64);
}

TEST(DataFormatTests, ReadFloat64) {
    auto object = read_data(std::string("P\x3F\xF0\x84\x0A\xD0\x08\xC1\x10", 9));
    ASSERT_EQ(DataType::Float, object.type);
    EXPECT_EQ(1.03223687423093579695887456182, object.float64);
}

TEST(DataFormatTests, ReadStringSmall) {
    auto object = read_data(std::string("s\x00\x00\x00\x09test\x00more", 14));
    ASSERT_EQ(DataType::String, object.type);
    EXPECT_EQ(9, object.string.size());
    EXPECT_EQ(std::string("test\x00more", 9), object.string);
}

TEST(DataFormatTests, ReadStringLarge) {
    auto object = read_data(std::string("S\x00\x00\x00\x00\x00\x00\x00\x09test\x00more", 18));
    ASSERT_EQ(DataType::String, object.type);
    EXPECT_EQ(9, object.string.size());
    EXPECT_EQ(std::string("test\x00more", 9), object.string);
}

TEST(DataFormatTests, ReadArraySmall) {
    auto object = read_data(std::string("a\x00\x00\x00\x09" //                              =>  5 (0)
                                        "n" "t" "f" // null, true, false                    =>  3 (3)
                                        "i\x0a\x0b\x0c\x0d" // 0x0a0b0c0d                   =>  5 (1)
                                        "I\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01" //              =>  9 (1)
                                        "p\x40\xb0\x00\x00" //                              =>  5 (1)
                                        "P\x3F\xF0\x84\x0A\xD0\x08\xC1\x10" //              =>  9 (1)
                                        "s\x00\x00\x00\x09test\x00more" //                  => 14 (1)
                                        "S\x00\x00\x00\x00\x00\x00\x00\x09test\x00more", // => 18 (1)
                                        68));

    ASSERT_EQ(DataType::Array, object.type);
    ASSERT_EQ(9, object.array.size());

    ASSERT_EQ(DataType::Null, object[0].type);
    ASSERT_EQ(DataType::Boolean, object[1].type);
    ASSERT_EQ(DataType::Boolean, object[2].type);
    ASSERT_EQ(DataType::IntOrFloat, object[3].type);
    ASSERT_EQ(DataType::IntOrFloat, object[4].type);
    ASSERT_EQ(DataType::Float, object[5].type);
    ASSERT_EQ(DataType::Float, object[6].type);
    ASSERT_EQ(DataType::String, object[7].type);
    ASSERT_EQ(DataType::String, object[8].type);

    EXPECT_EQ(true, object[1].boolean);
    EXPECT_EQ(false, object[2].boolean);

    EXPECT_EQ(0x0a0b0c0d, object[3].int64);
    EXPECT_EQ(0x0a0b0c0d, object[3].float64);

    EXPECT_EQ(0x0a0b0c0d0e0f0001, object[4].int64);
    EXPECT_EQ(0x0a0b0c0d0e0f0001, object[4].float64);

    EXPECT_EQ(5.5, object[5].float64);

    EXPECT_EQ(1.03223687423093579695887456182, object[6].float64);

    EXPECT_EQ(9, object[7].string.size());
    EXPECT_EQ(std::string("test\x00more", 9), object[7].string);

    EXPECT_EQ(9, object[8].string.size());
    EXPECT_EQ(std::string("test\x00more", 9), object[8].string);
}

TEST(DataFormatTests, ReadArrayLarge) {
    auto object = read_data(std::string("A\x00\x00\x00\x00\x00\x00\x00\x09" //              =>  9 (0)
                                        "n" "t" "f" // null, true, false                    =>  3 (3)
                                        "i\x0a\x0b\x0c\x0d" // 0x0a0b0c0d                   =>  5 (1)
                                        "I\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01" //              =>  9 (1)
                                        "p\x40\xb0\x00\x00" //                              =>  5 (1)
                                        "P\x3F\xF0\x84\x0A\xD0\x08\xC1\x10" //              =>  9 (1)
                                        "s\x00\x00\x00\x09test\x00more" //                  => 14 (1)
                                        "S\x00\x00\x00\x00\x00\x00\x00\x09test\x00more", // => 18 (1)
                                        72));

    ASSERT_EQ(DataType::Array, object.type);
    ASSERT_EQ(9, object.array.size());

    ASSERT_EQ(DataType::Null, object[0].type);
    ASSERT_EQ(DataType::Boolean, object[1].type);
    ASSERT_EQ(DataType::Boolean, object[2].type);
    ASSERT_EQ(DataType::IntOrFloat, object[3].type);
    ASSERT_EQ(DataType::IntOrFloat, object[4].type);
    ASSERT_EQ(DataType::Float, object[5].type);
    ASSERT_EQ(DataType::Float, object[6].type);
    ASSERT_EQ(DataType::String, object[7].type);
    ASSERT_EQ(DataType::String, object[8].type);

    EXPECT_EQ(true, object[1].boolean);
    EXPECT_EQ(false, object[2].boolean);

    EXPECT_EQ(0x0a0b0c0d, object[3].int64);
    EXPECT_EQ(0x0a0b0c0d, object[3].float64);

    EXPECT_EQ(0x0a0b0c0d0e0f0001, object[4].int64);
    EXPECT_EQ(0x0a0b0c0d0e0f0001, object[4].float64);

    EXPECT_EQ(5.5, object[5].float64);

    EXPECT_EQ(1.03223687423093579695887456182, object[6].float64);

    EXPECT_EQ(9, object[7].string.size());
    EXPECT_EQ(std::string("test\x00more", 9), object[7].string);

    EXPECT_EQ(9, object[8].string.size());
    EXPECT_EQ(std::string("test\x00more", 9), object[8].string);
}

TEST(DataFormatTests, ReadDictionarySmall) {
    auto object = read_data(std::string("d\x00\x00\x00\x03" //                      =>  5 (0)
                                        "s\x00\x00\x00\x04truet" //                 => 10 (1) "true": true
                                        "s\x00\x00\x00\05falsef" //                 => 11 (1) "false": false
                                        "s\x00\x00\x00\04fouri\x00\x00\x00\x04", // => 14 (1) "four": 4
                                        40));

    ASSERT_EQ(DataType::Dictionary, object.type);
    ASSERT_EQ(3, object.dictionary.size());

    ASSERT_EQ(1, object.dictionary.count("true"));
    ASSERT_EQ(1, object.dictionary.count("false"));
    ASSERT_EQ(1, object.dictionary.count("four"));

    EXPECT_EQ(DataType::Boolean, object["true"].type);
    EXPECT_EQ(true, object["true"].boolean);

    EXPECT_EQ(DataType::Boolean, object["false"].type);
    EXPECT_EQ(false, object["false"].boolean);

    EXPECT_EQ(DataType::IntOrFloat, object["four"].type);
    EXPECT_EQ(4, object["four"].int64);
}

TEST(DataFormatTests, ReadDictionaryLarge) {
    auto object = read_data(std::string("D\x00\x00\x00\x00\x00\x00\x00\x03" //      =>  9 (0)
                                        "s\x00\x00\x00\x04truet" //                 => 10 (1) "true": true
                                        "s\x00\x00\x00\05falsef" //                 => 11 (1) "false": false
                                        "s\x00\x00\x00\04fouri\x00\x00\x00\x04", // => 14 (1) "four": 4
                                        44));

    ASSERT_EQ(DataType::Dictionary, object.type);
    ASSERT_EQ(3, object.dictionary.size());

    ASSERT_EQ(1, object.dictionary.count("true"));
    ASSERT_EQ(1, object.dictionary.count("false"));
    ASSERT_EQ(1, object.dictionary.count("four"));

    EXPECT_EQ(DataType::Boolean, object["true"].type);
    EXPECT_EQ(true, object["true"].boolean);

    EXPECT_EQ(DataType::Boolean, object["false"].type);
    EXPECT_EQ(false, object["false"].boolean);

    EXPECT_EQ(DataType::IntOrFloat, object["four"].type);
    EXPECT_EQ(4, object["four"].int64);
}

TEST(DataFormatTests, IntConversion) {
    auto object = read_data("i\x0a\x0b\x0c\x0d");

    EXPECT_EQ(0x0a0b0c0d, as<int>(object));
    EXPECT_EQ(0x0a0b0c0d, as<long>(object));
    EXPECT_EQ(0x0a0b0c0d, as<unsigned int>(object));
    EXPECT_EQ(0x0a0b0c0d, as<unsigned long>(object));
    EXPECT_EQ(0x0a0b0c0d, as<float>(object));
    EXPECT_EQ(0x0a0b0c0d, as<double>(object));
}

TEST(DataFormatTests, BoolTrueConversion) {
    auto object = read_data("t");

    EXPECT_EQ(true, as<bool>(object));
}

TEST(DataFormatTests, BoolFalseConversion) {
    auto object = read_data("f");

    EXPECT_EQ(false, as<bool>(object));
}

TEST(DataFormatTests, HomogenousIntArrayConversion) {
    auto object = read_data(std::string("a\x00\x00\x00\x04"
                                        "i\x00\x00\x00\x02"
                                        "i\x00\x00\x00\x04"
                                        "i\x00\x00\x00\x06"
                                        "i\x00\x00\x00\x08",
                                        5 * 5));

    EXPECT_EQ((std::vector<int>{2, 4, 6, 8}), as<std::vector<int>>(object));
    EXPECT_EQ((std::vector<float>{2, 4, 6, 8}), as<std::vector<float>>(object));
}

TEST(DataFormatTests, HeterogenousNumberArrayConversion) {
    auto object = read_data(std::string("a\x00\x00\x00\x04"
                                        "p\x41\x48\x00\x00"
                                        "i\x00\x00\x00\x04"
                                        "p\x40\xb0\x00\x00"
                                        "i\x00\x00\x00\x08",
                                        5 * 5));

    EXPECT_EQ((std::vector<float>{12.5, 4, 5.5, 8}), as<std::vector<float>>(object));
}

TEST(DataFormatTests, StringNumberDictionary) {
    auto object = read_data(std::string("d\x00\x00\x00\x04"
                                        "s\x00\x00\x00\x03twoi\x00\x00\x00\x02"
                                        "s\x00\x00\x00\x03sixi\x00\x00\x00\x06"
                                        "s\x00\x00\x00\x03onei\x00\x00\x00\x01"
                                        "s\x00\x00\x00\x03teni\x00\x00\x00\x0a",
                                        13 * 4 + 5));

    std::unordered_map<std::string, int> mapping{
        { "one", 1 },
        { "two", 2 },
        { "six", 6 },
        { "ten", 10 },
    };

    EXPECT_EQ(mapping, (as<std::unordered_map<std::string, int>>(object)));
}
