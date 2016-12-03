-- Handle the actions when the character is created.
EventInit = function(self)
    -- Put event code here.
    local helm = self.loadItem(self, 1500, 10, Mud.random(1, 5));
    if (helm ~= nil) then
        if (not self:equipmentAdd(helm)) then return end
    end
    local cuirass = self.loadItem(self, 1501, 10, Mud.random(1, 5));
    if (cuirass ~= nil) then
        if (not self:equipmentAdd(cuirass)) then return end
    end
    local greave = self.loadItem(self, 1502, 10, Mud.random(1, 5));
    if (greave ~= nil) then
        if (not self:equipmentAdd(greave)) then return end
    end
    local boots = self.loadItem(self, 1503, 10, Mud.random(1, 5));
    if (boots ~= nil) then
        if (not self:equipmentAdd(boots)) then return end
    end
end

-- Handle the actions when in combat.
EventFight = function(self, character)
    -- Put event code here.
end

-- Handle the actions when a character enters the room.
EventEnter = function(self, character)
    -- Put event code here.
    if (Mud.random(1, 10) > 7)
    then
        self:doCommand("look " .. character.name);
        Mud.sleep(2);
        self:doCommand("say " .. character.name .. " Stand aside, citizen!")
        if (character:isPlayer()) then
            local variable = character:toPlayer():getVariable("seen_by_guard")
            if variable then
                print("Previous :" .. variable)
            end
            character:toPlayer():setVariable("seen_by_guard", "" .. Mud.random(1, 10))
            print("New : " .. character:toPlayer():getVariable("seen_by_guard"))
        end
    end
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
    -- Put event code here.
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