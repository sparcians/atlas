//
// Created by skhan on 11/20/24.
//

#include "../include/AtlasTypes.hpp"
#include "PageTableWalker.hpp"
#include "PageTableEntry.hpp"

namespace atlas
{

    uint32_t PageTableWalker::sv32PageTableWalk(uint32_t virtAddr, uint32_t satpRegVal,
                                                AtlasState* state)
    {
        uint32_t PTEaddresss, ptbaseAddress;
        const uint32_t PTE_SIZE = sizeof(RV32);
        const uint32_t offset = extractOffset(virtAddr, MMUMode::SV32);
        const std::vector<uint32_t> vpnValues = extractVPN(virtAddr, MMUMode::SV32);
        // TODO: getBaseAddrFromSatpReg();
        int vpnVectorIndex = vpnValues.size() - 1;
        ptbaseAddress = satpRegVal;
        while (vpnVectorIndex > (-1))
        {
            PTEaddresss = ptbaseAddress + vpnValues[vpnVectorIndex] * PTE_SIZE;
            ptbaseAddress = getPFN(PTEaddresss, state);
            vpnVectorIndex--;
        }
        const uint32_t PhyMemFrameAddress =
            ptbaseAddress
            + offset * PTE_SIZE; // ptbaseAddress points to the Physical Memory base Address
        return PhyMemFrameAddress;
    }

    // convert this to template method, make uint32_t as template
    uint32_t PageTableWalker::getPFN(const uint32_t entryValues, AtlasState* state)
    {
        const uint64_t pteValueFromMem = state->readMemory<uint64_t>(entryValues);
        PageTableEntry<MMUMode::SV32> entry(pteValueFromMem);
        // TODO:
        //  If accessing pte violates a PMA or PMP check, raise an access-fault exception
        //  corresponding to the original access type
        if (!entry.isValid() || ((!entry.canRead()) && entry.canWrite()))
        {
            throw std::runtime_error("page-fault exception");
        }
        // TODO: maybe this should be in PageTableWalk method itself, as this should be controlling
        // if next level is phyMem or next level of PT
        //  If ((!(pte.r==1)) || (!(pte.x==1)))
        //      PTE points to next level pageTable
        return (pteValueFromMem >> 10) << 10;
    }
} // namespace atlas