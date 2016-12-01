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
        Mud.log("[" .. self.name .. "] I need to find an axe.")
        JhonExplorer:reset()
        if (SearchAxe(self)) then
            Mud.log("[" .. self.name .. "] I've found an axe!")
            JhonExplorer:print()
        else
            Mud.log("[" .. self.name .. "] There is no axe in the area!")
            JhonExplorer:print()
            Mud.sleep(15)
        end
    end
    --    Mud.log("[" .. self.name .. "] Now I have an axe.")
    --    if (foundTree == false) then
    --        Mud.log("[" .. self.name .. "] I need to find a suitable tree.")
    --        --SearchTree(self)
    --    else
    --        Mud.log("[" .. self.name .. "] I have found a tree.")
    --    end
    --    if ((posessAxe) and (foundTree)) then
    --        Mud.log("[" .. self.name .. "] Now I can cut down the tree.")
    --    end
end

--- Search an axe everywhere inside the area.
-- @param self The character who is searching for the axe.
SearchAxe = function(self)
    -- Cycle until we find an axe inside the current room.
    while (not SearchAxeRoom(self)) do
        Mud.log("[" .. self.name .. "] Searching for an axe...")
        for roomKey, room in pairs(self:getRoomsInSight()) do
            for itemKey, item in pairs(room:getItems()) do
                -- Check if the room contains an axe.
                if (CheckIfItemIsAnAxe(item)) then
                    Mud.log("[" .. self.name .. "] I've seen an axe, moving to position...")
                    GetToDestination(self, self:luaGetPathTo(item.room))
                    return true
                end
            end
            JhonExplorer:setRoomAsVisited(room)
        end
        JhonExplorer:updateNotVisitedRooms()
        Mud.log("[" .. self.name .. "] I've not found an axe, moving to another position...")
        if (not JhonExplorer.notVisited:empty()) then
            while (not JhonExplorer.notVisited:empty()) do
                local nextRoom = JhonExplorer.notVisited:popfirst()
                if nextRoom == nil then
                    break
                end
                local pathToNextRoom = self:luaGetPathTo(nextRoom)
                if next(pathToNextRoom) ~= nil then
                    Mud.log("[" .. self.name .. "] Moving to " .. nextRoom.vnum .. " ...")
                    GetToDestination(self, pathToNextRoom)
                    break
                else
                    Mud.log("[" .. self.name .. "] Cannot find a path to " .. nextRoom.vnum .. " ...")
                    JhonExplorer.invalidRooms:pushfirst(nextRoom)
                end
            end
        else
            break
        end
        Mud.sleep(4)
    end
    return false
end

--- Search for an axe inside the given room.
-- @param self The character searching for the axe.
SearchAxeRoom = function(self)
    if self.room == nil then
        Mud.stop()
    end
    -- Get the list of items inside the current room.
    for itemKey, item in pairs(self.room:getItems()) do
        if (CheckIfItemIsAnAxe(item)) then
            return item
        end
    end
    return nil
end

--- Try to equip a posessed axe.
-- @param self The character.
-- @return If the character has equipped an axe.
EquipPosessedAxe = function(self)
    for itemKey, item in pairs(self:getEquipmentItems()) do
        if (CheckIfItemIsAnAxe(item)) then
            return true
        end
    end
    for itemKey, item in pairs(self:getInventoryItems()) do
        if (CheckIfItemIsAnAxe(item)) then
            return self:doCommand("wield " .. item.vnum)
        end
    end
    return false
end

--- Check if the given item is an axe.
-- @param item The item to check.
-- @return If the item is an axe.
CheckIfItemIsAnAxe = function(item)
    -- Check the item is a tool.
    if (item:getTypeName() == "Tool") then
        local modelToTool = item.model:toTool()
        -- Transformed to tool.
        local modelToolType = modelToTool.toolType
        -- Retrieved tool type.
        if (modelToolType:toString() == "WoodcutterAxe") then
            -- Found woodcutter axe.
            return true
        end
    end
    return false
end