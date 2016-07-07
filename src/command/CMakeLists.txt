#############################################
# Command Include Directories
set(RADMUD_COMMAND_INC
	./src/command
    PARENT_SCOPE
)

#############################################
# Command Source Files
set(RADMUD_COMMAND_SRC
	./src/command/command.cpp
	./src/command/combat.cpp
	./src/command/communication.cpp
	./src/command/crafting.cpp
	./src/command/gathering.cpp
	./src/command/general.cpp
	./src/command/gods.cpp
	./src/command/object.cpp
    PARENT_SCOPE
)