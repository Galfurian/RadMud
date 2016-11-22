#############################################
# SQLite Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/sqlite
        PARENT_SCOPE
        )

#############################################
# SQLite Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/sqlite/sqliteDbms.cpp
        ${CMAKE_SOURCE_DIR}/src/sqlite/tableLoader.cpp
        ${CMAKE_SOURCE_DIR}/src/sqlite/sqliteWrapper.cpp
        ${CMAKE_SOURCE_DIR}/src/sqlite/sqliteLoadFunctions.cpp
        PARENT_SCOPE
        )