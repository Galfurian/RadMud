/// @file   stack.hpp
/// @copyright
/// Copyright 2007, Nathan Reed
/// Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
/// Copyright 2016, Robin Gareus <robin@gareus.org>
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

#pragma once

namespace luabridge
{
/// Forward declaration.
template <class T> struct Stack;

/// @brief Push an object onto the Lua stack.
template <class T> inline void push(lua_State *L, T t)
{
	Stack<T>::push(L, t);
}

/// @brief Set a global value in the lua_State.
/// @note This works on any type specialized by `Stack`,
///        including `LuaRef` and its table proxies.
template <class T> inline void setGlobal(lua_State *L, T t, char const *name)
{
	push(L, t);
	lua_setglobal(L, name);
}

} // namespace luabridge

#include "stack.i.hpp"
