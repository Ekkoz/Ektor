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

#include "SampleClass.hpp"

SampleClass::SampleClass() : _a(0), _b(0)
{
}

SampleClass::SampleClass(std::uint64_t a, std::uint64_t b) : _a(a), _b(b)
{
}

SampleClass::~SampleClass()
{
}

std::uint64_t SampleClass::getA() const
{
  return (this->_a);
}

std::uint64_t SampleClass::getB() const
{
  return (this->_b);
}

void SampleClass::setA(std::uint64_t a)
{
  this->_a = a;
}

void SampleClass::setB(std::uint64_t b)
{
  this->_b = b;
}

void SampleClass::swap()
{
  std::uint64_t tmp = this->getA();

  this->setA(this->getB());
  this->setB(tmp);
}

std::ostream &operator<<(std::ostream &stream, const SampleClass &data)
{
  stream << "SampleClass(a=" << data.getA() << ",b=" << data.getB() << ");";
  return (stream);
}