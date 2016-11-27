--package.path = package.path .. ";../system/lua/luann.lua"
--local luann = require("luann")
package.path = package.path .. ";../system/lua/?.lua"
require 'explorer'

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
    EventMain(self);
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
        Mud.Log("[" .. self.name .. "] I need to find an axe.");
        JhonExplorer = Explorer()
        if (SearchAxe(self)) then
            Mud.Log("[" .. self.name .. "] I've found an axe!");
        else
            Mud.Log("[" .. self.name .. "] There is no axe in the area!");
            Mud.Sleep(15);
        end
    end
    --    Mud.Log("[" .. self.name .. "] Now I have an axe.");
    --    if (foundTree == false) then
    --        Mud.Log("[" .. self.name .. "] I need to find a suitable tree.");
    --        --SearchTree(self);
    --    else
    --        Mud.Log("[" .. self.name .. "] I have found a tree.");
    --    end
    --    if ((posessAxe) and (foundTree)) then
    --        Mud.Log("[" .. self.name .. "] Now I can cut down the tree.");
    --    end
end

--- Try to equip a posessed axe.
-- @param self The character.
-- @return If the character has equipped an axe.
EquipPosessedAxe = function(self)
    for itemKey, item in pairs(self:getEquipmentItems()) do
        if (CheckIfItemIsAnAxe(item)) then
            return true;
        end
    end
    for itemKey, item in pairs(self:getInventoryItems()) do
        if (CheckIfItemIsAnAxe(item)) then
            self:doCommand("wield " .. item.vnum);
            return true;
        end
    end
    return false;
end

--- Search an axe everywhere inside the area.
-- @param self The character who is searching for the axe.
SearchAxe = function(self)
    -- Cycle until we find an axe inside the current room.
    while (not SearchAxeRoom(self)) do
        Mud.Log("[" .. self.name .. "] Searching for an axe...");
        for roomKey, room in pairs(self:getRoomsInSight()) do
            for itemKey, item in pairs(room:getItems()) do
                -- Check if the room contains an axe.
                if (CheckIfItemIsAnAxe(item)) then
                    Mud.Log("[" .. self.name .. "] I've seen an axe, moving to position...");
                    GetToDestination(self, self:luaGetPathTo(item.room));
                    return true
                end
            end
            JhonExplorer:setRoomAsVisited(room)
        end
        JhonExplorer:updateNotVisitedRooms()
        Mud.Log("[" .. self.name .. "] I've not found an axe, moving to another position...");
        if (not JhonExplorer.notVisited:empty()) then
            while (not JhonExplorer.notVisited:empty()) do
                local nextRoom = JhonExplorer.notVisited:popfirst()
                if nextRoom == nil then
                    break
                end
                print("Removing room " .. nextRoom.vnum .. " from the NOT visited rooms")
                local pathToNextRoom = self:luaGetPathTo(nextRoom)
                if next(pathToNextRoom) ~= nil then
                    Mud.Log("[" .. self.name .. "] Moving to position " .. nextRoom.vnum .. " ...");
                    GetToDestination(self, pathToNextRoom)
                    break
                else
                    print("There is no valid path to room '" .. nextRoom.vnum .. "'")
                    JhonExplorer.invalidRooms:pushfirst(nextRoom)
                end
            end
        else
            break
        end
        Mud.Sleep(2);
    end
    return false
end

--- Search for an axe inside the given room.
-- @param self The character searching for the axe.
SearchAxeRoom = function(self)
    -- Get the list of items inside the current room.
    for itemKey, item in pairs(self.room:getItems()) do
        if (CheckIfItemIsAnAxe(item)) then
            return item
        end
    end
    return nil;
end

--- Move the character through the given path.
-- @param self The character to move.
-- @param path The path to follow.
GetToDestination = function(self, path)
    for directionKey, direction in pairs(path) do
        print(direction:toString())
        self:doCommand(direction:toString());
        Mud.Sleep(2);
    end
    Mud.Log("[" .. self.name .. "] Reached destination!");
end

--- Check if the given item is an axe.
-- @param item The item to check.
-- @return If the item is an axe.
CheckIfItemIsAnAxe = function(item)
    -- Check the item is a tool.
    if (item:getTypeName() == "Tool") then
        local modelToTool = item.model:toTool();
        -- Transformed to tool.
        local modelToolType = modelToTool.toolType;
        -- Retrieved tool type.
        if (modelToolType:toString() == "WoodcutterAxe") then
            -- Found woodcutter axe.
            return true;
        end
    end
    return false;
end