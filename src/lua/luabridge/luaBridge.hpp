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

#pragma once

#include <cassert>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <functional>

#include <cassert>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include <bitset>
#include <list>
#include <map>
#include <set>
#include <vector>

#include <inttypes.h>
#include <type_traits>
#include <memory>

#define LUABRIDGE_MAJOR_VERSION 2
#define LUABRIDGE_MINOR_VERSION 0
#define LUABRIDGE_VERSION 200

#include "luaHelpers.hpp"
#include "stack.hpp"
#include "typeTraits.hpp"
#include "typeList.hpp"
#include "funcTraits.hpp"
#include "constructor.hpp"
#include "classInfo.hpp"
#include "luaException.hpp"
#include "luaRef.hpp"
#include "iterator.hpp"
#include "security.hpp"
#include "userdata.hpp"
#include "cFunctions.hpp"
#include "namespace.hpp"
