/// @file   profession.cpp
/// @brief  Implement profession methods.
/// @author Enrico Fraccaroli
/// @date   Aug 11 2015
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

#include "creation/profession.hpp"

Profession::Profession() :
    vnum(),
    command(),
    action(),
    description(),
    startMessage(),
    finishMessage(),
    successMessage(),
    failureMessage(),
    interruptMessage(),
    notFoundMessage()
{
    // Nothing to do.
}

Profession::~Profession()
{
//    Logger::log(LogLevel::Debug,
//                "Deleted profession\t[%s]\t\t(%s)",
//                ToString(this->vnum),
//                this->name);
}

bool Profession::check()
{
    if (vnum <= 0) return false;
    if (command.empty()) return false;
    if (description.empty()) return false;
    if (action.empty()) return false;
    if (startMessage.empty()) return false;
    if (finishMessage.empty()) return false;
    if (successMessage.empty()) return false;
    if (failureMessage.empty()) return false;
    if (interruptMessage.empty()) return false;
    if (notFoundMessage.empty()) return false;
    return true;
}
