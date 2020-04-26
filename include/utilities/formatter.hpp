/// @file   formatter.hpp
/// @brief  Define the formatter class.
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

#pragma once

#include "enumerators/telnetChar.hpp"

/// @brief Multi-Format controller.
class Formatter {
public:
	/// List of formats.
	enum Format { ASCII, CLIENT };

	/// @brief Constructor.
	Formatter() = default;

	/// @brief Constructor.
	~Formatter() = default;

	/// @brief Provides the format used by the Mud for the output.
	static inline Format getFormat()
	{
		return ASCII;
	}

	/// @brief Returns the string which identifies the command which clears the map.
	/// @return The IAC:DO:CLR_MAP command.
	static inline std::string doClearMap()
	{
		if (getFormat() != CLIENT)
			return "";
		std::string o;
		o.push_back('\0');
		o.push_back(static_cast<char>(TelnetChar::IAC));
		o.push_back(static_cast<char>(TelnetChar::DO));
		o.push_back(static_cast<char>(TelnetChar::CLR_MAP));
		o.push_back('\0');
		return o;
	}

	/// @brief Returns the string which identifies the start of the map.
	/// @return The IAC:DO:DRAW_MAP command.
	static inline std::string doDrawMap()
	{
		if (getFormat() != CLIENT)
			return "";
		std::string o;
		o.push_back('\0');
		o.push_back(static_cast<char>(TelnetChar::IAC));
		o.push_back(static_cast<char>(TelnetChar::DO));
		o.push_back(static_cast<char>(TelnetChar::DRAW_MAP));
		o.push_back('\0');
		return o;
	}

	/// @brief Returns the string which identifies the end of the map.
	/// @return The IAC:DONT:DRAW_MAP command.
	static inline std::string dontDrawMap()
	{
		if (getFormat() != CLIENT)
			return "";
		std::string o;
		o.push_back('\0');
		o.push_back(static_cast<char>(TelnetChar::IAC));
		o.push_back(static_cast<char>(TelnetChar::DONT));
		o.push_back(static_cast<char>(TelnetChar::DRAW_MAP));
		o.push_back('\0');
		return o;
	}

	/// @defgroup TelnetFunction Telnet Command Generation Function
	/// @brief All the functions necessary to generate formatted Telnet commands.

	/// @{
	/// @brief Returns the string which comands to reset any kind of change on the font.
	static inline std::string reset()
	{
		if (getFormat() == ASCII)
			return "\u001B[0m";
		return doFormat() + "RESET" + dontFormat();
	}

	/// @brief Allows to return the cursor to the beginning of the line.
	static inline std::string cursorHome()
	{
		if (getFormat() == ASCII)
			return "\u001b[H";
		return doFormat() + "CURSOR-HOME" + dontFormat();
	}

	/// @brief Clears the entire scree.
	static inline std::string clearScreen()
	{
		if (getFormat() == ASCII)
			return "\u001B[2J";
		return doFormat() + "CLEAR-SCREEN" + dontFormat();
	}

	/// @brief Turn the font into Bold from now on.
	static inline std::string bold()
	{
		if (getFormat() == ASCII)
			return "\u001B[1m";
		return doFormat() + "BOLD" + dontFormat();
	}

	/// @brief Turn the font of the given string to Bold.
	static inline std::string bold(std::string s)
	{
		return (bold() + std::move(s) + reset());
	}

	/// @brief Turn the font into Italic from now on.
	static inline std::string italic()
	{
		if (getFormat() == ASCII)
			return "\u001B[3m";
		return doFormat() + "ITALIC" + dontFormat();
	}

	/// @brief Turn the font of the given string to Italic.
	static inline std::string italic(std::string s)
	{
		return (italic() + std::move(s) + reset());
	}

	/// @brief Change the font color to Black from now on.
	static inline std::string black()
	{
		if (getFormat() == ASCII)
			return "\u001B[0;30m";
		return doFormat() + "BLACK" + dontFormat();
	}

	/// @brief Turn the font of the given string to black.
	static inline std::string black(std::string s)
	{
		return (black() + std::move(s) + reset());
	}

	/// @brief Change the font color to Black from now on.
	static inline std::string red()
	{
		if (getFormat() == ASCII)
			return "\u001B[0;31m";
		return doFormat() + "RED" + dontFormat();
	}

