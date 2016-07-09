/// @file   writing.hpp
/// @brief  Contains information about a writing.
/// @author Enrico Fraccaroli
/// @date   Jul 25 2015
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

#pragma once

/// @brief Holds details about a writing.
struct Writing
{
        /// The virtual number of the writing.
        int vnum;
        /// The title of the writing.
        std::string title;
        /// The author of the writing.
        std::string author;
        /// The content of the writing.
        std::string content;
};

/// Writing map handler.
typedef std::map<int, Writing *> WritingMap;

/// WritingMap iterator.
typedef WritingMap::iterator WritingMapIterator;
