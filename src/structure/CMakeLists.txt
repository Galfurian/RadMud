#############################################
# Include Directories
set(RADMUD_STRUCTURE_INC
        ./src/structure/
        ./src/structure/algorithms
        PARENT_SCOPE
        )

#############################################
# Source Files
set(RADMUD_STRUCTURE_SRC
        ./src/structure/exit.cpp
        ./src/structure/room.cpp
        ./src/structure/area.cpp
        ./src/structure/continent.cpp
        ./src/structure/coordinates.cpp
        ./src/structure/generator.cpp
        ./src/structure/algorithms/aStar.cpp
        ./src/structure/algorithms/graphNode.cpp
        ./src/structure/algorithms/aStarNode.cpp
        ./src/structure/algorithms/pathFinder.cpp
        PARENT_SCOPE
        )