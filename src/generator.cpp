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
    auxiliary_vector = std::vector<std::string>();
    zones_names = std::map<std::string, std::vector<std::string> >();
    status_names = std::map<std::string, std::vector<std::string> >();
    zones_description = std::map<std::string, std::vector<std::string> >();
    status_description = std::map<std::string, std::vector<std::string> >();

    // NAMES
    // CAVERN
    auxiliary_vector.push_back("A narrow cave");
    auxiliary_vector.push_back("A small cavern");
    auxiliary_vector.push_back("A cave");
    auxiliary_vector.push_back("A natural cave");
    auxiliary_vector.push_back("A hole in the earth");
    auxiliary_vector.push_back("A large chamber");
    auxiliary_vector.push_back("A grotto");
    auxiliary_vector.push_back("A cavern");
    zones_names.insert(make_pair("cavern", auxiliary_vector));
    auxiliary_vector.clear();

    // UNDERDARK
    auxiliary_vector.push_back("A dark cave");
    auxiliary_vector.push_back("A cavity");
    auxiliary_vector.push_back("An open space");
    auxiliary_vector.push_back("A dark cave");
    auxiliary_vector.push_back("Somewhere far beyond");
    zones_names.insert(make_pair("underdark", auxiliary_vector));
    auxiliary_vector.clear();

    // STATUS

    // NORMAL
    auxiliary_vector.push_back(".");
    auxiliary_vector.push_back(".");
    auxiliary_vector.push_back(", realy dark.");
    auxiliary_vector.push_back(", digged by water.");
    auxiliary_vector.push_back(", made of marble stone.");
    status_names.insert(make_pair("normal", auxiliary_vector));
    auxiliary_vector.clear();

    // ICED
    auxiliary_vector.push_back(".");
    auxiliary_vector.push_back(".");
    auxiliary_vector.push_back(", completely frozen.");
    auxiliary_vector.push_back(", covered in ice.");
    auxiliary_vector.push_back(", cold as death.");
    status_names.insert(make_pair("iced", auxiliary_vector));
    auxiliary_vector.clear();

    // MAGMATIC
    auxiliary_vector.push_back(", crossed by lava.");
    auxiliary_vector.push_back(", hot as sun.");
    auxiliary_vector.push_back(", containing a lava pond.");
    status_names.insert(make_pair("magmatic", auxiliary_vector));
    auxiliary_vector.clear();

    // VEGETAL
    auxiliary_vector.push_back(", infested by vegetation.");
    status_names.insert(make_pair("vegetal", auxiliary_vector));
    auxiliary_vector.clear();

    // DESCRIPTION
    // CAVERN
    auxiliary_vector.push_back(
        "You have just entered @ROOM_NAME when you hear the distant noise of some animal, maybe his sleep is over. ");
    auxiliary_vector.push_back("You are in @ROOM_NAME in some places the walls have metal veins.");
    auxiliary_vector.push_back(
        "The sound of your footsteps is magnified as you advance in the dark, in addition to the sound of your breath, they are the only sounds that you can perceive.");
    auxiliary_vector.push_back(
        "You've just come out of a crack, you look around for a while, you're finished in @ROOM_NAME.");
    zones_description.insert(make_pair("cavern", auxiliary_vector));
    auxiliary_vector.clear();

    // UNDERDARK
    auxiliary_vector.push_back(
        "You look around, you do not know how you ended up here. So you're in @ROOM_NAME and now better if you think about where to go.");
    auxiliary_vector.push_back(
        "The superhuman cry of some animal comes from a small niche within walking distance from you. You turn around, but did not see anything, maybe now is behind you.");
    auxiliary_vector.push_back(
        "Your senses warn you of a danger, look to the side and see a rock wall covered with cracks, looking better see inside many small eyes.");
    auxiliary_vector.push_back(
        "When you enter in @ROOM_NAME and the darkness swallows you. You make a few steps and nothing changes, indeed, beyond the darkness there is now a deathly silence.");
    zones_description.insert(make_pair("underdark", auxiliary_vector));
    auxiliary_vector.clear();

    // STATUS DESCRIPTION
    // NORMAL
    auxiliary_vector.push_back("");
    auxiliary_vector.push_back("");
    auxiliary_vector.push_back("");
    auxiliary_vector.push_back("You do not see anything special here.");
    auxiliary_vector.push_back("You feel observed, only that we were missing.");
    auxiliary_vector.push_back("Your journey continues undisturbed, it would be nice if it was always like this!");
    status_description.insert(make_pair("normal", auxiliary_vector));
    auxiliary_vector.clear();

    // ICED
    auxiliary_vector.push_back(
        "The temperature has dropped sharply, you cold chills the blood begins to flow fatigue in your veins.");
    auxiliary_vector.push_back(
        "A layer of frost covering everything, the air itself is permeated by a thin blanket of snow that due to the small streams of wind never drops to the ground.");
    auxiliary_vector.push_back(
        "You're walking on a small sheet of ice, that's when you crack making you fall into a small puddle of icy water, it could not get any worse '.");
    status_description.insert(make_pair("iced", auxiliary_vector));
    auxiliary_vector.clear();

    // MAGMATIC
    auxiliary_vector.push_back(
        "A cascade of lava lights up the room slightly, this does not help you a lot to see, but the heat here makes it tiring to continue.");
    auxiliary_vector.push_back(
        "A river of lava passes under your feet as you advance on a natural bridge, you hope it lasts.");
    status_description.insert(make_pair("magmatic", auxiliary_vector));
    auxiliary_vector.clear();

    // VEGETAL
    auxiliary_vector.push_back(
        "The floor is completely covered with tendrils that intertwine with each other, making you difficult to continue.");
    auxiliary_vector.push_back(
        "Plants arise from cracks on the floor, stretched and shrunken always looking for a bit of light. did not think a plant could do all this.");
    status_description.insert(make_pair("vegetal", auxiliary_vector));
    auxiliary_vector.clear();
}

