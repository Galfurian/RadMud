#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/utilities/
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/utilities/CMacroWrapper.cpp
        ./src/utilities/formatter.cpp
        ./src/utilities/table.cpp
        ./src/utilities/logger.cpp
        ./src/utilities/utils.cpp
        PARENT_SCOPE
        )