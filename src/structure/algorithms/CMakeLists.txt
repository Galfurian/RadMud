#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/structure/algorithms
        ${CMAKE_SOURCE_DIR}/src/structure/algorithms/AStar
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/structure/algorithms/AStar/aStarNode.cpp
        PARENT_SCOPE
        )