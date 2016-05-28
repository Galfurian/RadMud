/// @file   main.cpp
/// @brief  It's the main file, from here we set up and keep running a MUD.
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
// Local Includes.
#include <time.h>
#include <chrono>

#include "commands.hpp"
#include "mud.hpp"
// Other Include.
#include "utils.hpp"

/// Prints the mud logo.
void PrintLogo();
/// Initialize the mud variables.
int InitMud();
/// Routine which saves the mud data.
int SaveMud();
/// Routine which deletes all the dynamically allocated objects.
int FreeMudMemory();

/// @brief  It's the main program.
/// @param argc The number of parameters.
/// @param argv The parameters.
/// @return Errore code.
int main(int argc, char ** argv)
{
    Stopwatch<std::chrono::milliseconds> stopwatch("Main");
    double timeBoot;
    double timeUptime;
    double timeShutdown;

    PrintLogo();

    ///////////////////////////////////////////////////////////////////////////
    if (argc != 0)
    {
        for (int it = 0; it < argc; it++)
        {
            std::cout << argv[it] << std::endl;
        }
    }

    stopwatch.start();

    ///////////////////////////////////////////////////////////////////////////
    LogMessage(kMSys, "Booting RadMud...");
    LogMessage(kMSys, "    Loading Directions...");
    Mud::getInstance().addDirection("north", Direction::North);
    Mud::getInstance().addDirection("south", Direction::South);
    Mud::getInstance().addDirection("west", Direction::West);
    Mud::getInstance().addDirection("east", Direction::East);
    Mud::getInstance().addDirection("up", Direction::Up);
    Mud::getInstance().addDirection("down", Direction::Down);
    LogMessage(kMSys, "    Loading Commands...");
    LoadCommands();
    LogMessage(kMSys, "    Loading States...");
    LoadStates();
    LogMessage(kMSys, "    Opening Database...");
    if (!Mud::getInstance().getDbms().openDatabase())
    {
        LogError("Error opening database!");
        return 1;
    }
    LogMessage(kMSys, "    Loading Tables...");
    if (!Mud::getInstance().getDbms().loadTables())
    {
        LogMessage(kMSys, "Error loading tables!");
        return 1;
    }
    LogMessage(kMSys, "    Initializing Communications...");
    if (!Mud::getInstance().initComunications())
    {
        LogError("Something gone wrong during initialization of comunication.");
        return 1;
    }

    LogMessage(kMSys, "    Initializing Mud Variables...");
    if (InitMud())
    {
        LogError("Something gone wrong during initialization of mud.");
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    timeBoot = stopwatch.elapsed();
    LogMessage(kMSys, "Booting Done (" + ToString(timeBoot) + ").");
    LogMessage(kMSys, "");

    // Start the main loop which handle the players input/output.
    stopwatch.start();
    Mud::getInstance().mainLoop();
    timeUptime = stopwatch.stop();

    ///////////////////////////////////////////////////////////////////////////
    // Shutting down the mud.
    LogMessage(kMSys, "");
    LogMessage(kMSys, "Shutting down RadMud...");
    stopwatch.start();

    // Stop listening.
    LogMessage(kMSys, "    Closing Communications...");
    Mud::getInstance().closeComunications();

    // Saving informations.
    SaveMud();

    // Freeing memory.
    FreeMudMemory();

    // Close database.
    LogMessage(kMSys, "    Closing Database...");
    if (!Mud::getInstance().getDbms().closeDatabase())
    {
        LogError("The database has not been closed correctly.");
    }

    timeShutdown = stopwatch.stop();
    LogMessage(kMSys, "Shutdown Completed.");
    ///////////////////////////////////////////////////////////////////////////
    size_t bIn = Mud::getInstance().getUpdater().bandwidth_in;
    size_t bOut = Mud::getInstance().getUpdater().bandwidth_out;
    size_t bUnc = Mud::getInstance().getUpdater().bandwidth_uncompressed;

    // Print some statistics.
    LogMessage(kMSys, "");
    LogMessage(kMSys, "Statistics");
    LogMessage(kMSys, "    Boot      : " + ToString(timeBoot) + " seconds.");
    LogMessage(kMSys, "    Uptime    : " + ToString(timeUptime) + " seconds.");
    LogMessage(kMSys, "    Shutdown  : " + ToString(timeShutdown) + " seconds.");
    LogMessage(kMSys, "    Bandwidth :");
    LogMessage(kMSys, "        In            = " + ToString(bIn) + " Bytes.");
    LogMessage(kMSys, "        Output        = " + ToString(bOut) + " Bytes.");
    LogMessage(kMSys, "        Uncompressed  = " + ToString(bUnc) + " Bytes.");
    LogMessage(kMSys, "        Band. Saved   = " + ToString(bUnc - bOut) + " Bytes.");
    LogMessage(kMSys, "");
    LogMessage(kMSys, "Goodbye!");
    LogMessage(kMSys, "#--------------------------------------------#");
    return 0;
}

void PrintLogo()
{
    LogMessage(kMSys, "#--------------------------------------------#");
    LogMessage(kMSys, "                 XXXXXXXXXXXXX                ");
    LogMessage(kMSys, "      /'--_###XXXXXXXXXXXXXXXXXXX###_--'\\    ");
    LogMessage(kMSys, "      \\##/#/#XXXXXXXXXXXXXXXXXXXXX#\\#\\##/  ");
    LogMessage(kMSys, "       \\/#/#XXXXXXXXXXXXXXXXXXXXXXX#\\#\\/   ");
    LogMessage(kMSys, "        \\/##XXXXXXXXXXXXXXXXXXXXXXX##\\/     ");
    LogMessage(kMSys, "         ###XXXX  ''-.XXX.-''  XXXX###        ");
    LogMessage(kMSys, "           \\XXXX               XXXX/         ");
    LogMessage(kMSys, "             XXXXXXXXXXXXXXXXXXXXX            ");
    LogMessage(kMSys, "             XXXX XXXX X XXXX XXXX            ");
    LogMessage(kMSys, "             XXX # XXX X XXX # XXX            ");
    LogMessage(kMSys, "            /XXXX XXX XXX XXX XXXX\\          ");
    LogMessage(kMSys, "           ##XXXXXXX X   X XXXXXXX##          ");
    LogMessage(kMSys, "          ##   XOXXX X   X XXXOX   ##         ");
    LogMessage(kMSys, "          ##    #XXXX XXX XXX #    ##         ");
    LogMessage(kMSys, "           ##..##  XXXXXXXXX  ##..##          ");
    LogMessage(kMSys, "            ###      XXXXX     ####           ");
    LogMessage(kMSys, "#--------------------------------------------#");
    LogMessage(kMSys, "|                   RadMud                   |");
    LogMessage(kMSys, "| Created by : Enrico Fraccaroli.            |");
    LogMessage(kMSys, "| Date       : 29 September 2014             |");
    LogMessage(kMSys, "#--------------------------------------------#");
}
/// Initialize the mud.
int InitMud()
{
    Mud::getInstance().getUpdater().initTimers();
    unsigned int sSize = Mud::getInstance().getUpdater().secondSize;
    unsigned int hSize = Mud::getInstance().getUpdater().hourSize;
    unsigned int dSize = Mud::getInstance().getUpdater().daySize;
    LogMessage(kMSys, "                                              ");
    LogMessage(kMSys, "Mud Informations");
    LogMessage(kMSys, "    Port          : " + ToString(kPort));
    LogMessage(kMSys, "    Second Length : " + ToString(sSize) + " ms");
    LogMessage(kMSys, "    Hour   Length : " + ToString(hSize) + " ms (" + ToString(hSize / 1000) + " s)");
    LogMessage(kMSys, "    Day    Length : " + ToString(dSize) + " ms (" + ToString(dSize / 1000) + " s)");
    return 0;
}

/// Save the mud informations.
int SaveMud()
{
    Mud::getInstance().getDbms().beginTransaction();
    LogMessage(kMSys, "    Saving information on Database for : Players...");
    Mud::getInstance().savePlayers();
    LogMessage(kMSys, "    Saving information on Database for : Items...");
    Mud::getInstance().saveItems();
    LogMessage(kMSys, "    Saving information on Database for : Rooms...");
    Mud::getInstance().saveRooms();
    Mud::getInstance().getDbms().endTransaction();
    return 0;
}

/// Delete all the instantiated classes in order to free used memory.
int FreeMudMemory()
{
    LogMessage(kMSys, "    Freeing memory occupied by: Players...");
    for (auto iterator : Mud::getInstance().mudPlayers)
    {
        delete (iterator);
    }

    LogMessage(kMSys, "    Freeing memory occupied by: Mobiles...");
    for (auto iterator : Mud::getInstance().mudMobiles)
    {
        delete (iterator.second);
    }

    LogMessage(kMSys, "    Freeing memory occupied by: Items...");
    for (auto iterator : Mud::getInstance().mudItems)
    {
        delete (iterator);
    }

    LogMessage(kMSys, "    Freeing memory occupied by: Rooms...");
    for (auto iterator : Mud::getInstance().mudRooms)
    {
        delete (iterator.second);
    }

    LogMessage(kMSys, "    Freeing memory occupied by: Areas...");
    for (auto iterator : Mud::getInstance().mudAreas)
    {
        delete (iterator.second);
    }

    LogMessage(kMSys, "    Freeing memory occupied by: Writings...");
    for (auto iterator : Mud::getInstance().mudWritings)
    {
        delete (iterator.second);
    }

    LogMessage(kMSys, "    Freeing memory occupied by: Corpses...");
    for (auto iterator : Mud::getInstance().mudCorpses)
    {
        delete (iterator);
    }

    return 0;
}
