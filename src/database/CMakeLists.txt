#############################################
# SQLite Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/database
        PARENT_SCOPE
        )

#############################################
# SQLite Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/database/resultSet.cpp
        ${CMAKE_SOURCE_DIR}/src/database/sqliteDbms.cpp
        ${CMAKE_SOURCE_DIR}/src/database/tableLoader.cpp
        ${CMAKE_SOURCE_DIR}/src/database/sqliteWrapper.cpp
        ${CMAKE_SOURCE_DIR}/src/database/sqliteException.cpp
        ${CMAKE_SOURCE_DIR}/src/database/sqliteLoadFunctions.cpp
        ${CMAKE_SOURCE_DIR}/src/database/sqliteWriteFunctions.cpp
        ${CMAKE_SOURCE_DIR}/src/database/dbFunctionsPlayer.cpp
        PARENT_SCOPE
        )