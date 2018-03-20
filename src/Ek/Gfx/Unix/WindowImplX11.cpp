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

#include <cstring>

#include "Ek/Gfx/Unix/WindowImplX11.hpp"
#include "Ek/Utils/Logger.hpp"

namespace ek
{
  GLint winAttributes[] =
  {
    GLX_X_RENDERABLE    , True,
    GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
    GLX_RENDER_TYPE     , GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
    GLX_RED_SIZE        , 8,
    GLX_GREEN_SIZE      , 8,
    GLX_BLUE_SIZE       , 8,
    GLX_ALPHA_SIZE      , 8,
    GLX_DEPTH_SIZE      , 24,
    GLX_STENCIL_SIZE    , 8,
    GLX_DOUBLEBUFFER    , True,
    /* Uncomment to enable multisampling */
    //GLX_SAMPLE_BUFFERS  , 1,
    //GLX_SAMPLES         , 4,
    None
  };
  
  GLint contextAttributes[] =
  {
    /* Configure a context based on OpenGL 3.3 */
    GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
    GLX_CONTEXT_MINOR_VERSION_ARB, 3,
    /* Uncomment to enable the compatibility profile */
    //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    None
  };

  static bool ctxErrorOccurred = false;

  static int ctxErrorHandler(Display *display, XErrorEvent *event)
  {
    ctxErrorOccurred = true;
    return (0);
  }

  static Bool checkXEvent(Display *display, XEvent *event, XPointer arg)
  {
    Window window = (Window) arg;
    
    return (event->xany.window == window);
  }

  WindowImplX11::WindowImplX11() : _initialised(false), _opened(false)
  {
  }

  WindowImplX11::~WindowImplX11()
  {
    this->close();
  }

  bool WindowImplX11::open(std::uint16_t width, std::uint16_t height)
  {
    this->_width = width;
    this->_height = height;

    if ((this->_display = XOpenDisplay(NULL)) == NULL)
    {
      ERROR("Window: Cannot open Display");
      return (false);
    }

    this->_screen = DefaultScreen(this->_display);

    if (!this->_isGLXCompatible())
    {
      ERROR("Window: GLX is not compatible: required at least version 1.3");
      return (false);
    }

    if (!this->_getBestFBConfig())
    {
      ERROR("Window: Cannot find any framebuffer configuration compatible");
      return (false);
    }

    if ((this->_visualInfo = glXGetVisualFromFBConfig(this->_display, this->_fbConfig)) == NULL)
    {
      ERROR("Window: Cannot extract visual info from the fb config");
      return (false);
    }

    this->_rootWindow = RootWindow(this->_display, this->_visualInfo->screen);
    this->_colormap = XCreateColormap(this->_display, this->_rootWindow, this->_visualInfo->visual, AllocNone);
    this->_x11Attributes.colormap = this->_colormap;
    this->_x11Attributes.background_pixmap = None;
    this->_x11Attributes.event_mask = KeyPressMask      | KeyReleaseMask   | ButtonPressMask    |
                                      ButtonReleaseMask | EnterWindowMask  | LeaveWindowMask    |
                                      PointerMotionMask | ButtonMotionMask | StructureNotifyMask;

    if (!(this->_window = XCreateWindow(this->_display, this->_rootWindow, 0, 0, width, height, 0, this->_visualInfo->depth, InputOutput, this->_visualInfo->visual, CWBorderPixel | CWColormap | CWEventMask, &this->_x11Attributes)))
    {
      ERROR("Window: Cannot create window");
      return (false);
    }

    XMapWindow(this->_display, this->_window);

    if (!this->_createOpenGLContext())
    {
      ERROR("Window: Cannot create OpenGL Context");
      return (false);
    }

    glXMakeCurrent(this->_display, this->_window, this->_glContext);
    XFlush(this->_display);

    this->_initialised = true;
    this->_opened = true;
    return (true);
  }

  void WindowImplX11::close()
  {
    this->_opened = false;
    if (this->_initialised)
    {
      XFree(this->_visualInfo);
      glXMakeCurrent(this->_display, 0, 0);
      glXDestroyContext(this->_display, this->_glContext);
      XDestroyWindow(this->_display, this->_window);
      XFreeColormap(this->_display, this->_colormap);
      XCloseDisplay(this->_display);
      this->_initialised = false;
    }
  }

