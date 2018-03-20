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

#include <cstdint>

#include "Ek/Gfx/Keyboard.hpp"
#include "Ek/Gfx/Mouse.hpp"

namespace ek
{
  class Event
  {
  public:
    struct ResizeEvent
    {
      std::uint16_t width;
      std::uint16_t height;
    };

    struct KeyEvent
    {
      ek::Keyboard::Key code;
      bool alt;
      bool control;
      bool shift;
      bool system;
    };

    struct MouseMoveEvent
    {
      std::uint16_t x;
      std::uint16_t y;
    };

    struct MouseButtonEvent
    {
      ek::Mouse::Button button;
      std::uint16_t x;
      std::uint16_t y;
    };

    struct MouseWheelScrollEvent
    {
      ek::Mouse::Wheel wheel;
      float delta;
      std::uint16_t x;
      std::uint16_t y;
    };

    enum EventType
    {
      Closed,
      Resized,
      KeyPressed,
      KeyReleased,
      MouseWheelScrolled,
      MouseButtonPressed,
      MouseButtonReleased,
      MouseMoved,
      MouseEntered,
      MouseLeft,
      Count
    };

    EventType type;

    union
    {
      ResizeEvent size;
      KeyEvent key;
      MouseMoveEvent mouseMove;
      MouseButtonEvent mouseButton;
      MouseWheelScrollEvent mouseWheelScroll;
    };
  };
};