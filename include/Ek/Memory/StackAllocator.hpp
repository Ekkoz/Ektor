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

#pragma once

#include "Ek/Memory/Memory.hpp"

namespace ek
{
  class StackAllocator
  {
  private:
    typedef struct s_stack_slot {
      struct s_stack_slot *last;
      bool free;
    } t_stack_slot;

    typedef struct s_stack_page {
      struct s_stack_page *last;
      struct s_stack_page *next;
      std::uint64_t size;
      t_stack_slot *top;
    } t_stack_page;

    void *_systemAlloc(std::uint64_t const);
    void  _systemFree(void *);

    void  _pageAlloc(std::uint64_t const) throw();
    void  _pageFree();

    void *_slotAlloc(std::uint64_t const) throw();
    void  _slotFree(t_stack_slot *);

    std::uint64_t _pageSize;
    std::uint64_t _pageHeaderSize;
    std::uint64_t _slotHeaderSize;

    t_stack_page *_currentPage;

  public:
    StackAllocator(std::uint64_t = DEFAULT_PAGE_SIZE);
    ~StackAllocator();

    StackAllocator(StackAllocator const &) = delete;
    void operator=(StackAllocator const &) = delete;

    void *allocate(std::uint64_t const) throw();
    void free(void *);
  };
};