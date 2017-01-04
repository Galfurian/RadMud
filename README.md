![logo](./doc/logo.png)
# RadMud [![Build Status](https://travis-ci.org/Galfurian/RadMud.svg?branch=master)](https://travis-ci.org/Galfurian/RadMud)
**Author**: Enrico Fraccaroli

**Date**: 29 September 2014

**Language**: C++

  1. [Introduction](https://github.com/Galfurian/RadMud/blob/master/README.md#introduction)
  2. [Features](https://github.com/Galfurian/RadMud/blob/master/README.md#features)
  3. [Requirements](https://github.com/Galfurian/RadMud/blob/master/README.md#requirements)
  4. [Compile](https://github.com/Galfurian/RadMud/blob/master/README.md#compile)
  5. [Execute](https://github.com/Galfurian/RadMud/blob/master/README.md#execute)
  6. [Connection](https://github.com/Galfurian/RadMud/blob/master/README.md#connection)

## Introduction
The idea is to create a Multi User Dungeon (MUD) set in a post-nuclear world.
Right now there is a minimal content since all the development effort is used to implement new features.
The philosophy behind each choice is to **make the MUD as much customizable as possible** (and **easily**) from players.
Also the idea is to provide to a Master the means to prepare quests, create special NPCs and so forth, online without re-starting the MUD.
This feature is **already implemented**, by means of a **lua interface** with the structures of the MUD.
Such scripts have access to the information about the objects/npcs/rooms through specific functions, **preventing any misuse or mistake**.

## Features
Follow the **main features** which are currently implemented
 - An **item system** which allows to easily manage items.
 - A **scripting system** which allows lua scripts to interract with the MUD.
 - A **crafting systems** has already been created, however there is still room for improvement.
 - A **hierarchical structure** for the MUD with four main levels (world, continent, area, room).
 - An **economy system** with shops and currencies.

## Requirements
In order to compile the mud the following libraries need to be installed:
 - sqlite3 (For offline data storage).
 - lua (For npc behaviour).
 - zlib (For compressed communication).

## Compile
In order to compile the mud just create a build directory and then use the cmake file provided.
Therefore, the commands are the following:
```
mkdir build
cd build
[optional] export CXX=clang++
cmake ..
make
```

## Execute
After compiling the mud, just execute the generated executable
```
./RadMud
```

## Connection
The mud can be accessed using telnet with the port **4000**.
```
telnet localhost 4000
```
