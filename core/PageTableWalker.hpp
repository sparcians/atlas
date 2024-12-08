//
// Created by skhan on 11/20/24.
//
namespace atlas
{

    struct Va32Bits
    {
        unsigned offset_ : 12;
        unsigned vpn0_ : 10;
        unsigned vpn1_ : 10;
    } __attribute__((packed));

    union Va32
    {
        Va32Bits bits_;
        uint32_t data_ = 0;

        Va32(uint32_t word) : data_(word) {}

        uint32_t offset() const { return bits_.offset_; }

        uint32_t vpn0() const { return bits_.vpn0_; }

        uint32_t vpn1() const { return bits_.vpn1_; }

        uint32_t vpn(int i) const
        {
            if (i == 0)
                return vpn0();
            if (i == 1)
                return vpn1();
            //   assert(0);
            return 0;
        }
    };

    struct Va39Bits
    {
        unsigned offset_ : 12;
        unsigned vpn0_ : 9;
        unsigned vpn1_ : 9;
        unsigned vpn2_ : 9;
    } __attribute__((packed));

    /// 39-bit virtual address.
    union Va39
    {
        Va39Bits bits_;
        uint64_t data_ = 0;

        Va39(uint64_t data) : data_(data) {}

        uint64_t offset() const { return bits_.offset_; }

        uint64_t vpn0() const { return bits_.vpn0_; }

        uint64_t vpn1() const { return bits_.vpn1_; }

        uint64_t vpn2() const { return bits_.vpn2_; }

        uint64_t vpn(int i) const
        {
            if (i == 0)
                return vpn0();
            if (i == 1)
                return vpn1();
            if (i == 2)
                return vpn2();
            //   assert(0);
            return 0;
        }
    };

    /// Structure to unpack the fields of Sv48 virtual address.
    struct Va48Bits
    {
        unsigned offset_ : 12;
        unsigned vpn0_ : 9;
        unsigned vpn1_ : 9;
        unsigned vpn2_ : 9;
        unsigned vpn3_ : 9;
    } __attribute__((packed));

    /// 48-bit virtual address.
    union Va48
    {
        Va48Bits bits_;
        uint64_t data_ = 0;

        Va48(uint64_t data) : data_(data) {}

        uint64_t offset() const { return bits_.offset_; }

        uint64_t vpn0() const { return bits_.vpn0_; }

        uint64_t vpn1() const { return bits_.vpn1_; }

        uint64_t vpn2() const { return bits_.vpn2_; }

        uint64_t vpn3() const { return bits_.vpn2_; }

        uint64_t vpn(int i) const
        {
            if (i == 0)
                return vpn0();
            if (i == 1)
                return vpn1();
            if (i == 2)
                return vpn2();
            if (i == 3)
                return vpn3();
            //   assert(0);
            return 0;
        }
    };

    class PageTableWalker
    {

      public:
        PageTableWalker();
        uint32_t getPFN(uint32_t, AtlasState*);
        uint32_t sv32PageTableWalk(uint32_t, uint32_t, AtlasState*);
    };

} // namespace atlas

#include "PageTableWalker.cpp"