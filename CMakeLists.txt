# -----------------------------------------------------------------------------
# Author: Enrico Fraccaroli
# Date  : 27/01/2018
# -----------------------------------------------------------------------------
# Check CMAKE Version.
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)

# -----------------------------------------------------------------------------
# Project INFORMATION
# -----------------------------------------------------------------------------
project(RadMud CXX)
SET(PROJECT_NAME RadMud)
SET(PROJECT_SOURCE_DIR src)

# -----------------------------------------------------------------------------
# Project compiler FLAGS
# -----------------------------------------------------------------------------
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-qual")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdisabled-optimization")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-nonliteral")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-security")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-y2k")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wimport")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winit-self")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winline")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winvalid-pch")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-field-initializers")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-format-attribute")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-include-dirs")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpacked")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wredundant-decls")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstack-protector")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-aliasing=2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-default")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-enum")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunreachable-code")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-function")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-label")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-value")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wvariadic-macros")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wwrite-strings")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wuninitialized")

# Debugging flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")

# Enable c++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# If the compiler is GNU, disable noexcept-type warnings.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-noexcept-type")
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

# -----------------------------------------------------------------------------
# Project MODULES
# -----------------------------------------------------------------------------
# Add cmake modules folder.
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Find the required packages.
find_package(Lua REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Sqlite3 REQUIRED)

