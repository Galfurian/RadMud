#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/enumerators
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/enumerators/ability.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/mapTile.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/toolType.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/direction.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/modelType.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/skillRank.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/liquidType.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/telnetChar.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/itemQuality.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/materialType.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/resourceType.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/characterPosture.cpp
        PARENT_SCOPE
        )