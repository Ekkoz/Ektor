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

#include "Ek/Memory/StackAllocator.hpp"
#include "Ek/Utils/Logger.hpp"

namespace ek
{
  StackAllocator::StackAllocator(std::uint64_t pageSize) :
    _pageSize(pageSize),
    _pageHeaderSize(ALIGN(sizeof(t_stack_page), DEFAULT_ALIGN_SIZE)),
    _slotHeaderSize(ALIGN(sizeof(t_stack_slot), DEFAULT_ALIGN_SIZE))
  {
    DEBUG("StackAllocator: Constructor");
    this->_currentPage = (t_stack_page *) this->_systemAlloc(this->_pageSize);
    this->_currentPage->last = nullptr;
    this->_currentPage->next = nullptr;
    this->_currentPage->size = this->_pageSize;
    this->_currentPage->top = (t_stack_slot *) (((char *) this->_currentPage) + ALIGN(sizeof(t_stack_page), DEFAULT_ALIGN_SIZE));
    this->_currentPage->top->last = nullptr;
    this->_currentPage->top->free = true;
  }

  StackAllocator::~StackAllocator()
  {
    t_stack_page *page;

    DEBUG("StackAllocator: Destructor");
    if (this->_currentPage->last != nullptr ||
        this->_currentPage->top->last != nullptr)
      WARN("StackAllocator: Memory leaks detected!");
    page = this->_currentPage->last;
    this->_systemFree(this->_currentPage);
    while (page)
    {
      this->_currentPage = page;
      page = this->_currentPage->last;
      this->_systemFree(this->_currentPage);
    }
  }

  void *StackAllocator::_systemAlloc(std::uint64_t const size)
  {
    DEBUG("StackAllocator: malloc(" << size << ")");
    return (std::malloc(size));
  }

  void StackAllocator::_systemFree(void *ptr)
  {
    DEBUG("StackAllocator: free(0x" << ptr << ")");
    std::free(ptr);
  }

  void StackAllocator::_pageAlloc(std::uint64_t const size) throw()
  {
    std::uint64_t pageSize = ALIGN(size + this->_pageHeaderSize, this->_pageSize);
    t_stack_page *page = (t_stack_page *) this->_systemAlloc(pageSize);

    if (page == nullptr)
      throw std::bad_alloc();
    this->_currentPage->next = page;
    page->last = this->_currentPage;
    page->next = nullptr;
    page->size = pageSize;
    page->top = (t_stack_slot *) (((char *) this->_currentPage) + ALIGN(sizeof(t_stack_page), DEFAULT_ALIGN_SIZE));
    page->top->last = nullptr;
    page->top->free = true;
    this->_currentPage = page;
  }

  void StackAllocator::_pageFree()
  {
    t_stack_page *page = this->_currentPage;

    DEBUG("StackAllocator: Freeing page");
    if (page->last != nullptr)
    {
      this->_currentPage = page->last;
      this->_systemFree(page);
    }
  }

  void *StackAllocator::_slotAlloc(std::uint64_t const size) throw()
  {
    t_stack_slot *slot;
    void *ptr;

    slot = this->_currentPage->top;
    slot->free = false;
    ptr = ((char *) slot) + this->_slotHeaderSize;
    this->_currentPage->top = (t_stack_slot *) (((char *) slot) + size);
    this->_currentPage->top->last = slot;
    this->_currentPage->top->free = true;
    return (ptr);
  }

  void StackAllocator::_slotFree(t_stack_slot *top)
  {
    this->_currentPage->top = top;
    while (this->_currentPage->top->last != nullptr &&
           this->_currentPage->top->free)
      {
        this->_currentPage->top = this->_currentPage->top->last;
        if (this->_currentPage->top->last == nullptr &&
            this->_currentPage->top->free)
          this->_pageFree();
      }
  }

  void *StackAllocator::allocate(std::uint64_t const size) throw()
  {
    std::uint64_t slotSize = ALIGN(size, DEFAULT_ALIGN_SIZE) + this->_slotHeaderSize;
    void *ptr;

    if (((char *) this->_currentPage->top) + slotSize + this->_slotHeaderSize >= ((char *) this->_currentPage) + this->_currentPage->size)
    {
      DEBUG("StackAllocator: Page size exceeded");
      this->_pageAlloc(slotSize);
      ptr = this->_slotAlloc(slotSize);
    }
    else
      ptr = this->_slotAlloc(slotSize);
    return (ptr);
  }

  void StackAllocator::free(void *ptr)
  {
    t_stack_slot *slot = (t_stack_slot *) (((char *) ptr) - this->_slotHeaderSize);

    slot->free = true;
    if (this->_currentPage->top->last == slot)
      this->_slotFree(slot);
  }
};