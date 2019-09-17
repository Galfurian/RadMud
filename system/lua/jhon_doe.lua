require("utils")
require("explorer")

local JhonExplorer = Explorer()

--- Handle the actions when the character is created.
-- @param self The character linked to the event.
EventInit = function(self)
    -- Put event code here.
end

--- Handle the actions when in combat.
-- @param self The character linked to the event.
-- @param character The other character.
EventFight = function(self, character)
    -- Put event code here.
end

--- Handle the actions when a character enters the room.
-- @param self The character linked to the event.
-- @param character The other character.
EventEnter = function(self, character)
    -- Put event code here.
end

--- Handle the actions when a character exits the room.
-- @param self The character linked to the event.
-- @param character The other character.
EventExit = function(self, character)
    -- Put event code here.
end

--- Handle conversation.
-- @param self The character linked to the event.
-- @param character The other character.
-- @param message The message which 'character' said to 'self'.
EventMessage = function(self, character, message)
    -- Put event code here.
end

--- Handle a random action.
-- @param self The character linked to the event.
EventRandom = function(self)
    EventMain(self)
end

--- Handle the actions when is Morning.
-- @param self The character linked to the event.
EventMorning = function(self)
    -- Put event code here.
end

--- Handle the actions when is the middle of the day.
-- @param self The character linked to the event.
EventDay = function(self)
    -- Put event code here.
end

--- Handle the actions when is Evening.
-- @param self The character linked to the event.
EventDusk = function(self)
    -- Put event code here.
end

--- Handle the actions when is Night.
-- @param self The character linked to the event.
EventNight = function(self)
    -- Put event code here.
end

--- Handle the actions when the mobile dies.
-- @param self The character linked to the event.
EventDeath = function(self)
    -- Put event code here.
end

--- Handle the main behaviour of the character.
-- @param self The character linked to the event.
EventMain = function(self)
    --    if (not EquipPosessedAxe(self)) then
    --        JhonExplorer:reset()
    --        local axe = SearchAxe(self);
    --        if (axe == nil) then
    --            -- There is no axe in the area!
    --            Mud.sleep(15)
    --        else
    --            -- I've found an axe!
    --            JhonExplorer:reset()
    --            --            local tree = SearchTree(self);
    --            --            if (tree == nil) then
    --            --                -- There is no tree in the area!
    --            --                Mud.sleep(15)
    --            --            else
    --            --                -- I've found a tree!
    --            --                self:doCommand("cut tree");
    --            --            end
    --        end
    --    end
end

--------------------------------------------------------------------------------
--- Search an axe everywhere inside the area and return it.
SearchAxe = function(self)
    -- Cycle until we find an axe inside the current room.
    while true do
        -- Search for an axe nearby.
        if (SearchAxeNearby(self)) then
            return true
        end
        -- Update the list of not visited rooms.
        JhonExplorer:updateNotVisitedRooms()
        -- Move to a non-visited room.
        -- I've not found an axe, moving to another position...
        -- If there is no more rooms that need to be visited, stop the algorithm.
        if (JhonExplorer.notVisited:empty()) then
            return false
        end
        -- Get the first of the non-visited rooms.
        local nextRoom = JhonExplorer.notVisited:popfirst()
        -- If such room is nil, stop the algorithm.
        if nextRoom == nil then
            return false
        end
        if (FindPathAndMoveToDestination(self, nextRoom, 2, 4) == false) then
            --  Cannot find a path to nextRoom
            JhonExplorer.invalidRooms:pushfirst(nextRoom)
        end
        --        Mud.sleep(4)
    end
    return nil
end

--------------------------------------------------------------------------------
--- Search for an axe inside the given room.
SearchAxeNearby = function(self)
    -- Iterate through all the rooms in sight.
    for room in mud.get_rooms_in_sight(self):iter() do
        -- Check if the room contains an axe.
        local axe = SearchAxeRoom(room)
        -- If we have found an axe, move where it is.
        if (axe ~= nil) then
            if (FindPathAndMoveToDestination(self, axe.room, 2, 4)) then
                print("Destination reached")
                if (self:doCommand("take " .. axe.vnum)) then
                    return true
                else
                    print("Cannot take the axe")
                end
            else
                -- Cannot find a path to nextRoom
                print("Unable to get to destination")
                JhonExplorer.invalidRooms:pushfirst(room)
            end
        end
        -- Otherwise, set the room as visited.
        JhonExplorer:setRoomAsVisited(room)
    end
    print("There is no axe nearby!")
    return false
end
--------------------------------------------------------------------------------
--- Search for an axe inside the given room.
SearchAxeRoom = function(room)
    -- Get the list of items inside the current room.
    for item in room.items:iter() do
        if (IsAnAxe(item)) then
            print("Found axe inside room " .. room.vnum)
            return item
        end
    end
    return nil
end
--------------------------------------------------------------------------------
--- Search for a tree inside the given room.
SearchTreeRoom = function(room)
    -- Get the list of items inside the current room.
    for item in room.items:iter() do
        if (IsATree(item)) then
            return item
        end
    end
    return nil
end
--------------------------------------------------------------------------------
--- Check if the given item is an axe.
IsAnAxe = function(item)
    -- Check the item is a tool.
    if (item:getType():toUInt() == ModelType.Tool) then
        print("Is a tool " .. item:getName())
        return (item.model:toTool().toolType:toUInt() == ToolType.WoodcutterAxe)
    end
    return false
end
--------------------------------------------------------------------------------
--- Check if the given item is a tree.
IsATree = function(item)
    -- Check the item is a tree.
    if (item:getType():toUInt() == ModelType.Resource) then
        print("Is a resource " .. item:getName())
        return (item.model:toResource().resourceType:toUInt() == ResourceType.Tree)
    end
    return false
end
--------------------------------------------------------------------------------
--- Try to equip a posessed axe.
EquipPosessedAxe = function(self)
    -- Check if the character already has equipped an axe.
    for item in self.equipment:iter() do
        if (IsAnAxe(item)) then
            return true
        end
    end
    -- Check if the character posesses an axe inside the inventory.
    for item in self.inventory:iter() do
        if (IsAnAxe(item)) then
            return self:doCommand("wield axe")
        end
    end
    return false
end
--------------------------------------------------------------------------------
MoveToLocation = function(self, room)
    print("Moving to location " .. room.vnum)
    -- Get the path to the given room.
    local pathToNextRoom = mud.find_path(self, room)
    -- If the path is not empty, then move to destination.
    if next(pathToNextRoom) ~= nil then
        -- Try to move to the destination.
        if (GetToDestination(self, pathToNextRoom)) then
            return true
        end
        -- Cannot reach an axe in sight!
        mud.stop()
    else
        -- Cannot find a path to nextRoom
        JhonExplorer.invalidRooms:pushfirst(room)
    end
    return false
end
--------------------------------------------------------------------------------
