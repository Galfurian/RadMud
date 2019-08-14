/// @file   updateInterface.hpp
/// @brief  Declaration of the UpdateInterface class.
/// @author Enrico Fraccaroli
/// @date   09/12/2016
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

/// @brief Interface class for objects that has to be updated.
/// @details A class extending this interface can be handled by the Updater
///           class by means of the two functions: updateTic and updateHour.
class UpdateInterface {
public:
	/// @brief Destructor.
	virtual ~UpdateInterface();

	/// @brief Function called by the Updated at each TIC.
	void updateTic();

	/// @brief Function called by the Updated at hour.
	void updateHour();

private:
	/// @brief Behavior executed at each TIC.
	virtual void updateTicImpl() = 0;

	/// @brief Behavior executed at each Hour.
	virtual void updateHourImpl() = 0;
};
