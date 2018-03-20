# MIT License
# 
# Copyright (c) 2018 EkkoZ
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# 

project(ek-gfx)

set(SRC
        IWindowImpl.cpp
        Window.cpp)

set(GFX_SPECIFIC_FLAGS "")

if(WIN32)
        set(SRC ${SRC}
                Win32/WindowImplWin32.cpp)
endif()

if(APPLE)
        set(SRC ${SRC}
                OSX/WindowImplCocoa.cpp)
endif()

if(UNIX AND NOT APPLE)
        set(SRC ${SRC}
                Unix/WindowImplX11.cpp)
        
        set(GFX_SPECIFIC_FLAGS ${GFX_SPECIFIC_FLAGS}
                "-lGL"
                "-lX11")
endif()

add_library(ek-gfx STATIC ${SRC})

target_link_libraries(ek-gfx ek-utils ${GFX_SPECIFIC_FLAGS})
