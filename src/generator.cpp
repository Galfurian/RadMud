/// @file   generator.cpp
/// @brief  Implements the methods used to generate objects.
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

// Basic Include.
#include "generator.hpp"

// Local Includes.

// Other Include.
#include "utils.hpp"
#include "logger.hpp"
#include "constants.hpp"

using namespace std;

Generator::Generator()
{
    // NAMES
    // CAVERN
    mapBase[AreaType::Cavern].name = "cavern";
    mapBase[AreaType::Cavern].names.push_back("A narrow cave");
    mapBase[AreaType::Cavern].names.push_back("A narrow cave");
    mapBase[AreaType::Cavern].names.push_back("A small cavern");
    mapBase[AreaType::Cavern].names.push_back("A cave");
    mapBase[AreaType::Cavern].names.push_back("A natural cave");
    mapBase[AreaType::Cavern].names.push_back("A hole in the earth");
    mapBase[AreaType::Cavern].names.push_back("A large chamber");
    mapBase[AreaType::Cavern].names.push_back("A grotto");
    mapBase[AreaType::Cavern].names.push_back("A cavern");
    mapBase[AreaType::Cavern].descriptions.push_back(
        "You have just entered @ROOM_NAME when you hear the distant noise of some animal, maybe his sleep is over. ");
    mapBase[AreaType::Cavern].descriptions.push_back("You are in @ROOM_NAME in some places the walls have metal veins.");
    mapBase[AreaType::Cavern].descriptions.push_back(
        "The sound of your footsteps is magnified as you advance in the dark, in addition to the sound of your breath, they are the only sounds that you can perceive.");
    mapBase[AreaType::Cavern].descriptions.push_back(
        "You've just come out of a crack, you look around for a while, you're finished in @ROOM_NAME.");

    // UNDERDARK
    mapBase[AreaType::Underdark].name = "underdark";
    mapBase[AreaType::Underdark].names.push_back("A dark cave");
    mapBase[AreaType::Underdark].names.push_back("A cavity");
    mapBase[AreaType::Underdark].names.push_back("An open space");
    mapBase[AreaType::Underdark].names.push_back("A dark cave");
    mapBase[AreaType::Underdark].names.push_back("Somewhere far beyond");
    mapBase[AreaType::Underdark].descriptions.push_back(
        "You look around, you do not know how you ended up here. So you're in @ROOM_NAME and now better if you think about where to go.");
    mapBase[AreaType::Underdark].descriptions.push_back(
        "The superhuman cry of some animal comes from a small niche within walking distance from you. You turn around, but did not see anything, maybe now is behind you.");
    mapBase[AreaType::Underdark].descriptions.push_back(
        "Your senses warn you of a danger, look to the side and see a rock wall covered with cracks, looking better see inside many small eyes.");
    mapBase[AreaType::Underdark].descriptions.push_back(
        "When you enter in @ROOM_NAME and the darkness swallows you. You make a few steps and nothing changes, indeed, beyond the darkness there is now a deathly silence.");

    // STATUS
    // NORMAL
    mapStatus[AreaStatus::Normal].name = "normal";
    mapStatus[AreaStatus::Normal].namesStatus.push_back(".");
    mapStatus[AreaStatus::Normal].namesStatus.push_back(", realy dark.");
    mapStatus[AreaStatus::Normal].namesStatus.push_back(", digged by water.");
    mapStatus[AreaStatus::Normal].namesStatus.push_back(", made of marble stone.");
    mapStatus[AreaStatus::Normal].descriptionStatus.push_back("You do not see anything special here.");
    mapStatus[AreaStatus::Normal].descriptionStatus.push_back("You feel observed, only that we were missing.");
    mapStatus[AreaStatus::Normal].descriptionStatus.push_back(
        "Your journey continues undisturbed, it would be nice if it was always like this!");

    // ICED
    mapStatus[AreaStatus::Iced].name = "iced";
    mapStatus[AreaStatus::Iced].namesStatus.push_back(".");
    mapStatus[AreaStatus::Iced].namesStatus.push_back(".");
    mapStatus[AreaStatus::Iced].namesStatus.push_back(", completely frozen.");
    mapStatus[AreaStatus::Iced].namesStatus.push_back(", covered in ice.");
    mapStatus[AreaStatus::Iced].namesStatus.push_back(", cold as death.");
    mapStatus[AreaStatus::Iced].descriptionStatus.push_back(
        "The temperature has dropped sharply, you cold chills the blood begins to flow fatigue in your veins.");
    mapStatus[AreaStatus::Iced].descriptionStatus.push_back(
        "A layer of frost covering everything, the air itself is permeated by a thin blanket of snow that due to the small streams of wind never drops to the ground.");
    mapStatus[AreaStatus::Iced].descriptionStatus.push_back(
        "You're walking on a small sheet of ice, that's when you crack making you fall into a small puddle of icy water, it could not get any worse '.");

    // MAGMATIC
    mapStatus[AreaStatus::Magmatic].name = "magmatic";
    mapStatus[AreaStatus::Magmatic].namesStatus.push_back(", crossed by lava.");
    mapStatus[AreaStatus::Magmatic].namesStatus.push_back(", hot as sun.");
    mapStatus[AreaStatus::Magmatic].namesStatus.push_back(", containing a lava pond.");
    mapStatus[AreaStatus::Magmatic].descriptionStatus.push_back(
        "A cascade of lava lights up the room slightly, this does not help you a lot to see, but the heat here makes it tiring to continue.");
    mapStatus[AreaStatus::Magmatic].descriptionStatus.push_back(
        "A river of lava passes under your feet as you advance on a natural bridge, you hope it lasts.");

    // VEGETAL
    mapStatus[AreaStatus::Vegetal].name = "vegetal";
    mapStatus[AreaStatus::Vegetal].namesStatus.push_back(", infested by vegetation.");
    mapStatus[AreaStatus::Vegetal].descriptionStatus.push_back(
        "The floor is completely covered with tendrils that intertwine with each other, making you difficult to continue.");
    mapStatus[AreaStatus::Vegetal].descriptionStatus.push_back(
        "Plants arise from cracks on the floor, stretched and shrunken always looking for a bit of light. did not think a plant could do all this.");
}

Generator::~Generator()
{
    Logger::log(LogLevel::Debug, "Deleted: Generator.");
}

Generator & Generator::instance()
{
    // Since it's a static variable, if the class has already been created,
    // It won't be created again. And it **is** thread-safe in C++11.
    static Generator instance;
    // Return a reference to our instance.
    return instance;
}

std::string Generator::generateName(const AreaType & type, const AreaStatus & status)
{
    size_t nameSelector = TRandInteger<size_t>(0, mapBase[type].names.size() - 1);
    size_t nameStatus = TRandInteger<size_t>(0, mapStatus[status].namesStatus.size() - 1);
    return mapBase[type].names[nameSelector] + mapStatus[status].namesStatus[nameStatus];
}

std::string Generator::generateDescription(const AreaType & type, const AreaStatus & status, const std::string & roomName)
{
    size_t nameSelector = TRandInteger<size_t>(0, mapBase[type].descriptions.size() - 1);
    size_t nameStatus = TRandInteger<size_t>(0, mapStatus[status].descriptionStatus.size() - 1);
    std::string result = mapBase[type].descriptions[nameSelector] + mapStatus[status].descriptionStatus[nameStatus];
    FindAndReplace(result, "@ROOM_NAME", ToLower(roomName));
    return result;
}
