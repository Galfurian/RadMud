#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/input
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/input/argument.cpp
        ./src/input/argumentHandler.cpp
        ./src/input/processInput.cpp
        ./src/input/processTelnetCommand.cpp
        PARENT_SCOPE
        )