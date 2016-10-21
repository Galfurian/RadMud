#############################################
# Include Directories
set(RADMUD_CHARACTER_INC
        ./src/character
        PARENT_SCOPE
        )

#############################################
# Source Files
set(RADMUD_CHARACTER_SRC
        ./src/character/character.cpp
        ./src/character/mobile.cpp
        ./src/character/player.cpp
        ./src/character/characterContainer.cpp
        ./src/character/effect.cpp
        ./src/character/faction.cpp
        ./src/character/race.cpp
        ./src/character/skill.cpp
        ./src/character/aggression.cpp
        ./src/character/aggressionList.cpp
        PARENT_SCOPE
        )