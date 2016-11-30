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
--    Mud.Log("[" .. self.name .. "] Searching for someone to follow.");
--    local charactersInSight = self:getCharactersInSight();
--    if (not charactersInSight:empty())
--    then
--        for i = 0, (charactersInSight:size() - 1) do
--            local character = charactersInSight:at(i);
--            if (self.room.vnum ~= character.room.vnum) then
--                GoToCharacter(self, character, self:luaGetPathTo(character.room));
--                break;
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
    -- Put event code here.
end

GoToCharacter = function(self, character, path)
    Mud.log("[" .. self.name .. "] Moving to :" .. character.name);
    if (not path:empty()) then
        for j = 0, (path:size() - 1) do
            self:doCommand(path:at(j):toString());
            Mud.sleep(2);
        end
    end
end