  bool WindowImplX11::isOpen() const
  {
    return (this->_opened);
  }

  void WindowImplX11::display()
  {
    if (this->_initialised && this->_opened)
      glXSwapBuffers(this->_display, this->_window);
  }

  bool WindowImplX11::pollEvent(ek::Event &event)
  {
    XEvent x11Event;

    if (XCheckIfEvent(this->_display, &x11Event, &checkXEvent, (XPointer) this->_window))
    {
      switch (x11Event.type)
      {
        /* KeyPressMask: Key down event */
        case KeyPress:
        {
          event.type = ek::Event::KeyPressed;
          event.key.code = ek::Keyboard::Unknown;
          event.key.alt = x11Event.xkey.state & Mod1Mask;
          event.key.control = x11Event.xkey.state & ControlMask;
          event.key.shift = x11Event.xkey.state & ShiftMask;
          event.key.system = x11Event.xkey.state & Mod4Mask;

          for (int i = 0; i < 4 && event.key.code == ek::Keyboard::Unknown; i++)
            event.key.code = this->_toEKKey(XLookupKeysym(&x11Event.xkey, i));
          
          if (event.key.code == ek::Keyboard::Unknown)
            return (false);

          return (true);
        }

        /* KeyReleaseMask: Key up event */
        case KeyRelease:
        {
          event.type = ek::Event::KeyReleased;
          event.key.code = ek::Keyboard::Unknown;
          event.key.alt = x11Event.xkey.state & Mod1Mask;
          event.key.control = x11Event.xkey.state & ControlMask;
          event.key.shift = x11Event.xkey.state & ShiftMask;
          event.key.system = x11Event.xkey.state & Mod4Mask;

          for (int i = 0; i < 4 && event.key.code == ek::Keyboard::Unknown; i++)
            event.key.code = this->_toEKKey(XLookupKeysym(&x11Event.xkey, i));
          
          if (event.key.code == ek::Keyboard::Unknown)
            return (false);

          return (true);
        }

        /* ButtonPressMask: Mouse button down event */
        case ButtonPress:
        {
          unsigned int button = x11Event.xbutton.button;

          // Check X11 mouse button, 4 & 5 are the vertical wheel and 6 & 7 the horizontal one (= button)
          if (button == Button1 ||
              button == Button2 ||
              button == Button3 ||
              button == 8 ||
              button == 9)
          {
            event.type = ek::Event::MouseButtonPressed;
            event.mouseButton.x = x11Event.xbutton.x;
            event.mouseButton.y = x11Event.xbutton.y;
            switch(button)
            {
              case Button1:
                event.mouseButton.button = ek::Mouse::Left;
                return (true);

              case Button2:
                event.mouseButton.button = ek::Mouse::Middle;
                return (true);

              case Button3:
                event.mouseButton.button = ek::Mouse::Right;
                return (true);

              case 8:
                event.mouseButton.button = ek::Mouse::XButton1;
                return (true);

              case 9:
                event.mouseButton.button = ek::Mouse::XButton2;
                return (true);
            }
          }
          return (false);
        }

        /* ButtonReleaseMask: Mouse button up event */
        case ButtonRelease:
        {
          unsigned int button = x11Event.xbutton.button;

          if (button == Button1 ||
              button == Button2 ||
              button == Button3 ||
              button == 8 ||
              button == 9)
          {
            event.type = ek::Event::MouseButtonReleased;
            event.mouseButton.x = x11Event.xbutton.x;
            event.mouseButton.y = x11Event.xbutton.y;
            switch(button)
            {
              case Button1:
                event.mouseButton.button = ek::Mouse::Left;
                return (true);

              case Button2:
                event.mouseButton.button = ek::Mouse::Middle;
                return (true);

              case Button3:
                event.mouseButton.button = ek::Mouse::Right;
                return (true);

              case 8:
                event.mouseButton.button = ek::Mouse::XButton1;
                return (true);

              case 9:
                event.mouseButton.button = ek::Mouse::XButton2;
                return (true);
            }
          }
          else if (button == Button4 || button == Button5)
          {
            event.type = ek::Event::MouseWheelScrolled;
            event.mouseWheelScroll.wheel = ek::Mouse::VerticalWheel;
            event.mouseWheelScroll.delta = (button == Button4) ? 1 : -1;
            event.mouseWheelScroll.x = x11Event.xbutton.x;
            event.mouseWheelScroll.y = x11Event.xbutton.y;
            return (true);
          }
          else if (button == 6 || button == 7)
          {
            event.type = ek::Event::MouseWheelScrolled;
            event.mouseWheelScroll.wheel = ek::Mouse::HorizontalWheel;
            event.mouseWheelScroll.delta = (button == 6) ? 1 : -1;
            event.mouseWheelScroll.x = x11Event.xbutton.x;
            event.mouseWheelScroll.y = x11Event.xbutton.y;
            return (true);
          }
          return (false);
        }

        /* EnterWindowMask: Mouse entered the window */
        case EnterNotify:
        {
          if (x11Event.xcrossing.mode == NotifyNormal)
          {
            event.type = ek::Event::MouseEntered;
            return (true);
          }
          return (false);
        }

        /* LeaveWindowMask: Mouse left the window */
        case LeaveNotify:
        {
          if (x11Event.xcrossing.mode == NotifyNormal)
          {
            event.type = ek::Event::MouseLeft;
            return (true);
          }
          return (false);
        }

        /* PointerMotionMask & ButtonMotionMask: Mouse moved */
        case MotionNotify:
        {
          event.type = ek::Event::MouseMoved;
          event.mouseMove.x = x11Event.xmotion.x;
          event.mouseMove.y = x11Event.xmotion.y;
          return (true);
        }
        
        /* StructureNotifyMask: Destroy the window */
        case DestroyNotify:
        {
          this->close();
          return (false);
        }
        
        /* StructureNotifyMask: Resize the window */
        case ConfigureNotify:
        {
          if (x11Event.xconfigure.width != this->_width ||
              x11Event.xconfigure.height != this->_height)
          {
            event.type = ek::Event::Resized;
            event.size.width = x11Event.xconfigure.width;
            event.size.height = x11Event.xconfigure.height;

            this->_width = x11Event.xconfigure.width;
            this->_height = x11Event.xconfigure.height;
            
            return (true);
          }
          return (false);
        }

        /* TODO Atom protocol to detect interactions with WM */
        case ClientMessage:
        {
          return (false);
        }
      }
    }
    return (false);
  }

