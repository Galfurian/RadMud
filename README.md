<img src="https://github.com/Galfurian/RadMud/blob/master/doc/logo.png" align="left" />
# RadMud [![Build Status](https://travis-ci.org/Galfurian/RadMud.svg?branch=master)](https://travis-ci.org/Galfurian/RadMud)
**Author**: Enrico Fraccaroli

**Date**: 29 September 2014

**Language**: C++

## Introduction
The idea is to create a Multi User Dungeon (MUD) set in a post-nuclear world.
Right now there is a minimal content since all the development effort is used to implement new features.

## Requirements
In order to compile the mud the following libraries need to be installed:
 - sqlite3 (For offline data storage).
 - lua (For npc behaviour).
 - zlib (For compressed communication).

## Compiling
In order to compile the mud just create a build directory and the use the cmake file provided.
Therefore, the commands are the following:
```
mkdir build
cd build
[optional] export CXX=clang++
cmake ..
make
```

## Execution
After compiling the mud, just execute the generated executable
```
./RadMud
```

## Connection
I've implemented inside the mud a class called Formatter. Such class provides the commands used to format the text shown as output to the players. Depending on the configuration of this class, the output will be compatible with telnet clients or with an ad-hoc client.

### Connection - Telnet
The mud can be accessed using **localhost** at port **4000**.

### Connection - Graphical client
An ad-hoc client can be used to connect to the mud. Such client can be found at: https://github.com/Galfurian/RadClient
