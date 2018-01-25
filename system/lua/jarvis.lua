package.path = package.path .. ";../system/lua/lib/?.lua"
require 'utils'

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
    if (Mud.random(1, 10) > 5)
    then
        self:doCommand("look " .. character.name);
    end
end

-- Handle the actions when a character exits the room.
EventExit = function(self, character)
    -- Put event code here.
end

-- Handle conversation.
EventMessage = function(self, character, message)
    -- Put event code here.
    if (MessageContains(message, "hi"))
    then
        self:doCommand("say " .. character.name .. " hi to you!");
    end
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