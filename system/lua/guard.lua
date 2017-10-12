-- Handle the actions when the character is created.
EventInit = function(self)
    -- Put event code here.
    local helm = Mud.loadItem(self, 1500, 10, Mud.random(1, 5))
    if (helm ~= nil) then
        self:equipmentAdd(helm)
    end
    local cuirass = Mud.loadItem(self, 1501, 10, Mud.random(1, 5))
    if (cuirass ~= nil) then
        self:equipmentAdd(cuirass)
    end
    local greave = Mud.loadItem(self, 1502, 10, Mud.random(1, 5))
    if (greave ~= nil) then
        self:equipmentAdd(greave)
    end
    local boots = Mud.loadItem(self, 1503, 10, Mud.random(1, 5))
    if (boots ~= nil) then
        self:equipmentAdd(boots)
    end
end

-- Handle the actions when in combat.
EventFight = function(self, character)
    -- Put event code here.
end

local EventEnterState = 0
local EventEnterCounter = 0
-- Handle the actions when a character enters the room.
EventEnter = function(self, character)
    if (EventEnterState == 0) then
        if (Mud.random(1, 10) < 7) then
            return true
        end
        self:doCommand("look " .. character.name)
    elseif (EventEnterState == 1) then
        self:doCommand("say " .. character.name .. " Stand aside, citizen!")
    elseif (EventEnterState == 2) then
        if (EventEnterCounter >= 3) then
            self:doCommand("say " .. character.name .. " Are you still here?!?")
            EventEnterState = 0
            EventEnterCounter = 0
            return true
        end
        EventEnterCounter = EventEnterCounter + 1
    end
    EventEnterState = EventEnterState + 1
    return false
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