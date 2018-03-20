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

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "Ek/Gfx/IWindowImpl.hpp"

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);

namespace ek
{
  class WindowImplX11 : public IWindowImpl
  {
  private:
    ::Display *_display;
    int _screen;
    GLXFBConfig _fbConfig;
    XVisualInfo *_visualInfo;
    Colormap _colormap;
    Window _rootWindow;
    Window _window;
    XSetWindowAttributes _x11Attributes;
    GLXContext _glContext;

    bool _initialised;
    bool _opened;

    std::uint16_t _width;
    std::uint16_t _height;

    bool _isGLXCompatible();
    bool _getBestFBConfig();
    bool _createOpenGLContext();
    bool _isExtensionSupported(const char *);

    ek::Keyboard::Key _toEKKey(::KeySym);

  public:
    WindowImplX11();
    ~WindowImplX11();

    bool open(std::uint16_t, std::uint16_t);
    void close();

    bool isOpen() const;

    void display();

    bool pollEvent(ek::Event &);
  };
};