Generator::~Generator()
{
    Logger::log(LogLevel::Debug, "Deleted: Generator.");
}

void Generator::generate(int phase, string zone, string status)
{
    if (auxiliary_vector.size() == 0)
    {
        return;
    }
    unsigned int choices = static_cast<unsigned int>(auxiliary_vector.size());
    unsigned int randomSeed = TRandInteger<unsigned int>(0, choices - 1);
    switch (phase)
    {
        default:
            break;
        case 1:
            if (zones_names.find(zone) != zones_names.end())
            {
                name = zones_names[zone][randomSeed];
            }
            else
            {
                Logger::log(LogLevel::Error, "Error during phase ONE of room generation.");
            }
            break;
        case 2:
            if (status_names.find(status) != status_names.end())
            {
                name_status = status_names[status][randomSeed];
            }
            else
            {
                Logger::log(LogLevel::Error, "Error during phase TWO of room generation.");
            }

            break;
        case 3:
            if (zones_description.find(zone) != zones_description.end())
            {
                description = zones_description[zone][randomSeed];
            }
            else
            {
                Logger::log(LogLevel::Error, "Error during phase THREE of room generation.");
            }
            break;
        case 4:
            if (status_description.find(status) != status_description.end())
            {
                description = status_description[status][randomSeed];
            }
            else
            {
                Logger::log(LogLevel::Error, "Error during phase FOUR of room generation.");
            }
            break;
    }
}

pair<string, string> Generator::generateRoom(string d_zone, string d_status)
{
    // Phase 1:
    generate(1, d_zone, d_status);

    // Phase 2:
    generate(2, d_zone, d_status);

    // Phase 3:
    generate(3, d_zone, d_status);

    // Phase 4:
    generate(4, d_zone, d_status);

    FindAndReplace(description, "@ROOM_NAME", ToLower(name));

    return make_pair(name + name_status, description);
}
