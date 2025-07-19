#pragma once

#include "include/PegasusTypes.hpp"
#include "core/translate/PageTableEntry.hpp"

#include <map>
#include <iostream>

namespace pegasus
{

    uint32_t determineMaxNumPteEntries(MMUMode mode)
    {
        if (mode == MMUMode::SV32)
        {
            return 1024;
        }
        else if (mode == MMUMode::SV39)
        {
            return 512;
        }
        else
        {
            sparta_assert(false, "Unsupported MMU Mode!");
        }
    }

    template <typename XLEN, MMUMode Mode> class PageTable
    {
      public:
        using PageTableType = std::map<uint32_t, PageTableEntry<XLEN, Mode>>;

        PageTable(uint32_t base_addr) :
            max_entries_(determineMaxNumPteEntries(Mode)),
            base_addr_(base_addr)
        {
        }

        const PageTableType & getPageTable() const { return page_table_; }

        XLEN getBaseAddr() const { return base_addr_; }

        XLEN getAddrOfIndex(const uint32_t idx) const
        {
            sparta_assert(isValidIndex_(idx), "Invalid index: " << idx);
            return base_addr_ + idx * sizeof(XLEN);
        }

        uint32_t getIndexOfAddr(const XLEN addr) const
        {
            sparta_assert(addr % sizeof(XLEN) == 0, "Address is not aligned: 0x" << addr);
            const uint32_t idx = (addr - base_addr_) / sizeof(XLEN);
            sparta_assert(isValidIndex_(idx), "Invalid index: " << idx);
            return idx;
        }

        void addEntry(uint32_t idx, PageTableEntry<XLEN, Mode> entry)
        {
            sparta_assert(isValidIndex_(idx), "Invalid index: " << idx);
            auto it = page_table_.find(idx);
            if (it != page_table_.end())
            {
                it->second = entry; // Update the existing entry
            }
            else
            {
                page_table_.insert({idx, entry}); // Insert the new entry
            }
        }

        PageTableEntry<XLEN, Mode> getEntry(uint32_t idx)
        {
            sparta_assert(isValidIndex_(idx), "Invalid index: " << idx);
            auto it = page_table_.find(idx);
            if (it != page_table_.end())
            {
                return it->second;
            }
            else
            {
                sparta_assert(false, "Index not found in the page table");
            }
        }

        PageTableEntry<XLEN, Mode> getEntryAtAddr(XLEN addr)
        {
            const uint32_t idx = getIndexOfAddr(addr);
            return getEntry(idx);
        }

        void removeEntry(uint32_t idx)
        {
            sparta_assert(isValidIndex_(idx), "Invalid index: " << idx);
            auto it = page_table_.find(idx);
            if (it != page_table_.end())
            {
                page_table_.erase(it);
            }
            else
            {
                sparta_assert(false, "Index not found in the page table");
            }
        }

        bool contains(uint32_t idx) const { return page_table_.find(idx) != page_table_.end(); }

      private:
        PageTableType page_table_;
        const uint32_t max_entries_;
        const uint32_t base_addr_;

        bool isValidIndex_(uint32_t idx) const { return idx < max_entries_; }
    };

    template <typename XLEN, MMUMode Mode>
    std::ostream & operator<<(std::ostream & os, const PageTable<XLEN, Mode> & pt)
    {
        for (const auto & [idx, entry] : pt.getPageTable())
        {
            os << "Index: " << idx << " | PTE: " << entry << std::endl;
        }
        return os;
    }

} // namespace pegasus
