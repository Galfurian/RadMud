#############################################
# Include Directories
set(RADMUD_UTILITIES_INC
        ./src/utilities/
        PARENT_SCOPE
        )

#############################################
# Source Files
set(RADMUD_UTILITIES_SRC
        ./src/utilities/CMacroWrapper.cpp
        ./src/utilities/formatter.cpp
        ./src/utilities/table.cpp
        ./src/utilities/logger.cpp
        ./src/utilities/utils.cpp
        ./src/utilities/aStar.cpp
        ./src/utilities/aStarNode.cpp
        PARENT_SCOPE
        )