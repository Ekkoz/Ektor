// MIT License
// 
// Copyright (c) 2018 EkkoZ
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 

#include "Ek/Memory/FrameAllocator.hpp"
#include "Ek/Utils/Maths.hpp"
#include "Ek/Utils/Logger.hpp"

namespace ek
{
  FrameAllocator::FrameAllocator(std::uint64_t pageSize, std::uint64_t slotSize) :
    _headerPageSize(ALIGN(sizeof(t_frame_page), DEFAULT_ALIGN_SIZE)),
    _headerSlotSize(ALIGN(sizeof(t_frame_slot), DEFAULT_ALIGN_SIZE)),
    _slotSize(MAX(ALIGN(slotSize, DEFAULT_ALIGN_SIZE), _headerSlotSize)),
    _pageSize(ALIGN(pageSize, DEFAULT_ALIGN_SIZE))
  {
    DEBUG("FrameAllocator: Constructor");
    this->_currentPage = (t_frame_page *) this->_systemAlloc(this->_pageSize);
    this->_currentPage->next = nullptr;
    this->_currentSlot = (t_frame_slot *) (((char *) this->_currentPage) + this->_headerPageSize);
    this->_currentSlot->size = (this->_pageSize - this->_headerPageSize) / this->_slotSize;
    this->_currentSlot->next = nullptr;
  }

  FrameAllocator::~FrameAllocator()
  {
    t_frame_page *page;

    DEBUG("FrameAllocator: Destructor");
    while (this->_currentPage)
    {
      page = this->_currentPage->next;
      this->_systemFree(this->_currentPage);
      this->_currentPage = page;
    }
  }

  void *FrameAllocator::_systemAlloc(std::uint64_t const size)
  {
    DEBUG("FrameAllocator: malloc(" << size << ")");
    return (std::malloc(size));
  }

  void FrameAllocator::_systemFree(void *ptr)
  {
    DEBUG("FrameAllocator: free(0x" << ptr << ")");
    std::free(ptr);
  }

  void FrameAllocator::_pageAlloc()
  {
    t_frame_page *page;

    page = (t_frame_page *) this->_systemAlloc(this->_pageSize);
    page->next = this->_currentPage;
    this->_currentPage = page;
    this->_currentSlot = (t_frame_slot *) (((char *) this->_currentPage) + this->_headerPageSize);
    this->_currentSlot->size = (this->_pageSize - this->_headerPageSize) / this->_slotSize;
    this->_currentSlot->next = nullptr;
  }

  void *FrameAllocator::_slotAlloc()
  {
    t_frame_slot *current;
    t_frame_slot *next;

    current = this->_currentSlot;
    if (current->size > 1)
    {
      next = (t_frame_slot *) (((char *) current) + this->_slotSize);
      next->size = current->size - 1;
      next->next = current->next;
      this->_currentSlot = next;
    }
    else
      this->_currentSlot = current->next;
    return ((void *) current);
  }

  void *FrameAllocator::allocate(std::uint64_t const size) throw()
  {
    void *ptr = nullptr;

    DEBUG("FrameAllocator: Allocate");
    if (size > this->_slotSize)
    {
      ERROR("FrameAllocator: A frame of " << size << " bytes has been asked; max frame size available: " << this->_slotSize << " bytes.");
      throw std::bad_alloc();
    }
    if (!this->_currentSlot)
      this->_pageAlloc();
    ptr = this->_slotAlloc();
    return (ptr);
  }

  void FrameAllocator::free(void *ptr)
  {
    t_frame_slot *slot;

    DEBUG("FrameAllocator: Free");
    slot = (t_frame_slot *) ptr;
    slot->size = 1;
    slot->next = this->_currentSlot;
    this->_currentSlot = slot;
  }
};