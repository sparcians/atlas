#pragma once

#include <stdint.h>
#include <iostream>

#include "core/AtlasState.hpp"

namespace atlas
{
    class VectorState
    {
      public:
        inline uint8_t getLMUL() const { return lmul_; }

        inline void setLMUL(uint8_t value) { lmul_ = value; }

        inline uint8_t getSEW() const { return sew_; }

        inline void setSEW(uint8_t value) { sew_ = value; }

        inline bool getVTA() const { return vta_; }

        inline void setVTA(uint8_t value) { vta_ = value; }

        inline bool getVMA() const { return vma_; }

        inline void setVMA(uint8_t value) { vma_ = value; }

        inline uint8_t getVL() const { return vl_; }

        inline void setVL(uint8_t value) { vl_ = value; }

        inline uint8_t getVSTART() const { return vstart_; }

        inline void setVSTART(uint8_t value) { vstart_ = value; }

        template <typename VLEN> inline uint8_t getVLMAX()
        {
            return sizeof(VLEN) * lmul_ / (sew_ == 0 ? 1 : sew_);
        }

        template <typename XLEN> void vsetVTYPE(AtlasState* state, XLEN vtype)
        {
            WRITE_CSR_REG<XLEN>(state, VTYPE, vtype);
            sew_ = 8u << READ_CSR_FIELD<XLEN>(state, VTYPE, "vsew");
            uint8_t vlmul_ = READ_CSR_FIELD<XLEN>(state, VTYPE, "vlmul");
            switch (vlmul_)
            {
                case 0b101:
                    lmul_ = 1;
                    break;
                case 0b110:
                    lmul_ = 2;
                    break;
                case 0b111:
                    lmul_ = 4;
                    break;
                case 0b000:
                    lmul_ = 8;
                    break;
                case 0b001:
                    lmul_ = 16;
                    break;
                case 0b010:
                    lmul_ = 32;
                    break;
                case 0b011:
                    lmul_ = 64;
                    break;
                default:
                    break;
            }
            vta_ = READ_CSR_FIELD<XLEN>(state, VTYPE, "vta");
            vma_ = READ_CSR_FIELD<XLEN>(state, VTYPE, "vma");
        }

        template <typename XLEN, typename VLEN>
        XLEN vsetAVL(AtlasState* state, bool set_max, XLEN avl = 0)
        {
            vl_ = set_max ? getVLMAX<VLEN>() : std::min<uint8_t>(getVLMAX<VLEN>(), avl);
            WRITE_CSR_REG<XLEN>(state, VL, vl_);
            return vl_;
        }

      private:
        // vtype csr
        uint8_t lmul_ = 8; // unit: one 8th
        uint8_t sew_ = 0;  // unit: one bit
        bool vta_ = false;
        bool vma_ = false;
        // vl csr
        uint8_t vl_ = 0;
        // vstart csr
        uint8_t vstart_ = 0;
    }; // struct VectorState

    template <typename VLEN>
    inline std::ostream & operator<<(std::ostream & os, const VectorState & vector_state)
    {
        os << "LMUL: ";
        if (vector_state.getLMUL() < 8)
        {
            os << "1/" << 8 / vector_state.getLMUL() << " ";
        }
        else
        {
            os << vector_state.getLMUL() / 8 << " ";
        }
        os << "SEW: " << vector_state.getSEW() << " ";
        os << "VTA: " << vector_state.getVTA() << " "
           << "VMA: " << vector_state.getVMA() << " ";
        os << "VL: " << vector_state.getVL() << " "
           << "VLMAX:" << vector_state.getVLMAX<VLEN>() << " "
           << "VSTART: " << vector_state.getVSTART() << "; ";
        return os;
    }

} // namespace atlas