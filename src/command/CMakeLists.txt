#############################################
# Include Directories
set(RADMUD_COMMAND_INC
        ./src/command
        PARENT_SCOPE
        )

#############################################
# Source Files
set(RADMUD_COMMAND_SRC
        ./src/command/argumentHandler.cpp
        ./src/command/argument.cpp
        ./src/command/command.cpp
        ./src/command/combat.cpp
        ./src/command/communication.cpp
        ./src/command/crafting.cpp
        ./src/command/manager.cpp
        ./src/command/general.cpp
        ./src/command/god.cpp
        ./src/command/object.cpp
        PARENT_SCOPE
        )