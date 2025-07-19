#pragma once

#include <stdint.h>
#include <iostream>

#include "core/PegasusState.hpp"

namespace pegasus
{
    constexpr size_t VLEN_MIN = 32;

    class VectorConfig
    {
      public:
        VectorConfig() = default;

        VectorConfig(size_t vlen, size_t lmul, size_t sew, bool vta, bool vma, size_t vl,
                     size_t vstart) :
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

        size_t getVLEN() const { return vlen_; }

        void setVLEN(size_t value)
        {
            // VLEN can only be power of 2, and >= VLEN_MIN.
            sparta_assert(((value & (value - 1)) == 0 && value >= VLEN_MIN), "Invalid LMUL value.");
            vlen_ = value;
        }

        size_t getLMUL() const { return lmul_; }

        void setLMUL(size_t value)
        {
            // LMUL can only be power of 2, and elememnt should fit into register group.
            sparta_assert(((value & (value - 1)) == 0 && (sew_ <= vlen_ / 8 * value)),
                          "Invalid LMUL value.");
            lmul_ = value;
        }

        size_t getSEW() const { return sew_; }

        void setSEW(size_t value)
        {
            // SEW can only be power of 2, and elememnt should fit into register group.
            sparta_assert(((value & (value - 1)) == 0 && (value <= vlen_ / 8 * lmul_)),
                          "Unsupported SEW value.");
            sew_ = value;
        }

        bool getVTA() const { return vta_; }

        void setVTA(size_t value) { vta_ = value; }

        bool getVMA() const { return vma_; }

        void setVMA(size_t value) { vma_ = value; }

        size_t getVL() const { return vl_; }

        void setVL(size_t value) { vl_ = value; }

        size_t getVSTART() const { return vstart_; }

        void setVSTART(size_t value) { vstart_ = value; }

        // helper functions

        size_t getVLMAX() const { return getVLEN() / 8 * getLMUL() / getSEW(); }

        template <typename XLEN> XLEN vsetAVL(PegasusState* state, bool set_max, XLEN avl = 0)
        {
            const size_t vl = set_max ? getVLMAX() : std::min<size_t>(getVLMAX(), avl);
            setVL(vl);
            WRITE_CSR_REG<XLEN>(state, VL, vl);
            return vl;
        }

        template <typename XLEN> void vsetVTYPE(PegasusState* state, XLEN vtype)
        {
            WRITE_CSR_REG<XLEN>(state, VTYPE, vtype);
            const size_t vlmul = READ_CSR_FIELD<XLEN>(state, VTYPE, "vlmul");

            static const size_t lmul_table[8] = {
                8,  // 000
                16, // 001
                32, // 010
                64, // 011
                0,  // 100 (invalid)
                1,  // 101
                2,  // 110
                4   // 111
            };

            const size_t lmul = lmul_table[vlmul & 0b111];
            sparta_assert(lmul, "Invalid vtype VLMUL encoding.");
            setLMUL(lmul);
            setSEW(8u << READ_CSR_FIELD<XLEN>(state, VTYPE, "vsew"));
            setVTA(READ_CSR_FIELD<XLEN>(state, VTYPE, "vta"));
            setVMA(READ_CSR_FIELD<XLEN>(state, VTYPE, "vma"));
        }

      private:
        // member variables

        size_t vlen_ = 64;

        // VTYPE CSR
        size_t lmul_ = 8; // unit: one 8th
        size_t sew_ = 8;  // unit: one bit
        bool vta_ = false;
        bool vma_ = false;

        // VL CSR
        size_t vl_ = 0;

        // VSTART CSR
        size_t vstart_ = 0;
    }; // class VectorConfig

    inline std::ostream & operator<<(std::ostream & os, const VectorConfig & config)
    {
        os << "VLEN: " << config.getVLEN() << " ";
        os << "LMUL: ";
        if (config.getLMUL() < 8)
        {
            os << "1/" << 8 / config.getLMUL() << " ";
        }
        else
        {
            os << config.getLMUL() / 8 << " ";
        }
        os << "SEW: " << config.getSEW() << " ";
        os << "VTA: " << std::boolalpha << config.getVTA() << " "
           << "VMA: " << config.getVMA() << std::noboolalpha << " ";
        os << "VL: " << config.getVL() << " "
           << "VSTART: " << config.getVSTART() << "; ";
        return os;
    }
} // namespace pegasus
