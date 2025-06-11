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
        VectorConfig() : vlen_(64), lmul_(8), sew_(8), vta_(false), vma_(false), vl_(0), vstart_(0)
        {
        }

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
            uint8_t vl = set_max ? getVLMAX() : std::min<uint8_t>(getVLMAX(), avl);
            setVL(vl);
            WRITE_CSR_REG<XLEN>(state, VL, vl);
            return vl;
        }

        template <typename XLEN> void vsetVTYPE(AtlasState* state, XLEN vtype)
        {
            WRITE_CSR_REG<XLEN>(state, VTYPE, vtype);
            uint8_t vlmul = READ_CSR_FIELD<XLEN>(state, VTYPE, "vlmul");

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

            uint8_t lmul = lmul_table[vlmul & 0b111];
            sparta_assert(lmul, "Invalid vtype VLMUL encoding.");
            setLMUL(lmul);
            setSEW(8u << READ_CSR_FIELD<XLEN>(state, VTYPE, "vsew"));
            setVTA(READ_CSR_FIELD<XLEN>(state, VTYPE, "vta"));
            setVMA(READ_CSR_FIELD<XLEN>(state, VTYPE, "vma"));
        }

      private:
        // member variables

        uint8_t vlen_;

        // VTYPE CSR
        uint8_t lmul_; // unit: one 8th
        uint8_t sew_;  // unit: one bit
        bool vta_;
        bool vma_;

        // VL CSR
        uint8_t vl_;

        // VSTART CSR
        uint8_t vstart_;
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

    class IteratorIF
    {
      public:
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        IteratorIF(uint8_t index) : index_(index)
        {
            // index_ = getNextIndex(index_);
        }

        virtual ~IteratorIF() = default;

        IteratorIF & operator++()
        {
            index_ = this->getNextIndex(++index_);
            return *this;
        }

        virtual bool operator!=(const IteratorIF & other) const = 0;

        bool operator==(const IteratorIF & other) const { return !(*this != other); }

        difference_type operator-(const IteratorIF & other) const
        {
            return static_cast<difference_type>(index_) - other.index_;
        }

        uint8_t inline getIndex() const { return index_; }

        virtual uint8_t getNextIndex(uint8_t index) const = 0;

      protected:
        uint8_t index_;
    }; // class IteratorIF

    template <uint8_t ElemWidth> struct Element
    {
      public:
        using ValueType = UintType<ElemWidth>;

        class BitIterator : public IteratorIF
        {
          public:
            using value_type = uint8_t;

            BitIterator(const Element<ElemWidth>* elem_ptr, uint8_t index) :
                IteratorIF(index),
                elem_ptr_(elem_ptr)
            {
                index_ = getNextIndex(index_);
            }

            bool operator!=(const IteratorIF & other) const override
            {
                return index_ != other.getIndex()
                       || elem_ptr_ != dynamic_cast<const BitIterator &>(other).elem_ptr_;
            }

            uint8_t getNextIndex(uint8_t index) const override
            {
                const uint8_t end_pos = elem_ptr_->getEndPos();
                index = std::max(index, elem_ptr_->getStartPos());
                if (index >= end_pos)
                {
                    return end_pos;
                }
                while (index < end_pos)
                {
                    if ((elem_ptr_->peekVal() >> index) & 1)
                    {
                        break;
                    }
                    ++index;
                }
                return index;
            }

          private:
            const Element<ElemWidth>* elem_ptr_;
        }; // class BitIterator

        Element() :
            state_(nullptr),
            reg_id_(0),
            idx_(0),
            start_pos_(0),
            end_pos_(ElemWidth),
            val_(0)
        {
        }

        virtual ~Element() = default;

        uint8_t getWidth() { return ElemWidth; }

        void setState(AtlasState* state) { state_ = state; }

        void setRegId(uint32_t reg_id) { reg_id_ = reg_id; }

        void setIdx(uint32_t idx) { idx_ = idx; }

        uint8_t getStartPos() const { return start_pos_; }

        void setStartPos(uint8_t start_pos) { start_pos_ = start_pos; }

        uint8_t getEndPos() const { return end_pos_; }

        void setEndPos(uint8_t end_pos) { end_pos_ = end_pos; }

        ValueType peekVal() const { return val_; }

        ValueType getVal()
        {
            ValueType bitmask = (((ValueType)1 << (end_pos_ - start_pos_)) - 1) << start_pos_;
            val_ = READ_VEC_ELEM<ValueType>(state_, reg_id_, idx_) & bitmask;
            return val_;
        }

        void setVal(ValueType value)
        {
            ValueType bitmask = (((ValueType)1 << (end_pos_ - start_pos_)) - 1) << start_pos_;
            val_ = value & bitmask;
            WRITE_VEC_ELEM<ValueType>(state_, reg_id_, val_, idx_);
        }

        BitIterator begin() { return BitIterator(this, start_pos_); }

        BitIterator end() { return BitIterator(this, end_pos_); }

      private:
        AtlasState* state_;
        uint32_t reg_id_;
        uint32_t idx_;
        uint8_t start_pos_;
        uint8_t end_pos_;
        ValueType val_;
    }; // struct Element

    template <typename ElemType> class Elements
    {
      public:
        class ElementIterator : public IteratorIF
        {
          public:
            using value_type = ElemType;
            using pointer = ElemType*;
            using reference = ElemType &;

            // template <typename T = ElemType, std::enable_if_t<!std::is_same_v<T, MaskElement>>>
            ElementIterator(const Elements* elems_ptr, uint8_t index) :
                IteratorIF(index),
                elems_ptr_(elems_ptr)
            {
                index_ = getNextIndex(index_);
            }

            virtual reference operator*()
            {
                current_.setState(elems_ptr_->state_);
                current_.setRegId(elems_ptr_->reg_id_
                                  + index_
                                        / (elems_ptr_->config_->getVLEN() / current_.getWidth()));
                current_.setIdx(index_ % (elems_ptr_->config_->getVLEN() / current_.getWidth()));
                return current_;
            }

            pointer operator->() { return &(operator*()); }

            bool operator!=(const IteratorIF & other) const override
            {
                return index_ != other.getIndex()
                       || elems_ptr_ != dynamic_cast<const ElementIterator &>(other).elems_ptr_;
            }

            uint8_t getNextIndex(uint8_t index) const override
            {
                if (elems_ptr_->isMasked())
                {
                    index = elems_ptr_->mask_iter_ptr_->getNextIndex(index);
                }
                return index;
            }

          protected:
            const Elements<ElemType>* elems_ptr_;
            ElemType current_;
        }; // class ElementIterator

        Elements(AtlasState* state, VectorConfig* config, uint32_t reg_id,
                 IteratorIF* mask_iter_ptr) :
            state_(state),
            config_(config),
            start_pos_(config_->getVSTART()),
            end_pos_(config_->getVL()),
            reg_id_(reg_id),
            mask_iter_ptr_(mask_iter_ptr)
        {
        }

        inline bool isMasked() const { return mask_iter_ptr_ != nullptr; }

        ElementIterator begin() { return ElementIterator(this, start_pos_); }

        ElementIterator end() { return ElementIterator(this, end_pos_); }

      protected:
        AtlasState* state_;
        const VectorConfig* config_;
        uint8_t start_pos_;
        uint8_t end_pos_;
        uint32_t reg_id_;
        IteratorIF* mask_iter_ptr_;

        friend class ElementIterator;
    }; // class Elements

    class MaskElements : public Elements<Element<VLEN_MIN>>
    {
      public:
        class MaskElementIterator : public Elements::ElementIterator
        {
          public:
            MaskElementIterator(const MaskElements* elems_ptr, uint8_t index) :
                Elements::ElementIterator(elems_ptr, index)
            {
            }

            reference operator*() override
            {
                const MaskElements* elems_ptr = static_cast<const MaskElements*>(
                    elems_ptr_); // To gain access to *elems_ptr_* members.
                current_.setState(elems_ptr->state_);
                current_.setRegId(elems_ptr->reg_id_);
                current_.setIdx(index_);
                // Fix *start_pos_* if first element.
                if (index_ == elems_ptr->start_pos_)
                {
                    current_.setStartPos(elems_ptr->config_->getVSTART() % VLEN_MIN);
                }
                else
                {
                    current_.setStartPos(0);
                }
                // Fix *end_pos_* if last element.
                if (index_ == elems_ptr->end_pos_ - 1)
                {
                    uint8_t vl = elems_ptr->config_->getVL();
                    if (vl && vl % VLEN_MIN == 0)
                    {
                        current_.setEndPos(VLEN_MIN);
                    }
                    else
                    {
                        current_.setEndPos(vl % VLEN_MIN);
                    }
                }
                else
                {
                    current_.setEndPos(VLEN_MIN);
                }
                return current_;
            }
        }; // class MaskElementIterator

        class MaskBitIterator : public IteratorIF
        {
          public:
            using value_type = uint8_t;

            MaskBitIterator(const MaskElements* elems_ptr, uint8_t index) :
                IteratorIF(index),
                elems_ptr_(elems_ptr)
            {
                index_ = getNextIndex(index_);
            }

            bool operator!=(const IteratorIF & other) const override
            {
                return index_ != other.getIndex()
                       || elems_ptr_ != dynamic_cast<const MaskBitIterator &>(other).elems_ptr_;
            }

            uint8_t getNextIndex(uint8_t index) const override
            {
                const uint8_t vl = elems_ptr_->config_->getVL();
                const uint8_t elem_offset = index / VLEN_MIN;
                if (index >= vl)
                {
                    return vl;
                }
                // TODO: Each bit requires a dmiRead of the Element. This is inefficient.
                for (auto mask_elem_iter = MaskElementIterator{elems_ptr_, elem_offset};
                     mask_elem_iter != MaskElementIterator{elems_ptr_, elems_ptr_->end_pos_};
                     ++mask_elem_iter)
                {
                    auto mask_elem = *mask_elem_iter;
                    mask_elem.getVal();
                    // If we don't have a hit in the first Element for this index, then we shall
                    // start from bit 0 of next Element.
                    const uint8_t bit_offset =
                        mask_elem_iter.getIndex() == elem_offset ? index % VLEN_MIN : 0;
                    for (auto bit_iter = Element<VLEN_MIN>::BitIterator{&mask_elem, bit_offset};
                         bit_iter != mask_elem.end(); ++bit_iter)
                    {
                        return mask_elem_iter.getIndex() * VLEN_MIN + bit_iter.getIndex();
                    }
                }
                return vl;
            }

          private:
            const MaskElements* elems_ptr_;
        }; // class MaskBitIterator

        MaskElements(AtlasState* state, VectorConfig* config, uint32_t reg_id) :
            Elements(state, config, reg_id, nullptr)
        {
            start_pos_ = config_->getVSTART() / VLEN_MIN;
            end_pos_ = (config_->getVL() + VLEN_MIN - 1) / VLEN_MIN;
        }

        friend class MaskElementIterator;
        friend class MaskBitIterator;
    }; // class MaskElements
} // namespace atlas