#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/structure
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/structure/exit.cpp
        ./src/structure/room.cpp
        ./src/structure/area.cpp
        ./src/structure/continent.cpp
        ./src/structure/coordinates.cpp
        ./src/structure/generator.cpp
        PARENT_SCOPE
        )