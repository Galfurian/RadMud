# RadMud
______________

**Author**: Enrico Fraccaroli

**Date**: 29 September 2014

**Language**: C++

______________
### Description
It's a Post Nuclear Multi User Dungeon made in C++.

______________
### Installation

The necessary libraries, in order to compile are:

 * **pthread**
 * **sqlite3**
 * **lua**
 * **zlib**

______________
### Execution
Run the server like this:

    ./RadMud &

______________
### Connecting
Currently the only way to connect to the server is by using the client which I've written ad-hoc.
It can be found at:
https://github.com/Galfurian/RadClient

______________
### Change Log - For Developers
#### Version 17 (14/09/2015)
 * **UPGRADE** [**Items**]
 	* We have achieved a stable items system, which allow to define for each item features like the material with which they have been made, its quality which influences a lot of its interactions with all the environment.
 * **UPGRADE** [**Visual Studio**]
    * The first step towards compatibility with Visual Studio has been made, now it's possible to compile the mud on Visual Studio.
 * **UPGRADE** [**Writings**]
 	* It's now possible in the mud to create writings on items, for now players are not allowed to create them, but soon will be implemented a related command.
 * **UPGRADE** [**Character Creation**]
 	* Character creation procedure has been upgraded, now it's more user friendly and more simple to upgrade.
 * **UPGRADE** [**Continents**]
 	* Continents have been developed, they are at an early stage so there are more implementations which need to be done.
 * **UPGRADE** [**Professions and Productions**]
 	* Professions and their products has been developed, not completely but the mechanism is only to refine now.
 * **UPGRADE** [**Liquids**]
 	* Liquids has been developed.

#### Version 16 (23/05/2015)
 * **UPGRADE** [**Room**]
    * Introduced room's connections above or below but without the presence of some stairs these rooms are unaccessible.
 * **UPGRADE** [**Gods**]
    * Introduced gods invisibility.
 * **UPGRADE** [**Models**]
    * Models flags on database has been re-arranged in order to simplify the understanding of the functionality.
 * **UPGRADE** [**Items**]
    * Improved item lists with counter.
 * **UPGRADE** [**Database**]
    * Database operations have been improved.

#### Version 15 (08/04/2015)
 * **ADD** [**EventEntrace**]
    * Implemented the EventEntrace for mobiles, so every time a character enter the room, a mobile execute the routine.
 * **ADD** [**Statistics**]
    * Variables for tracing all the components of the MUD (qt_items, qt_mobiles, uptime, ...) has been added.
 * **ADD** [**Lua**]
    * Introduced CheckSameRoom in lua, so no action take place if the mobile and the character are in different rooms.
 * **UPGRADE** [**Area**]
    * Made areas used as containers of rooms, allowing a performant access via coordinates or exhaustive search. Introduced the concept of boundaries of areas, beyond which there can't be nothing.
 * **UPGRADE** [**Item**]
    * Separated models from items. This way every Item has a pointer to a model which is shared with other items of the same kind.
 * **UPGRADE** [**Character Creation**]
    * Player creation has been completely rewritten.
 * **FIX** [**Time Evolution**]
    * Boundaries has been created in order to manage the Time evolution of the MUD.
    * The current configuration is:
        * Tick Length = 50ms
        * Hour Length = 3m
        * Day  Length = 1h12m
 * **FIX** [**Item**]
    * Fixed and renewed item system.
 * **FIX** [**Memory Leakage**]
    * The current update removes all the memory leakage. New developments of the MUD must take in great consideration the Runtime Memory Leakage due to bad programming, and prevent it happens.

#### Version 14 (23/03/2015)
 * **ADD** [**FOV & LOS**]
    * Implemented a **Field of View** algorithm that provide a radius of view based on perception of the player.
    * Implemented a **Line of Sight** algorithm.
 * **UPGRADE** [**Room**]
    * Exits now hold pointer to destination room.
    * CreateRoom command now works properly.
 * **UPGRADE** [**SQLite**]
    * Simplified the mud loading logs in SQLiteDbms.

#### Version 13 (22/03/2015)
 * **ADD** [**Areas**]
    * Introduced area table and linked all the rooms to an area.
 * **ADD** [**Skills Commands**]
    * Introduced command need to view skills and to modify their value.
 * **ADD** [**Utilities**] (Added new utilities functions)
    * One for generating a divider with customized char and lenght.
    * One for generating a string with a centered string in a given lenght.
    * One for printing Attrbiute name.
 * **UPGRADE** [**Item**] (Many modifications regarding the item system)
    * Developed a new item system.
    * Remove the Armor, Weapon and Container class.
    * Implemented a smarter way to store weapon position.
    * Modified the naming and the mechanism, used by character to store equipped equipment and posessed items.
 * **UPGRADE** [**Actions**]
    * Character actions counter is now a struct instead of a map.
 * **UPGRADE** [**Commands**] (Many modifications regarding commands)
    * Incorporated inside help function, the command "commands" and extended to support skills informations.
    * Improved all the commands which show a list.
    * Fixed Equipments command and equipment list on player.
 * **UPGRADE** [**SQLite**] (Upgraded completely the SQLite mechanism)
    * Moved all the files concerning SQLite in a sub-folder.
    * Introduced the SQLiteWrapper, to execute better query.
    * Subdivided into methods every loads of data from database table.
 * **ROLLBACK** [**Combat**]
    * Completely removed combat, we need to focus on other minor things first.
 * **FIX** [**Mobile**]
    * Fixed and upgraded mobile death, upgraded some lists.
 * **FIX** [**Comments**]
    * Updated all the comments.

