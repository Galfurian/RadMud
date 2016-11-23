/// @file   lua_script.hpp
/// @brief  Define all the methods used by lua to interract and communicate with the mud.
/// @author Enrico Fraccaroli
/// @date   Aug 25 2014
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following conditions:
///     The above copyright notice and this permission notice shall be included
///     in all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
/// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/// DEALINGS IN THE SOFTWARE.

#pragma once

#include <string>
#include <vector>
#include <cassert>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

/// @brief Allow from lua code, to log a string.
/// @param message The message to log.
void LuaLog(std::string message);

/// @brief Allow from lua to call the sleep function.
/// @param sleepTime The amount of seconds.
void LuaSleep(int sleepTime);

/// @brief Allow from lua coden to generate a random integer.
/// @param min Lower bound.
/// @param max Upper bound.
/// @return The generated random value.
int LuaRandom(int min, int max);

/// @brief Register in Lua all the functions.
/// @param L The lua state.
void LuaRegisterUtils(lua_State * L);
