#pragma once

#include "include/PegasusTypes.hpp"
#include <vector>

namespace pegasus::cosim
{
    /**
     * \class CoSim
     *
     *
     */
    class MemoryInterface
    {
      public:
        //! Allow derivation
        virtual ~MemoryInterface() {}

        virtual bool peek(HartId hart, Addr paddr, size_t size,
                          std::vector<uint8_t> & buffer) const = 0;
        virtual bool read(HartId hart, Addr paddr, size_t size,
                          std::vector<uint8_t> & buffer) const = 0;
        virtual bool poke(HartId hart, Addr paddr, std::vector<uint8_t> & buffer) const = 0;
        virtual bool write(HartId hart, Addr paddr, std::vector<uint8_t> & buffer) const = 0;
    };
} // namespace pegasus::cosim
