///----------------------------------------------------------------------------
/// @file   LuaBridge.h
/// @copyright
/// Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
/// Copyright 2007, Nathan Reed
///
/// License: The MIT License (http://www.opensource.org/licenses/mit-license.php)
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///----------------------------------------------------------------------------

#pragma once

// All #include dependencies are listed here
// instead of in the individual header files.
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>

#define LUABRIDGE_MAJOR_VERSION   2
#define LUABRIDGE_MINOR_VERSION   0
#define LUABRIDGE_VERSION       200

namespace luabridge
{
#include "stack.hpp"
#include "LuaHelpers.hpp"
#include "TypeTraits.hpp"
#include "TypeList.hpp"
#include "FuncTraits.hpp"
#include "Constructor.hpp"
#include "classInfo.hpp"

class LuaRef;

#include "LuaException.hpp"
#include "LuaRef.hpp"
#include "Iterator.hpp"

#include "Security.hpp"

#include "Userdata.hpp"
#include "CFunctions.hpp"
#include "Namespace.hpp"


/// @brief Push an object onto the Lua stack.
template<class T>
inline void push(lua_State * L, T t)
{
    Stack<T>::push(L, t);
}

/// @brief Set a global value in the lua_State.
/// @note This works on any type specialized by `Stack`,
///        including `LuaRef` and its table proxies.
template<class T>
inline void setGlobal(lua_State * L, T t, char const * name)
{
    push(L, t);
    lua_setglobal(L, name);
}

/// @brief Change whether or not metatables are hidden (on by default).
inline void setHideMetatables(bool shouldHide)
{
    Security::setHideMetatables(shouldHide);
}
}
