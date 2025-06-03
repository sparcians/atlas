#pragma once

#include <stdint.h>
#include <iostream>
#include <vector>

#include "core/AtlasState.hpp"
#include "core/VecNums.hpp"
#include "include/AtlasUtils.hpp"

namespace atlas
{
    constexpr uint8_t VLEN_MIN = 32;

    class VectorConfig
    {
      public:
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

        inline uint8_t getVSTART() const { return vstart; }

        inline void setVSTART(uint8_t value) { vstart = value; }

        // helper functions

        inline uint8_t getVLMAX() { return getVLEN() / 8 * getLMUL() / getSEW(); }

        template <typename XLEN> XLEN vsetAVL(AtlasState* state_ptr, bool set_max, XLEN avl = 0)
        {
            uint8_t vl = set_max ? getVLMAX() : std::min<uint8_t>(getVLMAX(), avl);
            setVL(vl);
            WRITE_CSR_REG<XLEN>(state_ptr, VL, vl);
            return vl;
        }

        template <typename XLEN> void vsetVTYPE(AtlasState* state_ptr, XLEN vtype)
        {
            WRITE_CSR_REG<XLEN>(state_ptr, VTYPE, vtype);
            uint8_t vlmul = READ_CSR_FIELD<XLEN>(state_ptr, VTYPE, "vlmul");
            switch (vlmul)
            {
                case 0b101:
                    setLMUL(1);
                    break;
                case 0b110:
                    setLMUL(2);
                    break;
                case 0b111:
                    setLMUL(4);
                    break;
                case 0b000:
                    setLMUL(8);
                    break;
                case 0b001:
                    setLMUL(16);
                    break;
                case 0b010:
                    setLMUL(32);
                    break;
                case 0b011:
                    setLMUL(64);
                    break;
                default:
                    sparta_assert(false, "Invalid vtype VLMUL encoding.");
                    break;
            }
            setSEW(8u << READ_CSR_FIELD<XLEN>(state_ptr, VTYPE, "vsew"));
            setVTA(READ_CSR_FIELD<XLEN>(state_ptr, VTYPE, "vta"));
            setVMA(READ_CSR_FIELD<XLEN>(state_ptr, VTYPE, "vma"));
        }

        template <typename XLEN> void vsetVSTART(AtlasState* state_ptr, XLEN vstart)
        {
            setVSTART(vstart);
            WRITE_CSR_REG<XLEN>(state_ptr, VSTART, vstart);
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
        uint8_t vstart = 0;
    }; // class VectorConfig

    template <uint8_t ElemWidth> struct VectorElement
    {
        using ElemType = GetUintType<ElemWidth>;

        void setVal(ElemType value) { WRITE_VEC_ELEM<ElemType>(state_ptr, reg_id, value, idx); }

        ElemType getVal() { return READ_VEC_ELEM<ElemType>(state_ptr, reg_id, idx); }

        AtlasState* state_ptr;
        uint32_t reg_id;
        uint32_t idx;
    }; // struct VectorElement

    class VectorMask
    {
      public:
        VectorMask(AtlasState* state_ptr, uint32_t reg_id) :
            state_ptr_(state_ptr),
            config_ptr_(state_ptr_->getVectorConfig())
        {
            for (uint8_t idx = 0; idx < config_ptr_->getVL() / VLEN_MIN; ++idx)
            {
                mask_.push_back(READ_VEC_ELEM<GetUintType<VLEN_MIN>>(state_ptr, reg_id, idx));
            }
        }

        VectorMask() = default;

        uint8_t getNextIndex(uint8_t index)
        {
            const uint8_t vl = config_ptr_->getVL();
            if (index >= vl)
            {
                return vl;
            }
            while (index < vl)
            {
                if ((mask_[index / VLEN_MIN] >> (index % VLEN_MIN)) & 1)
                {
                    break;
                }
                ++index;
            }
            return index;
        }

      private:
        AtlasState* state_ptr_;
        const VectorConfig* config_ptr_;
        std::vector<GetUintType<VLEN_MIN>> mask_;
    }; // class VectorMask

    template <uint8_t ElemWidth> class VectorElements
    {
      public:
        class ElementIterator
        {
          public:
            using difference_type = std::ptrdiff_t;
            using value_type = VectorElement<ElemWidth>;
            using pointer = VectorElement<ElemWidth>*;
            using reference = VectorElement<ElemWidth> &;
            using iterator_category = std::forward_iterator_tag;

            ElementIterator(VectorElements<ElemWidth>* elems_ptr, uint8_t index) :
                elems_ptr_(elems_ptr),
                index_(index)
            {
                if (elems_ptr_->masked_)
                {
                    index_ = elems_ptr->mask_.getNextIndex(index_);
                }
            }

            reference operator*()
            {
                elems_ptr_->current_.state_ptr = elems_ptr_->state_ptr_;
                elems_ptr_->current_.reg_id =
                    elems_ptr_->reg_id_ + index_ / (elems_ptr_->config_ptr_->getVLEN() / ElemWidth);
                elems_ptr_->current_.idx =
                    index_ % (elems_ptr_->config_ptr_->getVLEN() / ElemWidth);
                return elems_ptr_->current_;
            }

            pointer operator->() { return &(operator*()); }

            ElementIterator & operator++()
            {
                if (elems_ptr_->isMasked())
                {
                    index_ = elems_ptr_->mask_.getNextIndex(++index_);
                }
                else
                {
                    ++index_;
                }
                return *this;
            }

            ElementIterator operator++(int)
            {
                ++(*this);
                return *this;
            }

            bool operator!=(const ElementIterator & other) const
            {
                return index_ != other.index_ && elems_ptr_ == other.elems_ptr_;
            }

            bool operator==(const ElementIterator & other) const { return !(*this != other); }

            difference_type operator-(const ElementIterator & other) const
            {
                return static_cast<difference_type>(index_) - other.index_;
            }

            uint8_t inline getIndex() const { return index_; }

          private:
            VectorElements<ElemWidth>* elems_ptr_;
            uint8_t index_;
        }; // class ElementIterator

        VectorElements(AtlasState* state_ptr, uint32_t reg_id, bool masked) :
            state_ptr_(state_ptr),
            config_ptr_(state_ptr_->getVectorConfig()),
            index_(config_ptr_->getVSTART()),
            reg_id_(reg_id),
            masked_(masked),
            mask_(masked_ ? VectorMask{state_ptr, atlas::V0} : VectorMask{})
        {
        }

        inline bool isMasked() { return masked_; }

        ElementIterator begin() { return ElementIterator(this, config_ptr_->getVSTART()); }

        ElementIterator end() { return ElementIterator(this, config_ptr_->getVL()); }

      private:
        AtlasState* state_ptr_;
        const VectorConfig* config_ptr_;
        uint8_t index_;
        uint32_t reg_id_;
        bool masked_;
        VectorElement<ElemWidth> current_;
        VectorMask mask_;

        friend ElementIterator;
    }; // class VectorElements

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
        os << "VTA: " << config.getVTA() << " " << "VMA: " << config.getVMA() << " ";
        os << "VL: " << config.getVL() << " " << "VSTART: " << config.getVSTART() << "; ";
        return os;
    }
} // namespace atlas