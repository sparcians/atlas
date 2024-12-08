//
// Created by skhan on 11/20/24.
//

#include "../include/AtlasTypes.hpp"

namespace atlas
{
    PageTableWalker::PageTableWalker() {}

    uint32_t PageTableWalker::sv32PageTableWalk(uint32_t virtAddr, uint32_t satpRegVal,
                                                AtlasState* state)
    {
        Va32 va(virtAddr);
        // TODO: getBaseAddrFromSatpReg();
        uint32_t PDEaddresss = satpRegVal + va.vpn1() * PTE_SIZE;
        uint32_t ptbaseAddress = getPFN(PDEaddresss, state);
        uint32_t pteAddress = ptbaseAddress + va.vpn0() * PTE_SIZE;
        uint32_t PhyMembaseAddress = getPFN(pteAddress, state);
        uint32_t PhyMemFrameAddress = PhyMembaseAddress + va.offset() * PTE_SIZE;
        return PhyMemFrameAddress;
    }

    // convert this to template method, make uint32_t as template
    uint32_t PageTableWalker::getPFN(uint32_t entryValues, AtlasState* state)
    {
        uint64_t pteValueFromMem = state->readMemory<uint64_t>(entryValues);
        PageTableEntry<MMUMode::SV32> entry(pteValueFromMem);
        // TODO:
        //  If accessing pte violates a PMA or PMP check, raise an access-fault exception
        //  corresponding to the original access type
        if (!entry.isValid() || ((!entry.canRead()) && entry.canWrite()))
        {
            std::cout << "throwing page-fault exception" << std::endl;
            throw std::runtime_error("page-fault exception");
        }
        // TODO: maybe this should be in PageTableWalk method itself, as this should be controlling
        // if next level is phyMem or next level of PT
        //  If ((!(pte.r==1)) || (!(pte.x==1)))
        //      PTE points to next level pageTable
        return (pteValueFromMem >> 10) << 10;
    }
} // namespace atlas