#### Version 12 (03/01/2015)
 * **ADD** [**Stances**] (Introduced player stances)
    - **Sit** and **Rest** allow player to regain health/stamina faster.
    - **Crouch** will allow player to move stealthily.
    - **Prone** allow to shoot better and far.
 * **ADD** [**Room Flag**]
    - Introduced room flags, now with a simple BitMask a God can set a specific room flag easily.
 * **UPGRADE** [**Items**]
    - Now player can look at an object inside another object or dressed on another player.
 * **FIX** [**Update Loop**]
    - Now the update loop, during the login after entering the username, doesn't take in consideration the player if he hasn't successfully entered the world.

#### Version 11 (30/12/2014)
 * **ADD** [**Lua Scripting**]
    - Now it's possible to describe the behavior of a mobile with a much simpler lua script. That allow to separate what is the Code-Base of the server (less prone to change) from whats is the environment of the Mud (always in evolution, along with the players of the server).
 * **UPGRADE** [**Windows**]
    - Introduced inside the Makefile an option that allow to compile the project under Windows with Cygwin.
 * **UPGRADE** [**Character**]
    - Now both Players and Mobiles are sub-class of Character, a class that holds information shared by both the sub-class allowing the execution of the DoFunctions by Mobiles.

#### Version 10 (20/09/2014)
 * **ADD** [**Containers**]
    - Introduced the items of type **Containers**.
 * **UPGRADE** [**Inventory**]
    - Player's Inventory management, including Weight Constrains, Better listing and more.
 * **UPGRADE** [**Broadcasting**]
    - Message broadcasting has been spitted into level Room-Broadcasting and Mud-Broadcasting.
 * **UPGRADE** [**Player's Look**]
    - Now looking to a player show his equipment.
 * **FIX** [**Deconstruct**]
    - Item deconstruct now properly update the item information on database.
 * **FIX** [**Objects Interaction**]
    - Wear, Wield and Remove have been fixed, now they works perfectly.

#### Version 9 (18/09/2014)
 * **ADD** - Command for organize the rooms item(by name).
 * **ADD** - Mobile(mob now and one day npcs maybe).
 * **UPGRADE** - Periodic updates systems now handle separately mobiles, items and players.
 * **UPGRADE** - New exit system, now exits can deny passage to mobile, can be hidden, open and closed. Actually they can be blocked but the code to handle keys it's not implemented yet, for now a blocked door it's an inaccessible door.

#### Version 8 (15/09/2014)
 * **ADD** - Player/Item condition.
 * **ADD** - Advanced Prompt.
 * **UPGRADE** - Commands are now divided into multiple files divided by categories.
 * **FIX** -  Room generator.
 * **FIX** - some commands.
 * **FIX** - Makefile.

#### Version 7 (12/09/2014)
 * **ADD** - Two new commands needed to create and list rooms.
 * **FIX** - Corrected actions timing.
 * **FIX** - Corrected new room creation and inter-connection.
 
#### Version 6 (12/09/2014)
 * **UPGRADE** - Moved all storage system to : **sqlite3**.

#### Version 5 (06/09/2014)
 * **ADD** - Implemented a Room Generator.
 * **ADD** - Implemented command to mine new rooms.

#### Version 4 (04/09/2014)
 * **ADD** - Player's equipment.
 * **ADD** - Introduced commands: equip, remove.
 * **ADD** - Methods to find items and player in rooms.
 * **ADD** - Gathering system: mine, chop.
 * **ADD** - Resources node : iron deposit, tree.
 * **ADD** - Resources : iron ore, wood branch.
 * **ADD** - Delay on moving.
 * **ADD** - Coordinates to Mud map.
 * **FIX** - Carriage return to all message directer to player, improved compatibility with clients.

#### Version 3 (29/08/2014)
 * **ADD** - More interaction between players: object passing, whispers, saying something.
 * **ADD** - Logging events (Eg. Connections, Disconnections, Faults).
 * **ADD** - Dynamic room item list, now it non need to scan all the items in the MUD but just the ones in the room.
 * **ADD** - Advanced player's action control: Eg. delay on moving, possibility to add crafting, and more.
 * **ADD** - More function need to handle strings and various objects.
 * **UPGRADE** - Input prefix parse. (Eg. "lo" captured as "look").
 * **UPGRADE** - Dynamic item load, now can define different files for different group of items.

#### Version 1 (26/08/2014)
 * **ADD** - Colored/Formatted text.
 * **ADD** - Object.
 * **ADD** - Commands needed to Take, Drop items.
 * **ADD** - Player's Inventory.
 * **ADD** - A method to find items by vnum.
 * **UPGRADE** - Look command, now you can look to objects and directions.
 * **UPGRADE** - Help command, now it's generated by code.
 * **UPGRADE** - Player file Save and Load.

#### Version 0 (21/08/2014)
 * **ADD** - Accepts multiple connections from players.
 * **ADD** - Maintains a list of connected players.
 * **ADD** - Asks players for a name and password.
 * **ADD** - Saves player files to disk (name, password, current room, player flags).
 * **ADD** - Implements the commands: **quit**, **look**, **say**, **tell**, **help**, **goto**, **transfer**, **shutdown**, **setflag**, **clearflag**.
 * **ADD** - Implements movement commands.
 * **ADD** - Illustrates sending messages to a single player (eg. a tell) or all players (eg. a say).
 * **ADD** - Handles players disconnecting or quitting.
 * **ADD** - Periodic messages.
 * **ADD** - Loads room descriptions and exits from a disk file
 * **ADD** - Loads messages from a disk file
 * **ADD** - Loads control parameters from a disk file
 
______________
### No Warranty
This software is provided "as is" without express or implied warranty, and with no claim as to its suitability for any purpose.
