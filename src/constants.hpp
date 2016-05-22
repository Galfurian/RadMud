/// @file   constants.hpp
/// @brief  Contains the mud server settings.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
/// @copyright
/// Copyright (c) 2014, 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <ctime>
#include <string>
#include <vector>

// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
// Development constants.
// ////////////////////////////////////////////////////////////////////////////
/// Enable or disable DEBUGGING messages.
#define DEBUG 1

/// Determine which message it's logged.
static const std::vector<bool> kLogFlags =
{ true,    // kMErr
    true,  // kMWrn
    true,  // kMSys
    true,  // kMDat
    true,  // kMMud
    true,  // kMPla
    true,  // kMDev
    true,  // kMLua
    false  // kMDec
    };

/// Error messages.
static const int kMErr = 0;
/// Warning messages.
static const int kMWrn = 1;
/// System messages.
static const int kMSys = 2;
/// Database messages.
static const int kMDat = 3;
/// Mud messages.
static const int kMMud = 4;
/// Player actions messages.
static const int kMPla = 5;
/// Developers actions messages.
static const int kMDev = 6;
/// Lua messages.
static const int kMLua = 7;
/// Deconstructor messages.
static const int kMDec = 8;

// ////////////////////////////////////////////////////////////////////////////
// Global constants.
// ////////////////////////////////////////////////////////////////////////////
/// Server version.
static const std::string kVersion = "15";
/// Incoming connections port.
static const int kPort = 4000;
/// What room they start in.
static const int kInitialRoom = 1000;
/// Times they can try a password.
static const int kMaxPasswordAttempts = 3;
/// Mud weight measure.
static const std::string mud_measure = "lbs";
/// Mud coin name.
static const std::string mud_currency = "coins";
/// The maximum value of byte sent at time.
static const int kMaxStream = 4096;
/// Indicator for no socket connected.
static const int kNoSocketIndicator = -1;
/// Database file name.
static const std::string kDatabaseName = "radmud.db";
/// Location of system files.
static const std::string kSystemDir = "../system/";

#endif
