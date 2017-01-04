--package.path = package.path .. ";../system/lua/luann.lua"
--local luann = require("luann")
package.path = package.path .. ";../system/lua/lib/?.lua"
require 'explorer'
require 'utils'

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
    if (not EquipPosessedAxe(self)) then
        JhonExplorer:reset()
        local axe = SearchAxe(self);
        if (axe == nil) then
            -- There is no axe in the area!
            Mud.sleep(15)
        else
            -- I've found an axe!
        end
    end
    --    --  Now I have an axe.
    --    if (foundTree == false) then
    --        -- I need to find a suitable tree.
    --        --SearchTree(self)
    --    else
    --        -- I have found a tree.
    --    end
    --    if ((posessAxe) and (foundTree)) then
    --        -- Now I can cut down the tree.
    --    end
end

--- Search an axe everywhere inside the area and return it.
SearchAxe = function(self)
    -- Cycle until we find an axe inside the current room.
    while true do
        -- Search for an axe.
        for roomKey, room in pairs(self:getRoomsInSight()) do
            -- Check if the room contains an axe.
            for itemKey, item in pairs(room:getItems()) do
                -- Check if the item inside the room is an axe.
                if (IsAnAxe(item)) then
                    -- Get the path to the given room.
                    local pathToNextRoom = self:luaGetPathTo(room)
                    -- If the path is not empty, then move to destination.
                    if next(pathToNextRoom) ~= nil then
                        --  Moving to nextRoom
                        if (GetToDestination(self, pathToNextRoom)) then
                            return item
                        else
                            -- Cannot reach an axe in sight!
                            Mud.stop()
                        end
                    else
                        -- Cannot find a path to nextRoom
                        JhonExplorer.invalidRooms:pushfirst(nextRoom)
                    end
                end
            end
            -- If the given room does not contain any axe, set it as visited.
            JhonExplorer:setRoomAsVisited(room)
        end
        -- Update the list of not visited rooms.
        JhonExplorer:updateNotVisitedRooms()
        -- Move to a non-visited room.
        -- I've not found an axe, moving to another position...
        -- If there is no more rooms that need to be visited, stop the algorithm.
        if (JhonExplorer.notVisited:empty()) then
            break
        end
        -- Continue until the list of non-visited rooms is empty.
        while (not JhonExplorer.notVisited:empty()) do
            -- Get the first of the non-visited rooms.
            local nextRoom = JhonExplorer.notVisited:popfirst()
            -- If such room is nil, stop the algorithm.
            if nextRoom == nil then
                break
            end
            -- Get the path to the given room.
            local pathToNextRoom = self:luaGetPathTo(nextRoom)
            -- If the path is not empty, then move to destination.
            if next(pathToNextRoom) ~= nil then
                --  Moving to nextRoom.vnum
                if (GetToDestination(self, pathToNextRoom)) then
                    break
                else
                    --  Cannot reach an axe in sight!
                    Mud.stop()
                end
            else
                --  Cannot find a path to nextRoom
                JhonExplorer.invalidRooms:pushfirst(nextRoom)
            end
        end
        Mud.sleep(4)
    end
    return nil
end

--- Search for an axe inside the given room.
SearchAxeRoom = function(room)
    -- Check if the room is valid.
    if room ~= nil then
        -- Get the list of items inside the current room.
        for key, item in pairs(room:getItems()) do
            if (IsAnAxe(item)) then
                return item
            end
        end
    end
    return nil
end

--- Try to equip a posessed axe.
EquipPosessedAxe = function(self)
    for key, item in pairs(self:getEquipmentItems()) do
        if (IsAnAxe(item)) then
            return true
        end
    end
    for key, item in pairs(self:getInventoryItems()) do
        if (IsAnAxe(item)) then
            return self:doCommand("wield " .. item.vnum)
        end
    end
    return false
end

--- Check if the given item is an axe.
IsAnAxe = function(item)
    -- Check the item is a tool.
    if (item:getTypeName() == "Tool") then
        if (item.model:toTool().toolType:toString() == "WoodcutterAxe") then
            return true
        end
    end
    return false
end