// <PegasusAllocators.hpp> -*- C++ -*-

#pragma once

/*!
 * \file PegasusAllocators.hpp
 * \brief Defines a general TreeNode that contains all allocators used
 *        in simulation
 */

#include "sparta/simulation/TreeNode.hpp"

#include "core/PegasusInst.hpp"
#include "core/PegasusExtractor.hpp"

namespace pegasus
{
    /*!
     * \class PegasusAllocators
     * \brief A TreeNode that is actually a functional resource
     *        containing memory allocators
     */
    class PegasusAllocators : public sparta::TreeNode
    {
      public:
        static constexpr char name[] = "allocators";

        PegasusAllocators(sparta::TreeNode* node) :
            sparta::TreeNode(node, name, "Allocators used in simulation")
        {
        }

        static PegasusAllocators* getAllocators(sparta::TreeNode* node)
        {
            PegasusAllocators* allocators = nullptr;
            if (node)
            {
                if (node->hasChild(PegasusAllocators::name))
                {
                    allocators = node->getChildAs<PegasusAllocators>(PegasusAllocators::name);
                }
                else
                {
                    return getAllocators(node->getParent());
                }
            }
            return allocators;
        }

        PegasusInstAllocator inst_allocator{2000, 1500};
        PegasusExtractorAllocator extractor_allocator{10000, 8500};
    };
} // namespace pegasus
