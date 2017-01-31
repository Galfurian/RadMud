#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/structure/map_generation
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/structure/map_generation/mapCell.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/map_generation/heightMap.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/map_generation/mapGenerator.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/map_generation/mapGeneratorConfiguration.cpp
        PARENT_SCOPE
        )