  bool WindowImplX11::_isGLXCompatible()
  {
    int glxMajor;
    int glxMinor;

    if (!glXQueryVersion(this->_display, &glxMajor, &glxMinor) ||
        glxMajor < 1 ||
        (glxMajor == 1 && glxMinor < 3))
        return (false);
    DEBUG("Window: Using GLX version " << glxMajor << "." << glxMinor);
    return (true);
  }

  bool WindowImplX11::_getBestFBConfig()
  {
    XVisualInfo *visualInfo;
    GLXFBConfig *list;
    GLXFBConfig result;
    int listSize;
    int bestConfig = -1;
    int bestSamples = -1;
    int tmpBuff;
    int tmpSamples;
    int i;

    if ((list = glXChooseFBConfig(this->_display, this->_screen, winAttributes, &listSize)) == NULL ||
        listSize == 0)
    {
      ERROR("Window: Cannot find any framebuffer configuration compatible");
      return (false);
    }
    DEBUG("Window: Found " << listSize << " framebuffer configurations compatibles");
    for (i = 0; i < listSize; i++)
    {
      if ((visualInfo = glXGetVisualFromFBConfig(this->_display, list[i])))
      {
        glXGetFBConfigAttrib(this->_display, list[i], GLX_SAMPLE_BUFFERS, &tmpBuff);
        glXGetFBConfigAttrib(this->_display, list[i], GLX_SAMPLES, &tmpSamples);
        if (bestConfig < 0 || tmpBuff && tmpSamples > bestSamples)
        {
          bestConfig = i;
          bestSamples = tmpSamples;
        }
        XFree(visualInfo);
      }
    }
    this->_fbConfig = list[bestConfig];
    XFree(list);
    return (true);
  }

