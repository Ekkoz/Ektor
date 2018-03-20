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

#include "Ek/Gfx/IWindowImpl.hpp"
#include "Ek/Utils/Config.hpp"

#if defined(EK_SYSTEM_WINDOWS)

  #include "Ek/Gfx/Unix/WindowImplWin32.hpp"
  typedef ek::WindowImplWin32 WindowImplType;

#elif defined(EK_SYSTEM_MACOS)

  #include "Ek/Gfx/Unix/WindowImplCocoa.hpp"
  typedef ek::WindowImplCocoa WindowImplType;

#elif defined(EK_SYSTEM_UNIX)

  #include "./Unix/WindowImplX11.hpp"
  typedef ek::WindowImplX11 WindowImplType;

#endif

namespace ek
{
  IWindowImpl::~IWindowImpl()
  {
  }

  IWindowImpl *IWindowImpl::create()
  {
    return (new WindowImplType());
  }
};