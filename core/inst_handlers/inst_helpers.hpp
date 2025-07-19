#pragma once

#include <stdint.h>

typedef int64_t sreg_t;
typedef uint64_t reg_t;

template <typename T> inline sreg_t sext32(T x) { return (sreg_t)(int32_t)(x); }

template <typename T> inline reg_t zext32(T x) { return (reg_t)(uint32_t)(x); }

template <typename T> inline sreg_t sext(T x, uint32_t pos)
{
    return (((sreg_t)(x) << (64 - (pos))) >> (64 - (pos)));
}

template <typename T> inline reg_t zext(T x, uint32_t pos)
{
    return (((reg_t)(x) << (64 - (pos))) >> (64 - (pos)));
}

inline uint64_t mulhu(uint64_t a, uint64_t b)
{
    uint64_t t = 0;
    uint32_t y1 = 0, y2 = 0;
    uint64_t a0 = (uint32_t)a, a1 = a >> 32;
    uint64_t b0 = (uint32_t)b, b1 = b >> 32;

    t = a1 * b0 + ((a0 * b0) >> 32);
    y1 = t;
    y2 = t >> 32;

    t = a0 * b1 + y1;

    t = a1 * b1 + y2 + (t >> 32);

    return t;
}

inline int64_t mulhsu(int64_t a, uint64_t b)
{
    int negate = a < 0;
    uint64_t res = mulhu(a < 0 ? -a : a, b);
    return negate ? ~res + (a * b == 0) : res;
}

inline int64_t mulh(int64_t a, int64_t b)
{
    int negate = (a < 0) != (b < 0);
    uint64_t res = mulhu(a < 0 ? -a : a, b < 0 ? -b : b);
    return negate ? ~res + ((uint64_t)a * (uint64_t)b == 0) : res;
}

namespace pegasus
{
    template <uint64_t Mask> struct RegisterBitMask
    {
        static uint64_t mask(const uint64_t old_val, const uint64_t new_val)
        {
            // The 'Mask' template parameter is a bit mask that specifies which bits are writable.
            // We need to preserve the 'old_val' bits that are not writable, and replace the
            // writable bits with 'new_val'.
            return (old_val & ~Mask) | (new_val & Mask);
        }
    };

    template <> struct RegisterBitMask<0>;

    template <> struct RegisterBitMask<0xffffffffffffffff>
    {
        static uint64_t mask(const uint64_t old_val, const uint64_t new_val)
        {
            (void)old_val;
            return new_val;
        }
    };

    template <typename FieldT, typename Enable = void> struct CSRFields
    {
        static uint64_t readField(const uint64_t reg_val)
        {
            constexpr uint64_t num_field_bits = FieldT::high_bit - FieldT::low_bit + 1;
            constexpr uint64_t mask = (1 << num_field_bits) - 1;
            return (reg_val >> FieldT::low_bit) & mask;
        }
    };

} // namespace pegasus
