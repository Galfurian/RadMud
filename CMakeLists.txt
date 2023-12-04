# -----------------------------------------------------------------------------
# Author: Enrico Fraccaroli
# Date  : 27/01/2018
# -----------------------------------------------------------------------------

# Set the minimum CMake version, the project name and default build type.
cmake_minimum_required(VERSION 3.1...3.18)

# Set the project name.
project(radmud CXX)

# Set the default build type to Debug.
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

# -----------------------------------------------------------------------------
# ENABLE FETCH CONTENT
# -----------------------------------------------------------------------------

# We need this in order to import external projects.
include(FetchContent)
# Hide fetchcontent variables.
mark_as_advanced(FORCE
    FETCHCONTENT_QUIET
    FETCHCONTENT_BASE_DIR
    FETCHCONTENT_FULLY_DISCONNECTED
    FETCHCONTENT_UPDATES_DISCONNECTED
)

# -----------------------------------------------------------------------------
# OPTIONS
# -----------------------------------------------------------------------------

option(STRICT_WARNINGS "Enable strict compiler warnings" ON)
option(WARNINGS_AS_ERRORS "Treat all warnings as errors" OFF)

# -----------------------------------------------------------------------------
# DEPENDENCY (SYSTEM LIBRARIES)
# -----------------------------------------------------------------------------

# For documentation.
find_package(Doxygen)

# Find the required packages.
find_package(Lua REQUIRED)
find_package(SQLite3 REQUIRED)

# -----------------------------------------------------------------------------
# DEPENDENCY
# -----------------------------------------------------------------------------

# Retrieve the source.
FetchContent_Declare(luabridge
    GIT_REPOSITORY "https://github.com/vinniefalco/LuaBridge"
    GIT_TAG master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
# Retrieve the properties related to the content.
FetchContent_GetProperties(luabridge)
# If not populated, make the content available.
if(NOT luabridge_POPULATED)
    message(STATUS "Retrieving `luabridge`...")
    # Ensures the named dependencies have been populated.
    FetchContent_MakeAvailable(luabridge)
    # Hide fetchcontent variables, otherwise with ccmake it's a mess.
    mark_as_advanced(FORCE FETCHCONTENT_UPDATES_DISCONNECTED_LUABRIDGE FETCHCONTENT_SOURCE_DIR_LUABRIDGE)
endif()

# -----------------------------------------------------------------------------
# EXECUTABLE
# -----------------------------------------------------------------------------
add_executable(
    ${PROJECT_NAME}
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
    ${CMAKE_SOURCE_DIR}/src/structure/structureUtils.cpp
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
    ${CMAKE_SOURCE_DIR}/src/utilities/table.cpp
    ${CMAKE_SOURCE_DIR}/src/utilities/logger.cpp
    ${CMAKE_SOURCE_DIR}/src/utilities/utils.cpp
    ${CMAKE_SOURCE_DIR}/src/utilities/name_generator/nameGenerator.cpp
)
# Include the directories.
target_include_directories(
    ${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${LUA_INCLUDE_DIR}
    ${SQLite3_INCLUDE_DIRS}
    ${luabridge_SOURCE_DIR}/Source
)
# Link the libraries.
target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    ${LUA_LIBRARIES}
    ${SQLite3_LIBRARIES}
    pthread
    dl
    z
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

message(INFO ${luabridge_SOURCE_DIR})