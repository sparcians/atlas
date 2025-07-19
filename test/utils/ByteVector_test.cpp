#include "include/PegasusUtils.hpp"

#include "sparta/utils/SpartaTester.hpp"

void testConvertToByteVector()
{
    {
        const uint64_t value = 0xdeadbeef;
        std::vector<uint8_t> byte_vector = pegasus::convertToByteVector<uint64_t>(value);
        EXPECT_EQUAL(byte_vector.size(), 8);
        EXPECT_EQUAL(byte_vector[0], 0xef);
        EXPECT_EQUAL(byte_vector[1], 0xbe);
        EXPECT_EQUAL(byte_vector[2], 0xad);
        EXPECT_EQUAL(byte_vector[3], 0xde);
        EXPECT_EQUAL(byte_vector[4], 0x0);
        EXPECT_EQUAL(byte_vector[5], 0x0);
        EXPECT_EQUAL(byte_vector[6], 0x0);
        EXPECT_EQUAL(byte_vector[7], 0x0);
    }
    {
        const uint32_t value = 0x12345678;
        std::vector<uint8_t> byte_vector = pegasus::convertToByteVector<uint32_t>(value);
        EXPECT_EQUAL(byte_vector.size(), 4);
        EXPECT_EQUAL(byte_vector[0], 0x78);
        EXPECT_EQUAL(byte_vector[1], 0x56);
        EXPECT_EQUAL(byte_vector[2], 0x34);
        EXPECT_EQUAL(byte_vector[3], 0x12);
    }
    {
        const uint16_t value = 0xabcd;
        std::vector<uint8_t> byte_vector = pegasus::convertToByteVector<uint16_t>(value);
        EXPECT_EQUAL(byte_vector.size(), 2);
        EXPECT_EQUAL(byte_vector[0], 0xcd);
        EXPECT_EQUAL(byte_vector[1], 0xab);
    }
    {
        const uint8_t value = 0xff;
        std::vector<uint8_t> byte_vector = pegasus::convertToByteVector<uint8_t>(value);
        EXPECT_EQUAL(byte_vector.size(), 1);
        EXPECT_EQUAL(byte_vector[0], 0xff);
    }
}

void testConvertFromByteVector()
{
    {
        const std::vector<uint8_t> byte_vector{0xef, 0xbe, 0xad, 0xde, 0x0, 0x0, 0x0, 0x0};
        const uint64_t value = pegasus::convertFromByteVector<uint64_t>(byte_vector);
        EXPECT_EQUAL(value, 0xdeadbeef);
    }
    {
        const std::vector<uint8_t> byte_vector{0x78, 0x56, 0x34, 0x12};
        const uint32_t value = pegasus::convertFromByteVector<uint32_t>(byte_vector);
        EXPECT_EQUAL(value, 0x12345678);
    }
    {
        const std::vector<uint8_t> byte_vector{0xcd, 0xab};
        const uint16_t value = pegasus::convertFromByteVector<uint16_t>(byte_vector);
        EXPECT_EQUAL(value, 0xabcd);
    }
    {
        const std::vector<uint8_t> byte_vector{0xff};
        const uint8_t value = pegasus::convertFromByteVector<uint8_t>(byte_vector);
        EXPECT_EQUAL(value, 0xff);
    }
    {
        const std::vector<uint8_t> byte_vector{0xff};
        EXPECT_THROW(pegasus::convertFromByteVector<uint64_t>(byte_vector));
    }
}

int main()
{
    testConvertToByteVector();
    testConvertFromByteVector();

    REPORT_ERROR;
    return ERROR_CODE;
}
