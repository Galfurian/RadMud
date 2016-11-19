#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/command
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
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