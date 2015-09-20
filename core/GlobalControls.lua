local class        = require 'middleclass'
local Controllable = require 'core/Controllable'

local GlobalControls = class('core/GlobalControls')
GlobalControls:include(Controllable)

return GlobalControls