# -----------------------------------------------------------------------------
# Project INCLUDES
# -----------------------------------------------------------------------------
include_directories(
        ${LUA_INCLUDE_DIR}
        ${ZLIB_INCLUDE_DIR}
        ${SQLITE3_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/action
        ${CMAKE_SOURCE_DIR}/src/action/combat
        ${CMAKE_SOURCE_DIR}/src/action/object
        ${CMAKE_SOURCE_DIR}/src/character
        ${CMAKE_SOURCE_DIR}/src/character/effect
        ${CMAKE_SOURCE_DIR}/src/character/race
        ${CMAKE_SOURCE_DIR}/src/character/skill
        ${CMAKE_SOURCE_DIR}/src/character/behaviour
        ${CMAKE_SOURCE_DIR}/src/command
        ${CMAKE_SOURCE_DIR}/src/command/god
        ${CMAKE_SOURCE_DIR}/src/command/object
        ${CMAKE_SOURCE_DIR}/src/creation
        ${CMAKE_SOURCE_DIR}/src/database
        ${CMAKE_SOURCE_DIR}/src/enumerators
        ${CMAKE_SOURCE_DIR}/src/input
        ${CMAKE_SOURCE_DIR}/src/input/initialization
        ${CMAKE_SOURCE_DIR}/src/item
        ${CMAKE_SOURCE_DIR}/src/item/subitem
        ${CMAKE_SOURCE_DIR}/src/lua
        ${CMAKE_SOURCE_DIR}/src/lua/luabridge
        ${CMAKE_SOURCE_DIR}/src/lua/luabridge/detail
        ${CMAKE_SOURCE_DIR}/src/model
        ${CMAKE_SOURCE_DIR}/src/model/submodel
        ${CMAKE_SOURCE_DIR}/src/structure
        ${CMAKE_SOURCE_DIR}/src/structure/algorithms
        ${CMAKE_SOURCE_DIR}/src/structure/algorithms/AStar
        ${CMAKE_SOURCE_DIR}/src/structure/map_generation
        ${CMAKE_SOURCE_DIR}/src/structure/terrain
        ${CMAKE_SOURCE_DIR}/src/updater
        ${CMAKE_SOURCE_DIR}/src/utilities/
        ${CMAKE_SOURCE_DIR}/src/utilities/name_generator
)

# -----------------------------------------------------------------------------
# Project SOURCES
# -----------------------------------------------------------------------------
set(PROJECT_SRC
        ${CMAKE_SOURCE_DIR}/src/main.cpp
        ${CMAKE_SOURCE_DIR}/src/mud.cpp
        ${CMAKE_SOURCE_DIR}/src/action/generalAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/buildAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/craftAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/moveAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/scoutAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/reloadAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/loadAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/unloadAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/aimAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/combat/combatAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/combat/basicAttack.cpp
        ${CMAKE_SOURCE_DIR}/src/action/combat/chase.cpp
        ${CMAKE_SOURCE_DIR}/src/action/combat/flee.cpp
        ${CMAKE_SOURCE_DIR}/src/action/object/dismemberAction.cpp
        ${CMAKE_SOURCE_DIR}/src/character/bodyPart.cpp
        ${CMAKE_SOURCE_DIR}/src/character/character.cpp
        ${CMAKE_SOURCE_DIR}/src/character/characterUtilities.cpp
        ${CMAKE_SOURCE_DIR}/src/character/characterVector.cpp
        ${CMAKE_SOURCE_DIR}/src/character/mobile.cpp
        ${CMAKE_SOURCE_DIR}/src/character/player.cpp
        ${CMAKE_SOURCE_DIR}/src/character/faction.cpp
        ${CMAKE_SOURCE_DIR}/src/character/combatHandler.cpp
        ${CMAKE_SOURCE_DIR}/src/character/effect/effect.cpp
        ${CMAKE_SOURCE_DIR}/src/character/effect/effectManager.cpp
        ${CMAKE_SOURCE_DIR}/src/character/effect/effectFactory.cpp
        ${CMAKE_SOURCE_DIR}/src/character/race/race.cpp
        ${CMAKE_SOURCE_DIR}/src/character/skill/skill.cpp
        ${CMAKE_SOURCE_DIR}/src/character/skill/skillData.cpp
        ${CMAKE_SOURCE_DIR}/src/character/skill/skillManager.cpp
        ${CMAKE_SOURCE_DIR}/src/command/command.cpp
        ${CMAKE_SOURCE_DIR}/src/command/combat.cpp
        ${CMAKE_SOURCE_DIR}/src/command/communication.cpp
        ${CMAKE_SOURCE_DIR}/src/command/crafting.cpp
        ${CMAKE_SOURCE_DIR}/src/command/manager.cpp
        ${CMAKE_SOURCE_DIR}/src/command/general.cpp
        ${CMAKE_SOURCE_DIR}/src/command/movement.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGod.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGodMud.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGodItem.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGodLiquid.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGodMobile.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGodCreation.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGodCharacter.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGodStructure.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectLightSource.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectManagement.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectContainer.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectCharacter.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectLiquids.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectProcess.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectShop.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectFood.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObject.cpp
        ${CMAKE_SOURCE_DIR}/src/creation/building.cpp
        ${CMAKE_SOURCE_DIR}/src/creation/production.cpp
        ${CMAKE_SOURCE_DIR}/src/creation/liquid.cpp
        ${CMAKE_SOURCE_DIR}/src/creation/material.cpp
        ${CMAKE_SOURCE_DIR}/src/creation/profession.cpp
        ${CMAKE_SOURCE_DIR}/src/database/resultSet.cpp
        ${CMAKE_SOURCE_DIR}/src/database/sqliteDbms.cpp
        ${CMAKE_SOURCE_DIR}/src/database/tableLoader.cpp
        ${CMAKE_SOURCE_DIR}/src/database/sqliteWrapper.cpp
        ${CMAKE_SOURCE_DIR}/src/database/sqliteException.cpp
        ${CMAKE_SOURCE_DIR}/src/database/sqliteLoadFunctions.cpp
        ${CMAKE_SOURCE_DIR}/src/database/sqliteWriteFunctions.cpp
        ${CMAKE_SOURCE_DIR}/src/database/dbFunctionsPlayer.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/baseEnumerator.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/ability.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/toolType.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/knowledge.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/direction.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/modelType.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/skillRank.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/liquidType.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/telnetChar.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/itemQuality.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/materialType.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/resourceType.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/statusModifier.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/combatModifier.cpp
        ${CMAKE_SOURCE_DIR}/src/enumerators/characterPosture.cpp
        ${CMAKE_SOURCE_DIR}/src/input/argument.cpp
        ${CMAKE_SOURCE_DIR}/src/input/argumentHandler.cpp
        ${CMAKE_SOURCE_DIR}/src/input/processInput.cpp
        ${CMAKE_SOURCE_DIR}/src/input/initialization/processInitialization.cpp
        ${CMAKE_SOURCE_DIR}/src/input/initialization/processNewAge.cpp
        ${CMAKE_SOURCE_DIR}/src/input/initialization/processNewAttributes.cpp
        ${CMAKE_SOURCE_DIR}/src/input/initialization/processNewConfirm.cpp
        ${CMAKE_SOURCE_DIR}/src/input/initialization/processNewDescription.cpp
        ${CMAKE_SOURCE_DIR}/src/input/initialization/processNewGender.cpp
        ${CMAKE_SOURCE_DIR}/src/input/initialization/processNewName.cpp
        ${CMAKE_SOURCE_DIR}/src/input/initialization/processNewPassword.cpp
        ${CMAKE_SOURCE_DIR}/src/input/initialization/processNewPasswordConfirm.cpp
        ${CMAKE_SOURCE_DIR}/src/input/initialization/processNewRace.cpp
        ${CMAKE_SOURCE_DIR}/src/input/initialization/processNewStory.cpp
        ${CMAKE_SOURCE_DIR}/src/input/initialization/processNewWeight.cpp
        ${CMAKE_SOURCE_DIR}/src/input/initialization/processPlayerName.cpp
        ${CMAKE_SOURCE_DIR}/src/input/initialization/processPlayerPassword.cpp
        ${CMAKE_SOURCE_DIR}/src/item/item.cpp
        ${CMAKE_SOURCE_DIR}/src/item/writing.cpp
        ${CMAKE_SOURCE_DIR}/src/item/itemFactory.cpp
        ${CMAKE_SOURCE_DIR}/src/item/itemVector.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/shopItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/lightItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/armorItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/corpseItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/currencyItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/resourceItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/magazineItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/containerItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/meleeWeaponItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/rangedWeaponItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/liquidContainerItem.cpp
        ${CMAKE_SOURCE_DIR}/src/lua/lua_script.cpp
        ${CMAKE_SOURCE_DIR}/src/model/itemModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/modelFactory.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/armorModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/bookModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/containerModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/corpseModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/currencyModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/foodModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/furnitureModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/keyModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/lightModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/liquidContainerModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/mechanismModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/nodeModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/projectileModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/resourceModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/ropeModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/seedModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/shieldModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/shopModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/toolModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/vehicleModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/magazineModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/meleeWeaponModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/rangedWeaponModel.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/exit.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/room.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/area.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/generator.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/coordinates.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/roomFactory.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/roomUtilityFunctions.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/map_generation/mapCell.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/map_generation/heightMap.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/map_generation/mapWrapper.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/map_generation/mapGenerator.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/map_generation/mapGeneratorConfiguration.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/terrain/terrain.cpp
        ${CMAKE_SOURCE_DIR}/src/structure/terrain/terrainFactory.cpp
        ${CMAKE_SOURCE_DIR}/src/updater/updater.cpp
        ${CMAKE_SOURCE_DIR}/src/updater/updateInterface.cpp
        ${CMAKE_SOURCE_DIR}/src/utilities/CMacroWrapper.cpp
        ${CMAKE_SOURCE_DIR}/src/utilities/formatter.cpp
        ${CMAKE_SOURCE_DIR}/src/utilities/table.cpp
        ${CMAKE_SOURCE_DIR}/src/utilities/logger.cpp
        ${CMAKE_SOURCE_DIR}/src/utilities/utils.cpp
        ${CMAKE_SOURCE_DIR}/src/utilities/name_generator/nameGenerator.cpp
        )

# -----------------------------------------------------------------------------
# Project EXECUTABLE
# -----------------------------------------------------------------------------
add_executable(
        ${PROJECT_NAME}
        ${PROJECT_SRC}
)

# -----------------------------------------------------------------------------
# Link the Libraries
# -----------------------------------------------------------------------------
target_link_libraries(
        ${PROJECT_NAME}
        ${LUA_LIBRARIES}
        ${SQLITE3_LIBRARY}
        pthread
        dl
        z
)
