#pragma once

#include <stdint.h>

#include "sparta/functional/RegisterBits.hpp"
#include "core/VecConfig.hpp"

namespace pegasus
{
    inline sparta::RegisterBits getVecBitsMask(const VectorConfig* config)
    {
        sparta::RegisterBits mask{config->getVLEN() / 8};
        mask <<= config->getVLEN() - config->getVL();
        mask >>= config->getVLEN() - config->getVL() + config->getVSTART();
        mask <<= config->getVSTART();
        return mask;
    }

    inline size_t getFirstSetBit(const RegsiterBits & bits)
    {
        sparta::RegisterBits mask{config->getVLEN() / 8, 1};
        sparta::RegisterBits tmp{bits};
        size_t i = 0;
        for (; i < tmp.getSize() * 8; ++i)
        {
            if ((tmp & mask).any())
            {
                return i;
            }
            tmp >>= 1;
        }
        return i;
    }
} // namespace pegasus
