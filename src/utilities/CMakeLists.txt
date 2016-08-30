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
    ./src/utilities/coordinates.cpp
    ./src/utilities/table.cpp
    ./src/utilities/logger.cpp
    PARENT_SCOPE
)