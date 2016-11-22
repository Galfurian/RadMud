#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/support
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/support/ability.cpp
        ${CMAKE_SOURCE_DIR}/src/support/direction.cpp
        ${CMAKE_SOURCE_DIR}/src/support/modelType.cpp
        ${CMAKE_SOURCE_DIR}/src/support/telnetChar.cpp
        ${CMAKE_SOURCE_DIR}/src/support/itemQuality.cpp
        ${CMAKE_SOURCE_DIR}/src/support/equipmentSlot.cpp
        ${CMAKE_SOURCE_DIR}/src/support/characterPosture.cpp
        PARENT_SCOPE
        )