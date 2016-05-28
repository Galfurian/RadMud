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
#include "logger.hpp"
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
    // Create a stopwatch for general timing information.
    Stopwatch<std::chrono::milliseconds> stopwatch("Main");
    // Open logging file.
    if (!Logger::instance().openLog(kSystemDir + GetDate() + ".log"))
    {
        std::cerr << "Can't create the logging file." << std::endl;
        exit(1);
    }
    // Print the mud logo.
    PrintLogo();
    // Start the stopwatch.
    stopwatch.start();

    Logger::log(LogLevel::Global, "Booting RadMud...");

    Logger::log(LogLevel::Global, "    Loading Directions...");
    Mud::getInstance().addDirection("north", Direction::North);
    Mud::getInstance().addDirection("south", Direction::South);
    Mud::getInstance().addDirection("west", Direction::West);
    Mud::getInstance().addDirection("east", Direction::East);
    Mud::getInstance().addDirection("up", Direction::Up);
    Mud::getInstance().addDirection("down", Direction::Down);

    Logger::log(LogLevel::Global, "    Loading Commands...");
    LoadCommands();

    Logger::log(LogLevel::Global, "    Loading States...");
    LoadStates();

    Logger::log(LogLevel::Global, "    Opening Database...");
    if (!Mud::getInstance().getDbms().openDatabase())
    {
        LogError("Error opening database!");
        return 1;
    }

    Logger::log(LogLevel::Global, "    Loading Tables...");
    if (!Mud::getInstance().getDbms().loadTables())
    {
        Logger::log(LogLevel::Global, "Error loading tables!");
        return 1;
    }

    Logger::log(LogLevel::Global, "    Initializing Communications...");
    if (!Mud::getInstance().initComunications())
    {
        LogError("Something gone wrong during initialization of comunication.");
        return 1;
    }

    Logger::log(LogLevel::Global, "    Initializing Mud Variables...");
    if (InitMud())
    {
        LogError("Something gone wrong during initialization of mud.");
        return 1;
    }

    ///////////////////////////////////////////////////////////////////////////
    double timeBoot = stopwatch.elapsed();
    Logger::log(LogLevel::Global, "Booting Done (" + ToString(timeBoot) + ").");

    // Start the main loop which handle the players input/output.
    stopwatch.start();
    Mud::getInstance().mainLoop();
    double timeUptime = stopwatch.stop();

    ///////////////////////////////////////////////////////////////////////////
    // Shutting down the mud.
    Logger::log(LogLevel::Global, "Shutting down RadMud...");
    stopwatch.start();

    // Stop listening.
    Logger::log(LogLevel::Global, "    Closing Communications...");
    Mud::getInstance().closeComunications();

    // Saving informations.
    SaveMud();

    // Freeing memory.
    FreeMudMemory();

    // Close database.
    Logger::log(LogLevel::Global, "    Closing Database...");
    if (!Mud::getInstance().getDbms().closeDatabase())
    {
        LogError("The database has not been closed correctly.");
    }

    double timeShutdown = stopwatch.stop();
    Logger::log(LogLevel::Global, "Shutdown Completed.");
    ///////////////////////////////////////////////////////////////////////////
    size_t bIn = Mud::getInstance().getUpdater().bandwidth_in;
    size_t bOut = Mud::getInstance().getUpdater().bandwidth_out;
    size_t bUnc = Mud::getInstance().getUpdater().bandwidth_uncompressed;

    // Print some statistics.
    Logger::log(LogLevel::Global, "");
    Logger::log(LogLevel::Global, "Statistics");
    Logger::log(LogLevel::Global, "    Boot      : " + ToString(timeBoot) + " seconds.");
    Logger::log(LogLevel::Global, "    Uptime    : " + ToString(timeUptime) + " seconds.");
    Logger::log(LogLevel::Global, "    Shutdown  : " + ToString(timeShutdown) + " seconds.");
    Logger::log(LogLevel::Global, "    Bandwidth :");
    Logger::log(LogLevel::Global, "        In            = " + ToString(bIn) + " Bytes.");
    Logger::log(LogLevel::Global, "        Output        = " + ToString(bOut) + " Bytes.");
    Logger::log(LogLevel::Global, "        Uncompressed  = " + ToString(bUnc) + " Bytes.");
    Logger::log(LogLevel::Global, "        Band. Saved   = " + ToString(bUnc - bOut) + " Bytes.");
    Logger::log(LogLevel::Global, "");
    Logger::log(LogLevel::Global, "Goodbye!");
    Logger::log(LogLevel::Global, "#--------------------------------------------#");
    return 0;
}

