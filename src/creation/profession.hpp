/// @file   profession.hpp
/// @brief  Define profession variables and methods.
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

#pragma once

#include <string>
#include <map>

#include "characterPosture.hpp"

/// @brief Holds details about a profession.
class Profession
{
public:
    /// The virtual number of the profession.
    unsigned int vnum;
    /// The name of the profession.
    std::string name;
    /// A simple descroption of the profession.
    std::string description;
    /// The command used in this profession.
    std::string command;
    /// The posture which the character must have while performing this profession.
    CharacterPosture posture;
    /// The verb describing the action performed in this profession.
    std::string action;
    /// The message shown at the beginning of the process of production.
    std::string startMessage;
    /// The message shown at the end of the process of production.
    std::string finishMessage;
    /// The message shown if the process of production has been a success.
    std::string successMessage;
    /// The message shown if the process of production has been a failure.
    std::string failureMessage;
    /// The message shown if the process of production has been interrupted.
    std::string interruptMessage;
    /// The message shown if the desired production has not been found.
    std::string notFoundMessage;

    /// @brief Constructor.
    Profession();

    /// @brief Disable Copy Construct.
    Profession(Profession const &) = delete;

    /// @brief Disable Move construct.
    Profession(Profession &&) = delete;

    /// @brief Disable Copy assign.
    Profession & operator=(Profession const &) = delete;

    /// @brief Disable Move assign.
    Profession & operator=(Profession &&) = delete;

    /// @brief Destructor.
    ~Profession();

    /// @brief Check the correctness of the profession.
    /// @return <b>True</b> if the profession has correct values,<br><b>False</b> otherwise.
    bool check();

    /// @brief Return the name of the profession.
    /// @return The name of the profession.
    std::string getName();

    /// @brief Return the name of the profession with the first letter capitalized.
    /// @return The name of the profession.
    std::string getNameCapital();
};