	/// @brief Turn the font of the given string to red.
	static inline std::string red(std::string s)
	{
		return (red() + std::move(s) + reset());
	}

	/// @brief Change the font color to Green from now on.
	static inline std::string green()
	{
		if (getFormat() == ASCII)
			return "\u001B[0;32m";
		return doFormat() + "GREEN" + dontFormat();
	}

	/// @brief Turn the font of the given string to green.
	static inline std::string green(std::string s)
	{
		return (green() + std::move(s) + reset());
	}

	/// @brief Change the font color to Brown from now on.
	static inline std::string brown()
	{
		if (getFormat() == ASCII)
			return "\u001B[0;33m";
		return doFormat() + "BROWN" + dontFormat();
	}

	/// @brief Turn the font of the given string to brown.
	static inline std::string brown(std::string s)
	{
		return (brown() + std::move(s) + reset());
	}

	/// @brief Change the font color to Yellow from now on.
	static inline std::string yellow()
	{
		if (getFormat() == ASCII)
			return "\u001B[1;33m";
		return doFormat() + "YELLOW" + dontFormat();
	}

	/// @brief Change the font color of the given text.
	static inline std::string yellow(std::string s)
	{
		return (yellow() + std::move(s) + reset());
	}

	/// @brief Change the font color to Blue from now on.
	static inline std::string blue()
	{
		if (getFormat() == ASCII)
			return "\u001B[0;34m";
		return doFormat() + "BLUE" + dontFormat();
	}

	/// @brief Turn the font of the given string to blue.
	static inline std::string blue(std::string s)
	{
		return (blue() + std::move(s) + reset());
	}

	/// @brief Change the font color to Magenta from now on.
	static inline std::string magenta()
	{
		if (getFormat() == ASCII)
			return "\u001B[0;35m";
		return doFormat() + "MAGENTA" + dontFormat();
	}

	/// @brief Turn the font of the given string to magenta.
	static inline std::string magenta(std::string s)
	{
		return (magenta() + std::move(s) + reset());
	}

	/// @brief Change the font color to Cyan from now on.
	static inline std::string cyan()
	{
		if (getFormat() == ASCII)
			return "\u001B[0;36m";
		return doFormat() + "CYAN" + dontFormat();
	}

	/// @brief Turn the font of the given string to cyan.
	static inline std::string cyan(std::string s)
	{
		return (cyan() + std::move(s) + reset());
	}

	/// @brief Change the font color to Gray from now on.
	static inline std::string gray()
	{
		if (getFormat() == ASCII)
			return "\u001B[0;37m";
		return doFormat() + "GRAY" + dontFormat();
	}

	/// @brief Turn the font of the given string to cyan.
	static inline std::string gray(std::string s)
	{
		return (gray() + std::move(s) + reset());
	}

	/// @brief Change the font color to DarkGray from now on.
	static inline std::string darkGray()
	{
		if (getFormat() == ASCII)
			return "\u001B[1;30m";
		return doFormat() + "DARK-GRAY" + dontFormat();
	}

	/// @brief Turn the font of the given string to darkGray.
	static inline std::string darkGray(std::string s)
	{
		return (darkGray() + std::move(s) + reset());
	}

	///@}

private:
	/// @brief Returns the string which identifies the start of a format section.
	/// @return The IAC:DO:FORMAT command.
	static inline std::string doFormat()
	{
		if (getFormat() != CLIENT)
			return "";
		std::string o;
		o.push_back('\0');
		o.push_back(static_cast<char>(TelnetChar::IAC));
		o.push_back(static_cast<char>(TelnetChar::DO));
		o.push_back(static_cast<char>(TelnetChar::FORMAT));
		o.push_back('\0');
		return o;
	}

	/// @brief Returns the string which identifies the end of a format section.
	/// @return The IAC:DONT:FORMAT command.
	static inline std::string dontFormat()
	{
		if (getFormat() != CLIENT)
			return "";
		std::string o;
		o.push_back('\0');
		o.push_back(static_cast<char>(TelnetChar::IAC));
		o.push_back(static_cast<char>(TelnetChar::DONT));
		o.push_back(static_cast<char>(TelnetChar::FORMAT));
		o.push_back('\0');
		return o;
	}
};
