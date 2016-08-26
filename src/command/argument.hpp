/// @file   argument.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 25, 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include <string>

/// @brief Allows to easely manage input arguments from players.
class Argument
{
    public:
        /// The original argument string.
        std::string original;
        /// The string with both the index and the multiplier removed.
        std::string content;
        /// The provided index.
        int index;
        /// The provided multiplier.
        unsigned int multiplier;

    public:
        /// @brief Constructor.
        Argument(const std::string & _original);

        /// @brief Destructor.
        virtual ~Argument();

        /// Provides the original argument.
        std::string getOriginal() const;

        /// Provides the content with both index and multiplier removed.
        std::string getContent() const;

        /// Provides the index.
        int & getIndex();

        /// Provides the multiplier.
        unsigned int getMultiplier() const;

    private:
        /// Evaluates the index.
        void evaluateIndex();

        /// Evaluates the multiplier.
        void evaluateMultiplier();
};
