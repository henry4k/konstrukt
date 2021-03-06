local assert       = assert
local error        = error
local setmetatable = setmetatable
local getmetatable = getmetatable
local unpack       = table.unpack
local max          = math.max
local pow          = math.pow
local floor        = math.floor
local isBetween    = math.isBetween
local bit32extract = bit32.extract
local bit32replace = bit32.replace
local engine       = require 'engine'

local voxelValueCount = engine.GetVoxelInt32Count()


local VoxelData =
{
    mt = {},
    prototype = {}
}
VoxelData.mt.__index = VoxelData.prototype
setmetatable(VoxelData.prototype, VoxelData.prototype)
setmetatable(VoxelData, VoxelData)


function VoxelData:__call( data )
    local instance = data or {}

    if #instance ~= voxelValueCount then
        if #instance == 0 then
            -- initialize with zeros
            for i = 1, voxelValueCount do
                instance[i] = 0
            end
        else
            error('VoxelData needs to be initialized with exactly '..voxelValueCount..' integer values.')
        end
    end

    return setmetatable(instance, self.mt)
end

function VoxelData:isInstance( v )
    return getmetatable(v) == self.mt
end

function VoxelData.prototype:clone()
    return VoxelData(unpack(self))
end

local function checkBitRange( voxeldata, bitPosition, bitCount )
    local index = floor(bitPosition / 32) + 1
    assert(isBetween(index, 1, #voxeldata), 'Bit position is out of range.')

    local bitOffset = bitPosition - (index-1)*32
    assert(isBetween(bitOffset+bitCount, 1, 32), 'Bit count is out of range.')

    return index, bitOffset
end

function VoxelData.prototype:read( bitPosition, bitCount )
    local index, bitOffset = checkBitRange(self, bitPosition, bitCount)
    return bit32extract(self[index], bitOffset, bitCount)
end

function VoxelData.prototype:write( bitPosition, bitCount, value )
    local index, bitOffset = checkBitRange(self, bitPosition, bitCount)

    assert(value >= 0, 'Value must be positive.')
    local maxValue = pow(2, bitCount)
    if value > maxValue then
        error(string.format('Value %d is too big.  %d bits can store at most %d.',
                            value, bitCount, maxValue))
    end

    self[index] = bit32replace(self[index], value, bitOffset, bitCount)
end

function VoxelData.mt:__newindex( key, value )
    error('Can\'t create new voxel fields.')
end

function VoxelData.mt:__eq( other )
    for i = 1, max(#self, #other) do
        if self[i] ~= other[i] then
            return false
        end
    end
    return true
end

local function formatBlobAsHex( int32Values )
    local charactersPer32Bit = 32/4

    local strings = {}
    for i, v in ipairs(int32Values) do
        local valueString = string.format('%X', v)
        local fillString = string.rep('0', charactersPer32Bit - #valueString)

        -- reverse order to display it as one big number
        strings[#int32Values - i] = fillString .. valueString
    end

    return table.concat(strings, ' ')
end

function VoxelData.mt:__tostring()
    return formatBlobAsHex(self)
end


return VoxelData
