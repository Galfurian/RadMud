/// @file   telnet.hpp
/// @brief  Define telnet class.
/// @author Enrico Fraccaroli
/// @date   Jan 25 2016
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

#ifndef TELNET_HPP
#define TELNET_HPP

#include <string>

/// @brief Telnet command generator.
class Telnet
{
    private:
        /// @brief Constructor.
        Telnet();

        /// @brief Constructor.
        ~Telnet();

        /// List of telnet commands.
        enum TelnetChar
        {
            //escape
            IAC = 255, // InterpretAsCommand
            //commands
            SubnegotiationEnd = 240,
            NoOperation = 241,
            DataMark = 242,
            Break = 243,
            InterruptProcess = 244,
            AbortOutput = 245,
            AreYouThere = 246,
            EraseCharacter = 247,
            EraseLine = 248,
            GoAhead = 249,
            SubnegotiationBegin = 250,
            //negotiation
            WILL = 251,
            WONT = 252,
            DO = 253,
            DONT = 254,
            //options (common)
            SuppressGoAhead = 3,
            Status = 5,
            Echo = 1,
            TimingMark = 6,
            TerminalType = 24,
            TerminalSpeed = 32,
            RemoteFlowControl = 33,
            LineMode = 34,
            EnvironmentVariables = 36,
            NAWS = 31,
            //options (MUD-specific)
            MSDP = 69,
            MXP = 91,
            MCCP1 = 85,
            MCCP2 = 86,
            MSP = 90,

            DRAW_MAP = 91,
            CLR_MAP = 92,

            FORMAT = 100
        };

        /// @brief Returns the string which identifies the start of a format section.
        /// @return The IAC:DO:FORMAT command.
        static std::string doFormat();

        /// @brief Returns the string which identifies the end of a format section.
        /// @return The IAC:DONT:FORMAT command.
        static std::string dontFormat();

    public:
        /// @brief Returns the string which identifies the command which clears the map.
        /// @return The IAC:DO:CLR_MAP command.
        static std::string doClearMap();

        /// @brief Returns the string which identifies the start of the map.
        /// @return The IAC:DO:DRAW_MAP command.
        static std::string doDrawMap();

        /// @brief Returns the string which identifies the end of the map.
        /// @return The IAC:DONT:DRAW_MAP command.
        static std::string dontDrawMap();

        /// @defgroup TelnetFunction Telnet Command Generation Function
        /// @brief All the functions necessary to generate formatted Telnet commands.

        /// @{
        /// @brief Returns the string which comands to reset any kind of change on the font.
        static std::string reset();
        /// @brief Allows to return the cursor to the beginning of the line.
        static std::string cursorHome();
        /// @brief Clears the entire scree.
        static std::string clearScreen();
        /// @brief Turn the font into Bold from now on.
        static std::string bold();
        /// @brief Turn the font into Italic from now on.
        static std::string italic();
        /// @brief Change the font color to Black from now on.
        static std::string black();
        /// @brief Change the font color to Black from now on.
        static std::string red();
        /// @brief Change the font color to Green from now on.
        static std::string green();
        /// @brief Change the font color to Brown from now on.
        static std::string brown();
        /// @brief Change the font color to Yellow from now on.
        static std::string yellow();
        /// @brief Change the font color to Blue from now on.
        static std::string blue();
        /// @brief Change the font color to Magenta from now on.
        static std::string magenta();
        /// @brief Change the font color to Cyan from now on.
        static std::string cyan();
        /// @brief Change the font color to Gray from now on.
        static std::string gray();
        /// @brief Change the font color to DarkGray from now on.
        static std::string darkGray();
        ///@}
};

#endif
