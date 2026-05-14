-- Function for the truncated "Summary" view
local function get_truncated(str)
    if #str > 34 then
        return string.sub(str, 1, 12) .. "..." .. string.sub(str, -12)
    end
    return str
end

-- Function for the C-like concatenated view
local function get_concatenated(str)
    local len = #str
    if len <= 64 then
        return '"' .. str .. '"'
    end

    -- Logic: <= 128 chars -> 32 part chunks, else 64 part chunks
    local chunk_size = (len <= 128) and 32 or 64
    local result = ""
    
    for i = 1, len, chunk_size do
        local chunk = string.sub(str, i, i + chunk_size - 1)
        result = result .. "    \"" .. chunk .. "\"\n"
    end
    
    return result
end

-- Main program
print("Enter the string to format:")
local input_str = io.read()

if not input_str or input_str == "" then
    print("Error: Empty input.")
    return
end

print("\n" .. string.rep("=", 40))
print("           FORMATTED OUTPUT")
print(string.rep("=", 40))

-- 1. Truncated Summary
print("SUMMARY VIEW:")
print(get_truncated(input_str))
print(string.rep("-", 40))

-- 2. C-Style Concatenation
print("CONCATENATED VIEW:")
print(get_concatenated(input_str))
print(string.rep("=", 40))