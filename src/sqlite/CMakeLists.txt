#############################################
# SQLite Include Directories
set(RADMUD_SQLITE_INC
	./src/sqlite
    PARENT_SCOPE
)

#############################################
# SQLite Source Files
set(RADMUD_SQLITE_SRC
	./src/sqlite/SQLiteDbms.cpp
	./src/sqlite/SQLiteWrapper.cpp
    PARENT_SCOPE
)