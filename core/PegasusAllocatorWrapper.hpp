#pragma once

#include "sparta/utils/SpartaSharedPointerAllocator.hpp"

namespace pegasus
{
    template <typename AllocatorT> class PegasusInstAllocatorWrapper
    {
      public:
        ~PegasusInstAllocatorWrapper()
        {
            std::cout << "PegasusInstAllocatorWrapper: " << allocator_.getNumAllocated()
                      << " objects allocated/created" << std::endl;
        }

        explicit PegasusInstAllocatorWrapper(AllocatorT & allocator) : allocator_(allocator) {}

        // Used by Mavis for the type allocated
        using InstTypePtr = sparta::SpartaSharedPointer<typename AllocatorT::element_type>;

        // Called by Mavis when creating a new instruction
        template <typename... Args> InstTypePtr operator()(Args &&... args)
        {
            return sparta::allocate_sparta_shared_pointer<typename AllocatorT::element_type>(
                allocator_, std::forward<Args>(args)...);
        }

      private:
        AllocatorT & allocator_;
    };

    template <typename AllocatorT> class PegasusExtractorAllocatorWrapper
    {
      public:
        ~PegasusExtractorAllocatorWrapper()
        {
            std::cout << "PegasusExtractorAllocatorWrapper: " << allocator_.getNumAllocated()
                      << " objects allocated/created" << std::endl;
        }

        PegasusExtractorAllocatorWrapper(AllocatorT & allocator, PegasusState* state) :
            allocator_(allocator),
            state_(state)
        {
        }

        // Used by Mavis for the type allocated
        using InstTypePtr = sparta::SpartaSharedPointer<typename AllocatorT::element_type>;

        // Called by Mavis when creating a new instruction
        template <typename... Args> InstTypePtr operator()(Args &&... args)
        {
            return sparta::allocate_sparta_shared_pointer<typename AllocatorT::element_type>(
                allocator_, std::forward<Args>(args)..., state_);
        }

      private:
        AllocatorT & allocator_;
        PegasusState* state_ = nullptr;
    };
} // namespace pegasus
