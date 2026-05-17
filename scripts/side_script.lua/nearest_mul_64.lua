io.write("Enter an integer: ")
local n = io.read("*n")

if not n then
    io.stderr:write("Error: Invalid numeric input.\n")
    os.exit(1)
end

-- Overflow boundaries for 64-bit signed integers when aligning to 64
if n > 9223372036854775744 or n < -9223372036854775808 then
    io.stderr:write("Error: Input is too large and would cause integer overflow.\n")
    os.exit(1)
end

local remainder = n % 64
local less_than = n - remainder
local more_than = less_than + 64

-- If the input itself is a multiple of 64, adjust so they are strictly less/more
if remainder == 0 then
    less_than = n - 64
    more_than = n + 64
end

print("Nearest multiple less than input: " .. string.format("%.0f", less_than))
print("Nearest multiple more than input: " .. string.format("%.0f", more_than))