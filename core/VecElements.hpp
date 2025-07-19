#pragma once

#include <stdint.h>

#include "core/PegasusState.hpp"
#include "core/VecConfig.hpp"
#include "core/VecNums.hpp"
#include "include/PegasusUtils.hpp"

namespace pegasus
{
    /**
     * @class Element
     * @brief A proxy representation for vector element of native integer type size.
     *
     * A local copy of element bits is stored. Neccessary information
     * to extract bits from vector register is kept, e.g. reg_id and idx.
     * It also tracks start and end position of the bits, so that only active
     * bits in Body part is accessible.
     * Bit Iterator is provided for bit iteration.
     *
     * @tparam elemWidth Number of bits for this *Element* type.
     */
    template <size_t elemWidth> struct Element
    {
      public:
        using ValueType = UintType<elemWidth>;
        static constexpr size_t elem_width = elemWidth;

        /**
         * @class BitIterator
         * @brief Used for bit iteration of *Element*.
         */
        class BitIterator
        {
          public:
            using value_type = uint8_t;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;

            /**
             * @brief Constructor. When called, *index_* is adjucted to first active bit.
             * @param elem_ptr Poninter to *Element* object upon which this iterator operates.
             * @param index *index_* will be set to the first active bit after this (inclusive).
             */
            BitIterator(const Element<elemWidth>* elem_ptr, size_t index) : elem_ptr_(elem_ptr)
            {
                index_ = getNextIndex(index);
            }

            /**
             * @brief Pre-increment operator. *index_* will be set to the next active bit.
             * @return Reference to itself.
             */
            BitIterator & operator++()
            {
                index_ = this->getNextIndex(++index_);
                return *this;
            }

            /**
             * @brief Equal operator.
             * @return *true* if iterators point to the same *Element* and bit position.
             */
            bool operator==(const BitIterator & other) const
            {
                return index_ == other.index_ && elem_ptr_ == other.elem_ptr_;
            }

            /**
             * @brief Not equal operator.
             * @return *true* if not equal.
             */
            bool operator!=(const BitIterator & other) const { return !(*this == other); }

            /**
             * @brief Return current *index_*.
             * @return Current *index_*.
             */
            size_t getIndex() const { return index_; }

            /**
             * @brief Set *index_* to the next active bit.
             * @param index *index_* will be set to the next active bit after this (inclusive).
             * @return Index of next active bit.
             */
            size_t getNextIndex(size_t index) const
            {
                const size_t end_pos = elem_ptr_->getEndPos();
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
            /**< Pointer to *Element* object upon which this iterator operates. */
            const Element<elemWidth>* elem_ptr_ = nullptr;
            /**< Index tracking current pointed bit. */
            size_t index_ = 0;
        }; // class BitIterator

        /**
         * @brief Default Constructor.
         */
        Element() = default;

        /**
         * @brief Constructor.
         *
         * @param state Poninter of *PegasusState* object.
         * @param reg_id vector register ID.
         * @param idx Index offset in the Register
         */
        Element(PegasusState* state, uint32_t reg_id, uint32_t idx) :
            state_(state),
            reg_id_(reg_id),
            idx_(idx)
        {
        }

        /**
         * @brief Set *state_*.
         */
        void setState(PegasusState* state) { state_ = state; }

        /**
         * @brief Set *reg_id_*.
         */
        void setRegId(uint32_t reg_id) { reg_id_ = reg_id; }

        /**
         * @brief Set *idx_*.
         */
        void setIdx(uint32_t idx) { idx_ = idx; }

        /**
         * @brief Get *start_pos_*.
         */
        size_t getStartPos() const { return start_pos_; }

        /**
         * @brief Set *start_pos_*.
         */
        void setStartPos(size_t start_pos) { start_pos_ = start_pos; }

        /**
         * @brief Get *end_pos_*.
         */
        size_t getEndPos() const { return end_pos_; }

        /**
         * @brief Set *end_pos_*.
         */
        void setEndPos(size_t end_pos) { end_pos_ = end_pos; }

        /**
         * @brief Generate active bit mask based on start and end position.
         * @return Bit mask represented in native integer type.
         */
        ValueType getMask() const
        {
            return ((ValueType{1} << (end_pos_ - start_pos_)) - 1) << start_pos_;
        }

        /**
         * @brief Get the active local element bits without retrieving from vector register.
         * @return Local element bits in native integer type.
         */
        ValueType peekVal() const { return val_; }

        /**
         * @brief Set the active local element bits without updating vector register.
         * @param value Value to set to.
         */
        void pokeVal(ValueType value) { val_ = value & getMask(); }

        /**
         * @brief Retrieve the active element bits from vector register.
         * @return Element bits in native integer type.
         */
        ValueType getVal()
        {
            val_ = READ_VEC_ELEM<ValueType>(state_, reg_id_, idx_) & getMask();
            return val_;
        }

        /**
         * @brief Update active element bits in vector register.
         * @param value Value to set to.
         */
        void setVal(ValueType value)
        {
            const ValueType mask = getMask();
            val_ = value & mask;
            ValueType val = READ_VEC_ELEM<ValueType>(state_, reg_id_, idx_) & ~mask;
            WRITE_VEC_ELEM<ValueType>(state_, reg_id_, val | val_, idx_);
        }

        /**
         * @brief Retrieve bit of *Element* at *index* from vector register.
         * @param index The index of requested bit.
         * @return Bit value at *index*.
         */
        ValueType getBit(size_t index) const
        {
            return (READ_VEC_ELEM<ValueType>(state_, reg_id_, idx_) & (ValueType{1} << index)
                    & getMask())
                   >> index;
        }

        /**
         * @brief Update bit of *Element* at *index* in vector register to 1.
         * @param index The index of requested bit.
         */
        void setBit(size_t index)
        {
            ValueType val = READ_VEC_ELEM<ValueType>(state_, reg_id_, idx_);
            const ValueType mask = getMask();
            ValueType bitmask = (ValueType{1} << index) & mask;
            val |= bitmask;
            val_ = val & mask;
            WRITE_VEC_ELEM<ValueType>(state_, reg_id_, val, idx_);
        }

        /**
         * @brief Update bit of *Element* at *index* in vector register to 0.
         * @param index The index of requested bit.
         */
        void clearBit(size_t index)
        {
            ValueType val = READ_VEC_ELEM<ValueType>(state_, reg_id_, idx_);
            const ValueType mask = getMask();
            ValueType bitmask = (ValueType{1} << index) & mask;
            val &= ~bitmask;
            val_ = val & mask;
            WRITE_VEC_ELEM<ValueType>(state_, reg_id_, val, idx_);
        }

        /**
         * @brief Return *begin* iterator object.
         * @return *BitIterator* object pointitng to the start position.
         */
        BitIterator begin() { return BitIterator(this, start_pos_); }

        /**
         * @brief Return *end* iterator object.
         * @return *BitIterator* object pointitng to the end position.
         */
        BitIterator end() { return BitIterator(this, end_pos_); }

      private:
        /**< Needed for vector register access. */
        PegasusState* state_ = nullptr;
        /**< vector register ID. */
        uint32_t reg_id_ = 0;
        /**< Index offset for vector register. */
        uint32_t idx_ = 0;
        /**< Start position of active bits. */
        size_t start_pos_ = 0;
        /**< End position of active bits. */
        size_t end_pos_ = elemWidth;
        /**< Local bits of vector element. */
        ValueType val_ = 0;
    }; // struct Element

    template <bool isMaskElems>
    concept EnableIf = isMaskElems;

    /**
     * @class Elements
     * @brief A proxy representation for vector register group.
     *
     * This class focuses on efficient native integer type representation of
     * both vector register group and vector mask register.
     * Information to correctly represent vector register group is kept,
     * e.g. reg_id and *VectorConfig*, which includes LMUL, SEW, VLEN ... etc.
     * It also tracks start and end position of the active element based on
     * VSTART and VL from *VectorConfig*. Depending on what it's representing
     * (vector register groups or vector mask), start and end position are
     * calculated differently.
     * When representing vector mask register, it is in view of mutiple chunks
     * of native integer size (*Element*) instead of bit set. In this way
     * bit-wise operation of vector mask registers can be more efficient
     * performed.
     * Element iterator is provided for element iteration; bit iterator for
     * vector mask regitster bit iteration.
     *
     * @tparam ElemType Element type for individual element.
     * @tparam isMaskElems true if this *Elements* represents vector mask.
     */
    template <typename EType, bool isMaskElems> class Elements
    {
      public:
        using ElemType = EType;

        /**
         * @class ElementIterator
         * @brief Used for *Element* iteration of *Elements*.
         */
        class ElementIterator
        {
          public:
            using value_type = ElemType;
            using pointer = ElemType*;
            using reference = ElemType &;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;

            /**
             * @brief Constructor. When called, *index_* is adjucted to first active *Element*.
             * @param elems_ptr Poninter to *Elements* object upon which this iterator operates.
             * @param index *index_* will be set to the first active *Element* after this
             * (inclusive).
             */
            ElementIterator(const Elements* elems_ptr, size_t index) : elems_ptr_(elems_ptr)
            {
                index_ = getNextIndex(index);
            }

            /**
             * @brief Dereference operator.
             * @return A newly constructed *Element* pointed to by current *index_*.
             */
            value_type operator*() { return elems_ptr_->getElement(index_); }

            /**
             * @brief Pointer version of dereference operator.
             * @return A pointer to newly constructed *Element* pointed to by current *index_*.
             */
            pointer operator->() { return &(operator*()); }

            /**
             * @brief Pre-increment operator. *index_* will be set to the next active *Element*.
             * @return Reference to itself.
             */
            ElementIterator & operator++()
            {
                index_ = this->getNextIndex(++index_);
                return *this;
            }

            /**
             * @brief Equal operator.
             * @return *true* if iterators point to the same *Elements* and *Element* position.
             */
            bool operator==(const ElementIterator & other) const
            {
                return index_ == other.index_ && elems_ptr_ == other.elems_ptr_;
            }

            /**
             * @brief Not equal operator.
             * @return *true* if not equal.
             */
            bool operator!=(const ElementIterator & other) const { return !(*this == other); }

            /**
             * @brief Return current *index_*.
             * @return Current *index_*.
             */
            size_t getIndex() const { return index_; }

            /**
             * @brief Set *index_* to the next active *Element*. Only range checks are performed
             * here.
             * @param index *index_* will be set to the next active *Element* after this
             * (inclusive).
             * @return Index of next active *Element*.
             */
            size_t getNextIndex(size_t index) const
            {
                const size_t end_pos = elems_ptr_->end_pos_;
                index = std::max(index, elems_ptr_->start_pos_);
                if (index >= end_pos)
                {
                    return end_pos;
                }
                return index;
            }

          private:
            /**< Pointer to *Elements* object upon which this iterator operates. */
            const Elements* elems_ptr_ = nullptr;
            /**< Index tracking current pointed *Element*. */
            size_t index_ = 0;
        }; // class ElementIterator

        /**
         * @class MaskBitIterator
         * @brief Used for bit iteration of *MastElements*.
         */
        template <bool M = isMaskElems>
        requires EnableIf<M>

        class MaskBitIterator
        {
          public:
            using value_type = uint8_t;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;

            /**
             * @brief Constructor. When called, *index_* is adjucted to first active bit.
             * @param elems_ptr Poninter to *Elements* object upon which this iterator operates.
             * @param index *index_* will be set to the first active bit after this (inclusive).
             */
            MaskBitIterator(const Elements* elems_ptr, size_t index) : elems_ptr_(elems_ptr)
            {
                index_ = getNextIndex(index);
            }

            /**
             * @brief Pre-increment operator. *index_* will be set to the next active bit.
             * @return Reference to itself.
             */
            MaskBitIterator & operator++()
            {
                index_ = this->getNextIndex(++index_);
                return *this;
            }

            /**
             * @brief Equal operator.
             * @return *true* if iterators point to the same *Elements* and bit position.
             */
            bool operator==(const MaskBitIterator & other) const
            {
                return index_ == other.index_ && elems_ptr_ == other.elems_ptr_;
            }

            /**
             * @brief Not equal operator.
             * @return *true* if not equal.
             */
            bool operator!=(const MaskBitIterator & other) const { return !(*this == other); }

            /**
             * @brief Return current *index_*.
             * @return Current *index_*.
             */
            size_t getIndex() const { return index_; }

            /**
             * @brief Set *index_* to the next active bit.
             * @param index *index_* will be set to the next active bit after this (inclusive).
             * @return Index of next active bit.
             */
            size_t getNextIndex(size_t index) const
            {
                const size_t vl = elems_ptr_->config_->getVL();
                const size_t elem_offset = index / ElemType::elem_width;
                index = std::max(index, elems_ptr_->config_->getVSTART());
                if (index >= vl)
                {
                    return vl;
                }
                for (auto mask_elem_iter = ElementIterator{elems_ptr_, elem_offset};
                     mask_elem_iter != ElementIterator{elems_ptr_, elems_ptr_->end_pos_};
                     ++mask_elem_iter)
                {
                    auto mask_elem = *mask_elem_iter;
                    mask_elem.getVal();
                    // If we don't have a hit in the first Element for this index, then we shall
                    // start from bit 0 of next Element.
                    const size_t bit_offset =
                        mask_elem_iter.getIndex() == elem_offset ? index % ElemType::elem_width : 0;
                    for (typename Element<ElemType::elem_width>::BitIterator bit_iter{&mask_elem,
                                                                                      bit_offset};
                         bit_iter != mask_elem.end(); ++bit_iter)
                    {
                        return mask_elem_iter.getIndex() * ElemType::elem_width
                               + bit_iter.getIndex();
                    }
                }
                return vl;
            }

          private:
            /**< Pointer to *Elements* object upon which this iterator operates. */
            const Elements* elems_ptr_ = nullptr;
            /**< Index tracking current pointed bit. */
            size_t index_ = 0;
        }; // class MaskBitIterator

        /**
         * @brief Constructor for vector register group *Elements*.
         *
         * @param state Poninter to *PegasusState* object.
         * @param config Poninter to *VectorConfig* object.
         * @param reg_id vector register ID.
         */
        Elements(PegasusState* state, VectorConfig* config,
                 uint32_t reg_id) requires EnableIf<!isMaskElems> :
            state_(state),
            config_(config),
            start_pos_(config_->getVSTART()),
            end_pos_(config_->getVL()),
            reg_id_(reg_id)
        {
        }

        /**
         * @brief Constructor for vector mask *Elements*.
         *
         * @param state Poninter to *PegasusState* object.
         * @param config Poninter to *VectorConfig* object.
         * @param reg_id vector register ID.
         */
        Elements(PegasusState* state, VectorConfig* config,
                 uint32_t reg_id) requires EnableIf<isMaskElems> :
            state_(state),
            config_(config),
            start_pos_(config_->getVSTART() / ElemType::elem_width),
            end_pos_((config_->getVL() + ElemType::elem_width - 1) / ElemType::elem_width),
            reg_id_(reg_id)
        {
        }

        /**
         * @brief Return *begin* *ElementIterator* object.
         * @return *ElementIterator* object pointitng to the starting *Element*.
         */
        ElementIterator begin() const { return ElementIterator(this, start_pos_); }

        /**
         * @brief Return *end* *ElementIterator* object.
         * @return *ElementIterator* object pointitng to the ending *Element*.
         */
        ElementIterator end() const { return ElementIterator(this, end_pos_); }

        /**
         * @brief Return *begin* *MaskBitIterator* object.
         * @return *MaskBitIterator* object pointitng to the starting bit.
         */
        auto maskBitIterBegin() const requires EnableIf<isMaskElems>
        {
            return MaskBitIterator<>(this, config_->getVSTART());
        }

        /**
         * @brief Return *end* *MaskBitIterator* object.
         * @return *MaskBitIterator* object pointitng to the ending bit.
         */
        auto maskBitIterEnd() const requires EnableIf<isMaskElems>
        {
            return MaskBitIterator<>(this, config_->getVL());
        }

        /**
         * @brief Get *Element* from *Elements* at *index*.
         *
         * *Element* start and end position will be adjusted depending on whether it's
         * vector register group *Element* or vector mask *Element*.
         *
         * @param index The index of requested *Element*.
         * @return A newly constructed *Element* to which *index* points.
         */
        ElemType getElement(size_t index) const
        {
            uint32_t reg_id = reg_id_ + index / (config_->getVLEN() / ElemType::elem_width);
            uint32_t idx = index % (config_->getVLEN() / ElemType::elem_width);
            ElemType elem{state_, reg_id, idx};
            if constexpr (isMaskElems)
            {
                // Fix *start_pos_* if first element.
                if (index == start_pos_)
                {
                    elem.setStartPos(config_->getVSTART() % ElemType::elem_width);
                }
                else
                {
                    elem.setStartPos(0);
                }
                // Fix *end_pos_* if last element.
                if (index == end_pos_ - 1)
                {
                    const size_t vl = config_->getVL();
                    if (vl && vl % ElemType::elem_width == 0)
                    {
                        elem.setEndPos(ElemType::elem_width);
                    }
                    else
                    {
                        elem.setEndPos(vl % ElemType::elem_width);
                    }
                }
                else
                {
                    elem.setEndPos(ElemType::elem_width);
                }
            }
            return elem;
        }

        /**
         * @brief Retrieve bit of *MaskElements* at *index* from vector register.
         * @param index The index of requested bit.
         * @return Bit value at *index*.
         */
        typename ElemType::ValueType getBit(size_t index) const requires EnableIf<isMaskElems>
        {
            auto elem = getElement(index / ElemType::elem_width);
            return elem.getBit(index % ElemType::elem_width);
        }

        /**
         * @brief Update bit of *MaskElements* at *index* in vector register to 1.
         * @param index The index of requested bit.
         */
        void setBit(size_t index) const requires EnableIf<isMaskElems>
        {
            auto elem = getElement(index / ElemType::elem_width);
            return elem.setBit(index % ElemType::elem_width);
        }

        /**
         * @brief Update bit of *MaskElements* at *index* in vector register to 0.
         * @param index The index of requested bit.
         */
        void clearBit(size_t index) const requires EnableIf<isMaskElems>
        {
            auto elem = getElement(index / ElemType::elem_width);
            return elem.clearBit(index % ElemType::elem_width);
        }

      private:
        /**< Pointer to *PegasusState* object. */
        PegasusState* state_ = nullptr;
        /**< Pointer to *VectorConfig* object. */
        const VectorConfig* config_ = nullptr;
        /**< Starting *Element* position. */
        size_t start_pos_ = 0;
        /**< Ending *Element* position. */
        size_t end_pos_ = 0;
        /**< Vector register ID. */
        uint32_t reg_id_ = 0;
    }; // class Elements

    using MaskElement = Element<VLEN_MIN>;
    using MaskElements = Elements<MaskElement, true>;
    using MaskBitIterator = MaskElements::MaskBitIterator<>;
} // namespace pegasus
