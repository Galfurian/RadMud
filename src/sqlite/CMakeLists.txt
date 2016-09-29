#############################################
# SQLite Include Directories
set(RADMUD_SQLITE_INC
        ./src/sqlite
        PARENT_SCOPE
        )

#############################################
# SQLite Source Files
set(RADMUD_SQLITE_SRC
        ./src/sqlite/sqliteDbms.cpp
        ./src/sqlite/tableLoader.cpp
        ./src/sqlite/sqliteWrapper.cpp
        ./src/sqlite/sqliteLoadFunctions.cpp
        PARENT_SCOPE
        )