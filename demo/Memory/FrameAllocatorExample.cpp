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

#include <iostream>

#include "Ek/Memory/FrameAllocator.hpp"
#include "../SampleClass.hpp"

int main()
{
  /* Local allocator creation */
  ek::FrameAllocator allocator(DEFAULT_PAGE_SIZE, sizeof(char[27]));

  /* Any class to allocate */
  SampleClass *myClass;

  /* Or any data to allocate */
  char *myData;

  /* Class allocation */
  myClass = ALLOC_NEW(allocator, SampleClass, 42, 1337);

  /* Data allocation */
  myData = ALLOC_NEW(allocator, char[27]);

  /* Doing some stuff with allocated class */
  std::cout << *myClass << std::endl;
  myClass->setA(72);
  myClass->setB(101);
  std::cout << *myClass << std::endl;
  myClass->swap();
  std::cout << *myClass << std::endl;

  /* Doing some stuff with allocated data */
  for (char i = 65; i < 65 + 26; i++)
    myData[i - 65] = i;
  myData[26] = 0;
  std::cout << "Alphabet: " << myData << std::endl; 

  /* Class destruction */
  ALLOC_FREE(allocator, myClass);

  /* Data destruction */
  ALLOC_FREE(allocator, myData);

  /* Done! */
  return (0);
}