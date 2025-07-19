#pragma once

#include <cassert>

#include "include/PegasusTypes.hpp"

namespace pegasus
{
    class PegasusTranslationState
    {
      public:
        static const uint32_t MAX_TRANSLATION = 64;

        struct TranslationRequest
        {
          public:
            TranslationRequest() = default;

            TranslationRequest(Addr vaddr, size_t size) : vaddr_(vaddr), size_(size) {}

            Addr getVAddr() const { return vaddr_; }

            size_t getSize() const { return size_; }

            bool isValid() const { return size_ != 0; }

            void setMisaligned(const size_t misaligned_bytes)
            {
                sparta_assert(misaligned_bytes > 0);
                sparta_assert(misaligned_bytes < size_);
                misaligned_bytes_ = misaligned_bytes;
                misaligned_ = true;
            }

            bool isMisaligned() const { return misaligned_; }

            size_t getMisalignedBytes() const { return misaligned_bytes_; }

          private:
            Addr vaddr_ = 0;
            size_t size_ = 0;
            bool misaligned_ = false;
            size_t misaligned_bytes_ = 0;
        };

        struct TranslationResult
        {
          public:
            TranslationResult() = default;

            TranslationResult(Addr vaddr, Addr paddr, size_t sz) :
                vaddr_(vaddr),
                paddr_(paddr),
                size_(sz)
            {
            }

            // Get the original VAddr
            Addr getVAddr() const { return vaddr_; }

            Addr getPAddr() const { return paddr_; }

            size_t getSize() const { return size_; }

            bool isValid() const { return size_ != 0; }

          private:
            Addr vaddr_ = 0;
            Addr paddr_ = 0;
            size_t size_ = 0;
        };

        void makeRequest(const Addr vaddr, const size_t size)
        {
            sparta_assert(size > 0);
            sparta_assert(results_cnt_ == 0);
            sparta_assert(requests_cnt_ < requests_.size());
            requests_[requests_cnt_++] = {vaddr, size};
        }

        uint32_t getNumRequests() const { return requests_cnt_; }

        TranslationRequest & getRequest()
        {
            sparta_assert(requests_cnt_ > 0);
            return requests_[requests_cnt_ - 1];
        }

        void popRequest()
        {
            sparta_assert(requests_cnt_ > 0);
            --requests_cnt_;
        }

        void setResult(const Addr vaddr, const Addr paddr, const size_t size)
        {
            sparta_assert(results_cnt_ < results_.size());
            results_[results_cnt_++] = {vaddr, paddr, size};
        }

        uint32_t getNumResults() const { return results_cnt_; }

        const TranslationResult & getResult() const
        {
            sparta_assert(results_cnt_ > 0);
            return results_[results_cnt_ - 1];
        }

        void popResult()
        {
            sparta_assert(results_cnt_ > 0);
            --results_cnt_;
        }

        void reset()
        {
            requests_cnt_ = 0;
            results_cnt_ = 0;
        }

      private:
        // Translation request
        std::array<TranslationRequest, MAX_TRANSLATION> requests_;
        uint32_t requests_cnt_ = 0;

        // Translation result
        std::array<TranslationResult, MAX_TRANSLATION> results_;
        uint32_t results_cnt_ = 0;
    };
} // namespace pegasus
