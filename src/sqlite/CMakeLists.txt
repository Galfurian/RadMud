#############################################
# SQLite Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/sqlite
        PARENT_SCOPE
        )

#############################################
# SQLite Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/sqlite/sqliteDbms.cpp
        ./src/sqlite/tableLoader.cpp
        ./src/sqlite/sqliteWrapper.cpp
        ./src/sqlite/sqliteLoadFunctions.cpp
        PARENT_SCOPE
        )