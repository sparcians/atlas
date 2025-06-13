#pragma once

#include <stdint.h>
#include <iostream>

#include "core/AtlasState.hpp"

namespace atlas
{
    constexpr uint8_t VLEN_MIN = 32;

    class VectorConfig
    {
      public:
        VectorConfig() = default;

        VectorConfig(uint8_t vlen, uint8_t lmul, uint8_t sew, bool vta, bool vma, uint8_t vl,
                     uint8_t vstart) :
            vlen_(vlen),
            lmul_(lmul),
            sew_(sew),
            vta_(vta),
            vma_(vma),
            vl_(vl),
            vstart_(vstart)
        {
        }

        explicit VectorConfig(const VectorConfig &) = default;

        // member accessors

        inline uint8_t getVLEN() const { return vlen_; }

        inline void setVLEN(uint8_t value)
        {
            // VLEN can only be power of 2, and >= VLEN_MIN.
            sparta_assert(((value & (value - 1)) == 0 && value >= VLEN_MIN), "Invalid LMUL value.");
            vlen_ = value;
        }

        inline uint8_t getLMUL() const { return lmul_; }

        inline void setLMUL(uint8_t value)
        {
            // LMUL can only be power of 2, and elememnt should fit into register group.
            sparta_assert(((value & (value - 1)) == 0 && (sew_ <= vlen_ / 8 * value)),
                          "Invalid LMUL value.");
            lmul_ = value;
        }

        inline uint8_t getSEW() const { return sew_; }

        inline void setSEW(uint8_t value)
        {
            // SEW can only be power of 2, and elememnt should fit into register group.
            sparta_assert(((value & (value - 1)) == 0 && (value <= vlen_ / 8 * lmul_)),
                          "Invalid SEW value.");
            sew_ = value;
        }

        inline bool getVTA() const { return vta_; }

        inline void setVTA(uint8_t value) { vta_ = value; }

        inline bool getVMA() const { return vma_; }

        inline void setVMA(uint8_t value) { vma_ = value; }

        inline uint8_t getVL() const { return vl_; }

        inline void setVL(uint8_t value) { vl_ = value; }

        inline uint8_t getVSTART() const { return vstart_; }

        inline void setVSTART(uint8_t value) { vstart_ = value; }

        // helper functions

        inline uint8_t getVLMAX() const { return getVLEN() / 8 * getLMUL() / getSEW(); }

        template <typename XLEN> XLEN vsetAVL(AtlasState* state, bool set_max, XLEN avl = 0)
        {
            const uint8_t vl = set_max ? getVLMAX() : std::min<uint8_t>(getVLMAX(), avl);
            setVL(vl);
            WRITE_CSR_REG<XLEN>(state, VL, vl);
            return vl;
        }

        template <typename XLEN> void vsetVTYPE(AtlasState* state, XLEN vtype)
        {
            WRITE_CSR_REG<XLEN>(state, VTYPE, vtype);
            const uint8_t vlmul = READ_CSR_FIELD<XLEN>(state, VTYPE, "vlmul");

            static const uint8_t lmul_table[8] = {
                8,  // 000
                16, // 001
                32, // 010
                64, // 011
                0,  // 100 (invalid)
                1,  // 101
                2,  // 110
                4   // 111
            };

            const uint8_t lmul = lmul_table[vlmul & 0b111];
            sparta_assert(lmul, "Invalid vtype VLMUL encoding.");
            setLMUL(lmul);
            setSEW(8u << READ_CSR_FIELD<XLEN>(state, VTYPE, "vsew"));
            setVTA(READ_CSR_FIELD<XLEN>(state, VTYPE, "vta"));
            setVMA(READ_CSR_FIELD<XLEN>(state, VTYPE, "vma"));
        }

      private:
        // member variables

        uint8_t vlen_ = 64;

        // VTYPE CSR
        uint8_t lmul_ = 8; // unit: one 8th
        uint8_t sew_ = 8;  // unit: one bit
        bool vta_ = false;
        bool vma_ = false;

        // VL CSR
        uint8_t vl_ = 0;

        // VSTART CSR
        uint8_t vstart_ = 0;
    }; // class VectorConfig

    inline std::ostream & operator<<(std::ostream & os, const VectorConfig & config)
    {
        os << "VLEN: " << (uint32_t)config.getVLEN() << " ";
        os << "LMUL: ";
        if (config.getLMUL() < 8)
        {
            os << "1/" << 8 / (uint32_t)config.getLMUL() << " ";
        }
        else
        {
            os << (uint32_t)config.getLMUL() / 8 << " ";
        }
        os << "SEW: " << (uint32_t)config.getSEW() << " ";
        os << "VTA: " << std::boolalpha << config.getVTA() << " " << "VMA: " << config.getVMA()
           << std::noboolalpha << " ";
        os << "VL: " << (uint32_t)config.getVL() << " "
           << "VSTART: " << (uint32_t)config.getVSTART() << "; ";
        return os;
    }
} // namespace atlas
