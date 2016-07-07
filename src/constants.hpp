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

#pragma once

#include <ctime>
#include <string>
#include <vector>

// ////////////////////////////////////////////////////////////////////////////
// Global constants.
// ////////////////////////////////////////////////////////////////////////////
/// Server version.
static const std::string kVersion = "15";
/// What room they start in.
static const int kInitialRoom = 1000;
/// Times they can try a password.
static const int kMaxPasswordAttempts = 3;
/// Mud weight measure.
static const std::string mud_measure = "lbs";
/// The maximum value of byte sent at time.
static const int kMaxStream = 4096;
/// Indicator for no socket connected.
static const int kNoSocketIndicator = -1;
/// Database file name.
static const std::string kDatabaseName = "radmud.db";
/// Location of system files.
static const std::string kSystemDir = "../system/";