  bool WindowImplX11::_createOpenGLContext()
  {
    int (*oldHandler)(Display *, XErrorEvent *) = XSetErrorHandler(&ctxErrorHandler);
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");

    if (!this->_isExtensionSupported("GLX_ARB_create_context") ||
        !glXCreateContextAttribsARB)
    {
      ERROR("Window: The extension GLX_ARB_create_context doesn't exist: cannot create OpenGL context");
      return (false);
    }
    this->_glContext = glXCreateContextAttribsARB(this->_display, this->_fbConfig, 0, True, contextAttributes);
    XSync(this->_display, False);
    if (!ctxErrorOccurred && this->_glContext)
    {
      DEBUG("Window: OpenGL 3.3 Context created!");
      XSetErrorHandler(oldHandler);
      return (true);
    }
    ERROR("Window: Cannot create OpenGL 3.3 context");
    return (false);
  }

  bool WindowImplX11::_isExtensionSupported(const char *extension)
  {
    const char *extList = glXQueryExtensionsString(this->_display, this->_screen);
    const char *start;
    const char *where, *terminator;

    where = strchr(extension, ' ');
    if (where || *extension == '\0')
      return (false);
    for (start = extList;;) {
      where = strstr(start, extension);
      if (!where)
        break;
      terminator = where + strlen(extension);
      if (where == start || *(where - 1) == ' ')
        if (*terminator == ' ' || *terminator == '\0')
          return (true);

      start = terminator;
    }
    return (false);
  }

