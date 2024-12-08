#include <iostream>
#include <stdexcept>

namespace atlas{
    // constructor that sets the maximum number of entries in the page table

    template<MMUMode Mode>
    uint16_t PageTable<Mode>::getMaxNumEntries(MMUMode mode) const
    {
        if(mode == MMUMode::SV32)
            return PT_ENTRIES_SV32;
        else if (mode == MMUMode::SV39)
            return PT_ENTRIES_SV39;
        return 0;
    }

    template<MMUMode Mode>
    PageTable<Mode>::PageTable(uint32_t baseAddr) : 
        max_entries(getMaxNumEntries(Mode)) , baseAddressOfpageTable(baseAddr)
    {}

    template <> void PageTable<MMUMode::SV32>::addEntry(uint32_t index, PageTableEntry<MMUMode::SV32> entry) {
        if (size() <= max_entries && isValidIndex(index)) {
            auto it = pageTable.find(index);
            if (it != pageTable.end()) {
                // Update the existing entry
                it->second = entry;
            } else {
                // Insert the new entry
                pageTable.insert({index, entry});
            }
        } else {
            throw std::runtime_error("Page table has reached its maximum capacity/PageTable index out of bound!");
        }
    }

    // template <> void PageTable<MMUMode::SV39>::addEntry(uint32_t index, PageTableEntry<MMUMode::SV39> entry) {
    //     if (size() < max_entries && isValidIndex(index)) {
    //         if(pageTable.contains(index))
    //             pageTable[index] = entry;
    //     } else {
    //         throw std::runtime_error("Page table has reached its maximum capacity/PageTable index out of bound!");
    //     }
    // }

    template <MMUMode Mode> bool PageTable<Mode>::isValidIndex(uint32_t idx){
        return ((idx >= baseAddressOfpageTable) && (idx < ((max_entries * PTE_SIZE) + baseAddressOfpageTable)));
    }


    // Method to get an entry from the page table at a given index
    template <MMUMode Mode> PageTableEntry<Mode> PageTable<Mode>::getEntry(uint32_t index) {
        if(isValidIndex(index)){
            auto it = pageTable.find(index);
            if (it != pageTable.end()) {
                return it->second;
            } else {
                throw std::out_of_range("Index not found in the page table");
            }
        }
        else
            throw std::out_of_range("Index Invalid");
    }

    // Method to remove an entry from the page table at a given index
    template <MMUMode Mode> void PageTable<Mode>::removeEntry(uint32_t index) {
        if(isValidIndex(index)){
            auto it = pageTable.find(index);
            if (it != pageTable.end()) {
                pageTable.erase(it);
            } else {
                throw std::out_of_range("Index not found in the page table");
            }
        } else{
            throw std::out_of_range("Index Invalid");
        }
    }

    // Method to check if an entry exists at a given index
    template <MMUMode Mode> bool PageTable<Mode>::contains(uint32_t index)  {
        return pageTable.find(index) != pageTable.end();
    }

    // Method to get the current number of entries in the page table
    template <MMUMode Mode> size_t PageTable<Mode>::size()  {
        return pageTable.size();
    }

    // // Method to print all entries in the page table
    // template <MMUMode Mode> void PageTable<Mode>::print()  {
    //     for ( auto& [index, entry] : pageTable) {
    //         std::cout << "Index: " << index << " | PTE: " << entry.getPTE() << std::endl;
    //     }
    // }

    template <MMUMode Mode>
    void PageTable<Mode>::print(std::ostream& os) {
        for (const auto& [index, entry] : pageTable) {
            os << "Index: " << index << " | PTE: " << entry.getPTE() << std::endl;
        }
    }

    template <MMUMode Mode>
    std::ostream & operator<<(std::ostream & os, const PageTable<Mode>& pt)
    {
        pt.print(os);
        return os;
    }
}
