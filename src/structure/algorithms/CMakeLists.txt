#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/structure/algorithms
        ./src/structure/algorithms/AStar
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/structure/algorithms/AStar/aStarNode.cpp
        PARENT_SCOPE
        )