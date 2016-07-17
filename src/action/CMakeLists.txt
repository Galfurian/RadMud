#############################################
# Action Include Directories
set(RADMUD_ACTION_INC
	./src/action
    PARENT_SCOPE
)

#############################################
# Action Source Files
set(RADMUD_ACTION_SRC
	./src/action/generalAction.cpp
	./src/action/buildAction.cpp
	./src/action/craftAction.cpp
	./src/action/moveAction.cpp
	./src/action/combatAction.cpp
    PARENT_SCOPE
)