io.write("Enter string payload: ")
local user_input = io.read("*l")
if not user_input then return end
user_input = user_input:gsub("%s+", "") -- strip any whitespace

io.write("Enter digit/character capacity: ")
local capacity = io.read("*n")

if not capacity or capacity <= 0 then
    io.stderr:write("Error: Capacity must be a positive integer.\n")
    os.exit(1)
end

local raw_len = #user_input
local final_str = user_input
local overflowed = false

-- If the string is longer than the allowed capacity,
-- slice it from the right side to keep only the lowest bits/digits.
if raw_len > capacity then
    overflowed = true
    -- In Lua, string slicing is 1-indexed.
    -- This extracts the rightmost 'capacity' characters.
    final_str = user_input:sub(raw_len - capacity + 1, raw_len)
end

print("\n----------- String Truncation Simulation -----------")
print("Original String: " .. user_input)
print("Original Length: " .. raw_len)
print("Capacity Limit:  " .. capacity)
print("Truncated Value: " .. final_str)

if overflowed then
    io.stderr:write("(!) WARNING: Value exceeds capacity. Leftmost (most significant) characters were truncated.\n")
end
print("----------------------------------------------------")