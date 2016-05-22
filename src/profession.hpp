/// @file   profession.hpp
/// @brief  Define profession variables and methods.
/// @author Enrico Fraccaroli
/// @date   Aug 11 2015
/// @copyright
/// Copyright (c) 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#ifndef PROFESSION_HPP
#define PROFESSION_HPP

#include <string>
#include <map>
#include "defines.hpp"

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

        /// @brief Copy Constructor.
        Profession(const Profession & source);

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

/// Profession map handler.
typedef std::map<unsigned int, Profession> ProfessionMap;

/// ProfessionMap iterator.
typedef ProfessionMap::iterator ProfessionMapIterator;

#endif
