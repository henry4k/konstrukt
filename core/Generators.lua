local Generators = {}


local function PermutateIndices( permutations, permutationIndices, i )
    local permutationIndex = permutationIndices[i] + 1
    if permutationIndex <= #permutations[i] then
        permutationIndices[i] = permutationIndex
        return true
    else
        if i > 1 then
            permutationIndices[i] = 1
            return PermutateIndices(permutations, permutationIndices, i-1)
        else
            return false
        end
    end
end

function Generators.permutateAsTable( ... )
    local permutations = {...}

    local permutationIndices = {}
    for i = 1, #permutations do
        permutationIndices[i] = 1
    end

    local permutationCount = 0
    local stop = false

    return function()
        if not stop then
            local permutation = {}
            for i = 1, #permutations do
                permutation[i] = permutations[i][permutationIndices[i]]
            end

            if not PermutateIndices(permutations, permutationIndices, #permutations) then
                stop = true
            end

            permutationCount = permutationCount + 1
            return permutationCount, permutation
        else
            return nil, {}
        end
    end
end

function Generators.permutate( ... )
    local nextf = Generators.permutateAsTable(...)
    return function()
        local i, permutation = nextf()
        return i, table.unpack(permutation)
    end
end


return Generators