  ek::Keyboard::Key WindowImplX11::_toEKKey(::KeySym symbol)
  {
    switch (symbol)
    {
      case XK_Shift_L:      return (ek::Keyboard::LShift);
      case XK_Shift_R:      return (ek::Keyboard::RShift);
      case XK_Control_L:    return (ek::Keyboard::LControl);
      case XK_Control_R:    return (ek::Keyboard::RControl);
      case XK_Alt_L:        return (ek::Keyboard::LAlt);
      case XK_Alt_R:        return (ek::Keyboard::RAlt);
      case XK_Super_L:      return (ek::Keyboard::LSystem);
      case XK_Super_R:      return (ek::Keyboard::RSystem);
      case XK_Menu:         return (ek::Keyboard::Menu);
      case XK_Escape:       return (ek::Keyboard::Escape);
      case XK_semicolon:    return (ek::Keyboard::SemiColon);
      case XK_slash:        return (ek::Keyboard::Slash);
      case XK_equal:        return (ek::Keyboard::Equal);
      case XK_minus:        return (ek::Keyboard::Dash);
      case XK_bracketleft:  return (ek::Keyboard::LBracket);
      case XK_bracketright: return (ek::Keyboard::RBracket);
      case XK_comma:        return (ek::Keyboard::Comma);
      case XK_period:       return (ek::Keyboard::Period);
      case XK_apostrophe:   return (ek::Keyboard::Quote);
      case XK_backslash:    return (ek::Keyboard::BackSlash);
      case XK_grave:        return (ek::Keyboard::Tilde);
      case XK_space:        return (ek::Keyboard::Space);
      case XK_Return:       return (ek::Keyboard::Return);
      case XK_KP_Enter:     return (ek::Keyboard::Return);
      case XK_BackSpace:    return (ek::Keyboard::BackSpace);
      case XK_Tab:          return (ek::Keyboard::Tab);
      case XK_Prior:        return (ek::Keyboard::PageUp);
      case XK_Next:         return (ek::Keyboard::PageDown);
      case XK_End:          return (ek::Keyboard::End);
      case XK_Home:         return (ek::Keyboard::Home);
      case XK_Insert:       return (ek::Keyboard::Insert);
      case XK_Delete:       return (ek::Keyboard::Delete);
      case XK_KP_Add:       return (ek::Keyboard::Add);
      case XK_KP_Subtract:  return (ek::Keyboard::Subtract);
      case XK_KP_Multiply:  return (ek::Keyboard::Multiply);
      case XK_KP_Divide:    return (ek::Keyboard::Divide);
      case XK_Pause:        return (ek::Keyboard::Pause);
      case XK_F1:           return (ek::Keyboard::F1);
      case XK_F2:           return (ek::Keyboard::F2);
      case XK_F3:           return (ek::Keyboard::F3);
      case XK_F4:           return (ek::Keyboard::F4);
      case XK_F5:           return (ek::Keyboard::F5);
      case XK_F6:           return (ek::Keyboard::F6);
      case XK_F7:           return (ek::Keyboard::F7);
      case XK_F8:           return (ek::Keyboard::F8);
      case XK_F9:           return (ek::Keyboard::F9);
      case XK_F10:          return (ek::Keyboard::F10);
      case XK_F11:          return (ek::Keyboard::F11);
      case XK_F12:          return (ek::Keyboard::F12);
      case XK_F13:          return (ek::Keyboard::F13);
      case XK_F14:          return (ek::Keyboard::F14);
      case XK_F15:          return (ek::Keyboard::F15);
      case XK_Left:         return (ek::Keyboard::Left);
      case XK_Right:        return (ek::Keyboard::Right);
      case XK_Up:           return (ek::Keyboard::Up);
      case XK_Down:         return (ek::Keyboard::Down);
      case XK_KP_Insert:    return (ek::Keyboard::Numpad0);
      case XK_KP_End:       return (ek::Keyboard::Numpad1);
      case XK_KP_Down:      return (ek::Keyboard::Numpad2);
      case XK_KP_Page_Down: return (ek::Keyboard::Numpad3);
      case XK_KP_Left:      return (ek::Keyboard::Numpad4);
      case XK_KP_Begin:     return (ek::Keyboard::Numpad5);
      case XK_KP_Right:     return (ek::Keyboard::Numpad6);
      case XK_KP_Home:      return (ek::Keyboard::Numpad7);
      case XK_KP_Up:        return (ek::Keyboard::Numpad8);
      case XK_KP_Page_Up:   return (ek::Keyboard::Numpad9);
      case XK_a:            return (ek::Keyboard::A);
      case XK_b:            return (ek::Keyboard::B);
      case XK_c:            return (ek::Keyboard::C);
      case XK_d:            return (ek::Keyboard::D);
      case XK_e:            return (ek::Keyboard::E);
      case XK_f:            return (ek::Keyboard::F);
      case XK_g:            return (ek::Keyboard::G);
      case XK_h:            return (ek::Keyboard::H);
      case XK_i:            return (ek::Keyboard::I);
      case XK_j:            return (ek::Keyboard::J);
      case XK_k:            return (ek::Keyboard::K);
      case XK_l:            return (ek::Keyboard::L);
      case XK_m:            return (ek::Keyboard::M);
      case XK_n:            return (ek::Keyboard::N);
      case XK_o:            return (ek::Keyboard::O);
      case XK_p:            return (ek::Keyboard::P);
      case XK_q:            return (ek::Keyboard::Q);
      case XK_r:            return (ek::Keyboard::R);
      case XK_s:            return (ek::Keyboard::S);
      case XK_t:            return (ek::Keyboard::T);
      case XK_u:            return (ek::Keyboard::U);
      case XK_v:            return (ek::Keyboard::V);
      case XK_w:            return (ek::Keyboard::W);
      case XK_x:            return (ek::Keyboard::X);
      case XK_y:            return (ek::Keyboard::Y);
      case XK_z:            return (ek::Keyboard::Z);
      case XK_0:            return (ek::Keyboard::Num0);
      case XK_1:            return (ek::Keyboard::Num1);
      case XK_2:            return (ek::Keyboard::Num2);
      case XK_3:            return (ek::Keyboard::Num3);
      case XK_4:            return (ek::Keyboard::Num4);
      case XK_5:            return (ek::Keyboard::Num5);
      case XK_6:            return (ek::Keyboard::Num6);
      case XK_7:            return (ek::Keyboard::Num7);
      case XK_8:            return (ek::Keyboard::Num8);
      case XK_9:            return (ek::Keyboard::Num9);
    }
    
    return (ek::Keyboard::Unknown);
  }

};