void PrintLogo()
{
    Logger::log(LogLevel::Global, "#--------------------------------------------#");
    Logger::log(LogLevel::Global, "                 XXXXXXXXXXXXX                ");
    Logger::log(LogLevel::Global, "      /'--_###XXXXXXXXXXXXXXXXXXX###_--'\\    ");
    Logger::log(LogLevel::Global, "      \\##/#/#XXXXXXXXXXXXXXXXXXXXX#\\#\\##/  ");
    Logger::log(LogLevel::Global, "       \\/#/#XXXXXXXXXXXXXXXXXXXXXXX#\\#\\/   ");
    Logger::log(LogLevel::Global, "        \\/##XXXXXXXXXXXXXXXXXXXXXXX##\\/     ");
    Logger::log(LogLevel::Global, "         ###XXXX  ''-.XXX.-''  XXXX###        ");
    Logger::log(LogLevel::Global, "           \\XXXX               XXXX/         ");
    Logger::log(LogLevel::Global, "             XXXXXXXXXXXXXXXXXXXXX            ");
    Logger::log(LogLevel::Global, "             XXXX XXXX X XXXX XXXX            ");
    Logger::log(LogLevel::Global, "             XXX # XXX X XXX # XXX            ");
    Logger::log(LogLevel::Global, "            /XXXX XXX XXX XXX XXXX\\          ");
    Logger::log(LogLevel::Global, "           ##XXXXXXX X   X XXXXXXX##          ");
    Logger::log(LogLevel::Global, "          ##   XOXXX X   X XXXOX   ##         ");
    Logger::log(LogLevel::Global, "          ##    #XXXX XXX XXX #    ##         ");
    Logger::log(LogLevel::Global, "           ##..##  XXXXXXXXX  ##..##          ");
    Logger::log(LogLevel::Global, "            ###      XXXXX     ####           ");
    Logger::log(LogLevel::Global, "#--------------------------------------------#");
    Logger::log(LogLevel::Global, "|                   RadMud                   |");
    Logger::log(LogLevel::Global, "| Created by : Enrico Fraccaroli.            |");
    Logger::log(LogLevel::Global, "| Date       : 29 September 2014             |");
    Logger::log(LogLevel::Global, "#--------------------------------------------#");
}
/// Initialize the mud.
int InitMud()
{
    Mud::getInstance().getUpdater().initTimers();
    unsigned int sSize = Mud::getInstance().getUpdater().secondSize;
    unsigned int hSize = Mud::getInstance().getUpdater().hourSize;
    unsigned int dSize = Mud::getInstance().getUpdater().daySize;
    Logger::log(LogLevel::Global, "                                              ");
    Logger::log(LogLevel::Global, "Mud Informations");
    Logger::log(LogLevel::Global, "    Port          : " + ToString(kPort));
    Logger::log(LogLevel::Global, "    Second Length : " + ToString(sSize) + " ms");
    Logger::log(LogLevel::Global, "    Hour   Length : " + ToString(hSize) + " ms (" + ToString(hSize / 1000) + " s)");
    Logger::log(LogLevel::Global, "    Day    Length : " + ToString(dSize) + " ms (" + ToString(dSize / 1000) + " s)");
    return 0;
}

/// Save the mud informations.
int SaveMud()
{
    Mud::getInstance().getDbms().beginTransaction();
    Logger::log(LogLevel::Global, "    Saving information on Database for : Players...");
    Mud::getInstance().savePlayers();
    Logger::log(LogLevel::Global, "    Saving information on Database for : Items...");
    Mud::getInstance().saveItems();
    Logger::log(LogLevel::Global, "    Saving information on Database for : Rooms...");
    Mud::getInstance().saveRooms();
    Mud::getInstance().getDbms().endTransaction();
    return 0;
}

/// Delete all the instantiated classes in order to free used memory.
int FreeMudMemory()
{
    Logger::log(LogLevel::Global, "    Freeing memory occupied by: Players...");
    for (auto iterator : Mud::getInstance().mudPlayers)
    {
        delete (iterator);
    }

    Logger::log(LogLevel::Global, "    Freeing memory occupied by: Mobiles...");
    for (auto iterator : Mud::getInstance().mudMobiles)
    {
        delete (iterator.second);
    }

    Logger::log(LogLevel::Global, "    Freeing memory occupied by: Items...");
    for (auto iterator : Mud::getInstance().mudItems)
    {
        delete (iterator);
    }

    Logger::log(LogLevel::Global, "    Freeing memory occupied by: Rooms...");
    for (auto iterator : Mud::getInstance().mudRooms)
    {
        delete (iterator.second);
    }

    Logger::log(LogLevel::Global, "    Freeing memory occupied by: Areas...");
    for (auto iterator : Mud::getInstance().mudAreas)
    {
        delete (iterator.second);
    }

    Logger::log(LogLevel::Global, "    Freeing memory occupied by: Writings...");
    for (auto iterator : Mud::getInstance().mudWritings)
    {
        delete (iterator.second);
    }

    Logger::log(LogLevel::Global, "    Freeing memory occupied by: Corpses...");
    for (auto iterator : Mud::getInstance().mudCorpses)
    {
        delete (iterator);
    }

    return 0;
}
