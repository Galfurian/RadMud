#############################################
# Utilities Include Directories
set(RADMUD_UTILITIES_INC
	./src/utilities/
    PARENT_SCOPE
)

#############################################
# Utilities Source Files
set(RADMUD_UTILITIES_SRC
    ./src/utilities/CMacroWrapper.cpp
    ./src/utilities/coordinates.cpp
    ./src/utilities/table.cpp
    PARENT_SCOPE
)