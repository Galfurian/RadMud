# Plan: Migrate Backend from SQLite to JSON

This document outlines the multi-phase plan to replace the existing SQLite database backend with a JSON-based file system. This will be done carefully to ensure no data is lost during the transition.

## Phase 1: Integration & Serialization

The goal of this phase is to integrate the new JSON library and add the necessary code to serialize the existing game objects to a JSON format, without removing the SQLite code.

1. **Integrate JSON Library:**
    - Modify `CMakeLists.txt` to fetch and link the `Galfurian/json` library using `FetchContent`.

2. **Implement JSON Serialization:**
    - For each core data class (`Item`, `ItemModel`, `Room`, `Area`, `Mobile`, `Player`, `Faction`, `Race`, etc.), implement a `ToJson()` method or an `operator<<` overload that converts the object's state into a `Json::Value`.
    - This will run in parallel with the existing SQLite loading mechanism.

## Phase 2: Data Migration Tool

This phase focuses on creating a one-time tool to perform the migration.

1. **Create a Migration Command:**
    - Implement a new `god` command (e.g., `dump_to_json`).

2. **Dump In-Memory Data:**
    - The command will trigger a function that:
        1. Ensures the MUD has loaded all data from the SQLite database.
        2. Iterates through all the global data containers in the `Mud` singleton (e.g., `mudItems`, `mudRooms`, `mudMobiles`).
        3. Uses the new serialization methods from Phase 1 to convert each object into a `Json::Value`.
        4. Writes the resulting JSON data to separate files (e.g., `system/items.json`, `system/rooms.json`, `system/players.json`).

## Phase 3: Switch to JSON Backend

Once the data has been successfully dumped, we will switch the MUD to use the JSON files as its primary data source.

1. **Implement JSON Deserialization:**
    - For each core data class, implement a `operator>>` overload to populate an object from a `Json::Value`.

2. **Modify Startup Logic:**
    - Change `Mud::initDatabase()` to read the JSON files from the `system/` directory instead of calling `SQLiteDbms::instance().loadTables()`.
    - Use the new deserialization methods to populate the MUD's data containers.

3. **Modify Save Logic:**
    - Change `Mud::saveMud()` and related functions to serialize the current game state back into the JSON files, overwriting them.

4. **Verification:**
    - Run the MUD. If it loads and saves correctly using only the JSON files, this phase is a success. The SQLite database will no longer be read from or written to.

## Phase 4: Cleanup

After verifying that the JSON backend is fully functional, the old SQLite code can be removed.

1. **Remove SQLite Code:**
    - Delete the `database/` directory.
    - Remove the `SQLiteDbms` class and all related function calls.
    - Clean up the `CMakeLists.txt` file to remove any remaining SQLite3 dependencies.

2. **Final Review:**
    - Ensure the codebase is clean and all legacy database code has been successfully purged.
