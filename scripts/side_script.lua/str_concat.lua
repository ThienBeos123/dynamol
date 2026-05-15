-- String merging functions - Uses table.concat()
local function merge_strings(strlist)
    local result = table.concat(strlist, "")
    return result
end

-- ============ Input Handling ============
print("=============== STRING CONCATENATION ===============")
io.write("Enter the amount of concatenated strings: ")
local strcount = tonumber(io.read())
-- End if there is no string to concat
if not strcount then return end
print("================= PER STRING INPUT =================")
local str_table = {}
for i = 1, strcount, 1 do
    io.write("Enter string " .. i .. ": ")
    local curr_str = io.read()
    if not curr_str or curr_str == "" then
        io.stderr:write("\n=========== ERROR ===========")
        error("Error: Empty input. Terminating...")
    end
    table.insert(str_table, curr_str)
end

-- ============ Main Program - Output ============
print("====================== OUTPUT ======================")
print("Concatenated String Output: ")
io.write(merge_strings(str_table) .. '\n')

