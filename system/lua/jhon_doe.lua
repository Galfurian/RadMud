--package.path = package.path .. ";../system/lua/luann.lua"
--local luann = require("luann")

local posessAxe = false;
local foundTree = false;

-- Handle the actions when the character is created.
EventInit = function(self)
    -- Put event code here.
end

-- Handle the actions when in combat.
EventFight = function(self, character)
    -- Put event code here.
end

-- Handle the actions when a character enters the room.
EventEnter = function(self, character)
    -- Put event code here.
end

-- Handle the actions when a character exits the room.
EventExit = function(self, character)
    -- Put event code here.
end

-- Handle conversation.
EventMessage = function(self, character, message)
    -- Put event code here.
end

-- Handle a random action.
EventRandom = function(self)
--    Mud.Log("[" .. self.name .. "] Looking around for usefull stuff.");
--    local itemsInSight = self:getItemsInSight();
--    if (not itemsInSight:empty()) then
--        for i = 0, (itemsInSight:size() - 1) do
--            local item = itemsInSight:at(i);
--            Mud.Log("[" .. self.name .. "] I can see :" .. item:getName());
--            local path = self:luaGetPathTo(item.room);
--            if (not path:empty()) then
--                for j = 0, (path:size() - 1) do
--                    Mud.Log("[" .. self.name .. "] " .. path:at(j):toString());
--                end
--            end
--        end
--    end
end

-- Handle the actions when is Morning.
EventMorning = function(self)
    -- Put event code here.
end

-- Handle the actions when is the middle of the day.
EventDay = function(self)
    -- Put event code here.
end

-- Handle the actions when is Evening.
EventDusk = function(self)
    -- Put event code here.
end

-- Handle the actions when is Night.
EventNight = function(self)
    -- Put event code here.
end

-- Handle the actions when the mobile dies.
EventDeath = function(self)
    -- Put event code here.
end

-- Handle the main behaviour of the character.
EventMain = function(self)
    if (posessAxe == false) then
        Mud.Log("[" .. self.name .. "] I need to find an axe.");
        --SearchAxe(self);
    else
        Mud.Log("[" .. self.name .. "] I have an axe.");
        if (foundTree == false) then
            Mud.Log("[" .. self.name .. "] I need to find a suitable tree.");
            --SearchTree(self);
        else
            Mud.Log("[" .. self.name .. "] I have found a tree.");
        end
    end
    if ((posessAxe) and (foundTree)) then
        Mud.Log("[" .. self.name .. "] Now I can cut down the tree.");
    end
end

SearchAxe = function(self)
    while (posessAxe == false) do
        -- Serach the Axe inside the room.
        SearchAxeRoom(self);
        -- If we have found the axe, stop the loop.
        if (posessAxe == true) then
            Mud.Log("[" .. self.name .. "] Ok, I've found an axe.");
            break;
        else
            Mud.Log("[" .. self.name .. "] I've not found an axe, I need to move.");
            -- Get the possible directions.
            local exits = self.room:getExits();
            -- If there are no exits, just stop the loop.
            if (exits:size() == 0) then
                break;
            end
            -- Choose at random among the possible exits.
            local choosenExit = exits:at(Mud.Random(0, exits:size() - 1));
            -- Go to the next direction.
            Mud.Log("[" .. self.name .. "] Moving...");
            self:doCommand(choosenExit:getDirection());
            Mud.Sleep(4);
            Mud.Log("[" .. self.name .. "] Done...");
        end
    end
end

SearchAxeRoom = function(self)
    -- Get the list of items inside the current room.
    local itemList = self.room:getItems();
    if (not itemList:empty())
    then
        for i = 0, (itemList:size() - 1) do
            Mud.Log("Getting index :" .. i);
            local item = itemList:at(i);
            if (item.model:getType() == ModelType.Tool) then
                Mud.Log("[" .. self.name .. "] I've found an axe!");
                self:doCommand("take " .. item.vnum);
                self:doCommand("wield " .. item.vnum);
                posessAxe = true;
            end
        end
    end
end

SearchTree = function(self)
    -- Get the current room.
    local currentRoom = self.room.vnum;
    -- Serach the Tree inside the room.
    SearchTreeRoom(self);
    if (foundTree == true)
    then
        Mud.Log("[" .. self.name .. "] Ok, I've found a tree.");
    else
        Mud.Log("[" .. self.name .. "] I've not found a tree, I need to move.");
        -- Get the possible directions.
        local exits = self.room:getExits();
        if (exits:size() > 0)
        then
            local choosenExit = exits:at(Mud.Random(0, exits:size() - 1));
            -- Go to the next direction.
            self:doCommand(choosenExit:getDirection());
        end
    end
end

SearchTreeRoom = function(self)
    -- Get the list of items inside the current room.
    local itemList = self.room:getItems();
    if (itemList:size() > 0)
    then
        for i = 0, (itemList:size() - 1)
        do
            local item = itemList:at(i);
            if (item.model.vnum == 3100)
            then
                Mud.Log("[" .. self.name .. "] I've found a tree!");
                foundTree = true;
            end
        end
    end
end